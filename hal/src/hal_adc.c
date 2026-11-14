/*
 * hal_adc.c — TIM3 (8 kHz TRGO) -> ADC1 CH0 -> DMA1 CH1 (circular, HT/TC IRQ), SPL
 * CPU khong tham gia lay mau; chi bi ngat 2 lan / 32 ms.
 */
#include "hal_adc.h"
#include "bsp.h"

static volatile uint16_t s_buf[ADC_BUF_SIZE];
static TaskHandle_t      s_notify_task = NULL;

void hal_adc_init(void)
{
    GPIO_InitTypeDef        gpio;
    TIM_TimeBaseInitTypeDef timer;
    ADC_InitTypeDef         adc;
    DMA_InitTypeDef         dma;
    NVIC_InitTypeDef        nvic;

    /* ---- PA0 analog ---- */
    gpio.GPIO_Pin   = BSP_MIC_PIN;
    gpio.GPIO_Mode  = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(BSP_MIC_PORT, &gpio);

    /* ---- TIM3: clock 72 MHz (APB1 /2 -> x2), 72 MHz / 9000 = 8 kHz, TRGO = update ---- */
    TIM_TimeBaseStructInit(&timer);
    timer.TIM_Prescaler   = 0;
    timer.TIM_Period      = (72000000 / ADC_SAMPLE_RATE_HZ) - 1;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &timer);
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    /* ---- ADC1: 1 kenh, kich ngoai TIM3 TRGO, 28.5 cycle ~ 3.4 us ---- */
    adc.ADC_Mode               = ADC_Mode_Independent;
    adc.ADC_ScanConvMode       = DISABLE;
    adc.ADC_ContinuousConvMode = DISABLE;
    adc.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_T3_TRGO;
    adc.ADC_DataAlign          = ADC_DataAlign_Right;
    adc.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc);
    ADC_RegularChannelConfig(ADC1, BSP_ADC_CHANNEL, 1, ADC_SampleTime_28Cycles5);
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)) { }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)) { }
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);

    /* ---- DMA1 CH1: ADC1->DR -> s_buf, 16 bit, vong ---- */
    DMA_DeInit(DMA1_Channel1);
    dma.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dma.DMA_MemoryBaseAddr     = (uint32_t)s_buf;
    dma.DMA_DIR                = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize         = ADC_BUF_SIZE;
    dma.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    dma.DMA_Mode               = DMA_Mode_Circular;
    dma.DMA_Priority           = DMA_Priority_High;
    dma.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dma);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, ENABLE);

    /* Muc uu tien phai >= configMAX_SYSCALL (so lon hon = thap hon) moi duoc goi FromISR */
    nvic.NVIC_IRQChannel                   = DMA1_Channel1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY;
    nvic.NVIC_IRQChannelSubPriority        = 0;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

void hal_adc_start(TaskHandle_t task)
{
    s_notify_task = task;
    DMA_Cmd(DMA1_Channel1, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

const volatile uint16_t *hal_adc_half(uint32_t half)
{
    return &s_buf[half ? ADC_HALF_SIZE : 0];
}

void DMA1_Channel1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (DMA_GetITStatus(DMA1_IT_HT1) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_HT1);

        if (s_notify_task != NULL) {
            xTaskNotifyFromISR(s_notify_task, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        }
    }

    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);

        if (s_notify_task != NULL) {
            xTaskNotifyFromISR(s_notify_task, 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
