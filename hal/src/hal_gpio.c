/*
 * hal_gpio.c — PB13 LED (push-pull), PB12/PB14 nut (pull-up, active low, EXTI suon xuong)
 */
#include "hal_gpio.h"
#include "bsp.h"

typedef struct {
    uint16_t pin;
    uint8_t  pin_source;
    uint32_t exti_line;
} btn_hw_t;

static const btn_hw_t s_btn[HAL_BTN_COUNT] = {
    [HAL_BTN_UP]   = { BSP_BTN_UP_PIN,   BSP_BTN_UP_PIN_SOURCE,   BSP_BTN_UP_EXTI_LINE   },
    [HAL_BTN_DOWN] = { BSP_BTN_DOWN_PIN, BSP_BTN_DOWN_PIN_SOURCE, BSP_BTN_DOWN_EXTI_LINE },
};

static TaskHandle_t s_btn_task = NULL;

/* Bat/tat ngat EXTI cua 1 nut (suon xuong) */
static void btn_exti(hal_btn_t id, FunctionalState state)
{
    EXTI_InitTypeDef e;

    e.EXTI_Line    = s_btn[id].exti_line;
    e.EXTI_Mode    = EXTI_Mode_Interrupt;
    e.EXTI_Trigger = EXTI_Trigger_Falling;
    e.EXTI_LineCmd = state;
    EXTI_Init(&e);
}

void hal_gpio_init(void)
{
    GPIO_InitTypeDef g;
    NVIC_InitTypeDef n;

    g.GPIO_Pin   = BSP_LED_PIN;
    g.GPIO_Mode  = GPIO_Mode_Out_PP;
    g.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(BSP_LED_PORT, &g);
    hal_gpio_led_write(0);

    g.GPIO_Pin  = BSP_BTN_UP_PIN | BSP_BTN_DOWN_PIN;
    g.GPIO_Mode = GPIO_Mode_IPU;                        /* pull-up trong */
    GPIO_Init(BSP_BTN_PORT, &g);

    /* EXTI12, EXTI14 <- port B; line chua bat (doi task san sang) */
    for (uint32_t i = 0; i < HAL_BTN_COUNT; i++) {
        GPIO_EXTILineConfig(BSP_BTN_PORT_SOURCE, s_btn[i].pin_source);
    }

    /* Thap hon DMA (5) va >= MAX_SYSCALL -> duoc goi ...FromISR() */
    n.NVIC_IRQChannel                   = BSP_BTN_IRQn;
    n.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1;
    n.NVIC_IRQChannelSubPriority        = 0;
    n.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&n);
}

void hal_gpio_led_write(uint8_t on)
{
    if (on) {
        GPIO_SetBits(BSP_LED_PORT, BSP_LED_PIN);
    } else {
        GPIO_ResetBits(BSP_LED_PORT, BSP_LED_PIN);
    }
}

uint8_t hal_gpio_button_pressed(hal_btn_t id)
{
    return (GPIO_ReadInputDataBit(BSP_BTN_PORT, s_btn[id].pin) == Bit_RESET) ? 1U : 0U;
}

void hal_gpio_button_start(TaskHandle_t task)
{
    s_btn_task = task;

    taskENTER_CRITICAL();               /* ISR cung sua thanh ghi EXTI */
    for (uint32_t i = 0; i < HAL_BTN_COUNT; i++) {
        EXTI_ClearITPendingBit(s_btn[i].exti_line);     /* bo suon cu */
        btn_exti((hal_btn_t)i, ENABLE);
    }
    taskEXIT_CRITICAL();
}

void hal_gpio_button_rearm(hal_btn_t id)
{
    taskENTER_CRITICAL();
    EXTI_ClearITPendingBit(s_btn[id].exti_line);        /* bo suon rung trong luc khoa */
    btn_exti(id, ENABLE);
    taskEXIT_CRITICAL();
}

void EXTI15_10_IRQHandler(void)
{
    BaseType_t woken = pdFALSE;
    uint32_t   bits  = 0;

    for (uint32_t i = 0; i < HAL_BTN_COUNT; i++) {
        if (EXTI_GetITStatus(s_btn[i].exti_line) != RESET) {
            EXTI_ClearITPendingBit(s_btn[i].exti_line);
            btn_exti((hal_btn_t)i, DISABLE);    /* khoa den khi task chong rung xong */
            bits |= HAL_BTN_NOTIFY(i);
        }
    }
    if (bits && s_btn_task != NULL) {
        xTaskNotifyFromISR(s_btn_task, bits, eSetBits, &woken);
    }
    portYIELD_FROM_ISR(woken);
}
