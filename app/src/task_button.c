/*
 * task_button.c — Task_BUTTON (chu ky 10 ms khi khong cho nut)
 *  - Nut PB12 / PB14 (ngat EXTI + chong rung 30 ms): tang / giam nguong 5 dB,
 *    gioi han 60..95 dB.
 *  - dB > nguong lien tuc 3 s -> bat LED PB13; dB <= nguong -> tat LED.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "task_button.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "hal_gpio.h"

static TaskHandle_t xButtonTaskHandle = NULL;

/* Doc bang debugger (Live Expressions) khi chay tren kit */
volatile int32_t g_alarm_thresh_db = ALARM_THRESH_DEFAULT;

static void prvButtonTask(void *pvParameters)
{
    static const int32_t step[HAL_BTN_COUNT] = { +ALARM_THRESH_STEP, -ALARM_THRESH_STEP };
    TickType_t     last    = xTaskGetTickCount();
    float          db      = 0.0f;
    int32_t        thresh  = ALARM_THRESH_DEFAULT;
    uint8_t        alarm   = 0;
    uint32_t       over_ms = 0;

    (void)pvParameters;

    hal_gpio_button_start(xTaskGetCurrentTaskHandle());

    for (;;) {
        uint32_t bits = 0;

        (void)xTaskNotifyWait(0U, 0xFFFFFFFF, &bits, 0);     /* khong block */

        /* Chi xu ly nut co thong bao tu ISR. */
        for (uint32_t i = 0; i < HAL_BTN_COUNT; i++) {
            hal_btn_t id = (hal_btn_t)i;
            if ((bits & HAL_BTN_NOTIFY(id)) == 0) {
                continue;
            }

            /* Cho chong rung roi doc lai muc nut. */
            vTaskDelay(pdMS_TO_TICKS(BTN_DEBOUNCE_MS));
            if (hal_gpio_button_pressed(id)) {
                thresh += step[id];
                if (thresh > ALARM_THRESH_MAX) {
                    thresh = ALARM_THRESH_MAX;
                }
                if (thresh < ALARM_THRESH_MIN) {
                    thresh = ALARM_THRESH_MIN;
                }
                g_alarm_thresh_db = thresh;
            }

            /* Cho nha nut va kiem tra lai truoc khi mo EXTI. */
            do {
                while (hal_gpio_button_pressed(id)) {
                    vTaskDelay(pdMS_TO_TICKS(BTN_POLL_MS));
                }
                vTaskDelay(pdMS_TO_TICKS(BTN_DEBOUNCE_MS));
            } while (hal_gpio_button_pressed(id));

            hal_gpio_button_rearm(id);
            last = xTaskGetTickCount();
        }

        /* Doc dB sau khi cho nut de lay gia tri moi nhat trong queue. */
        (void)xQueueReceive(xQueueDbToButton, &db, 0);

        /* ---- Canh bao: vuot nguong lien tuc ALARM_HOLD_MS ---- */
        if (db > (float)thresh) {
            if (over_ms < ALARM_HOLD_MS) {
                over_ms += BTN_POLL_MS;
            }
        } else {
            over_ms = 0;
        }

        uint8_t want = (over_ms >= ALARM_HOLD_MS) ? 1U : 0U;
        if (want != alarm) {
            alarm = want;
            hal_gpio_led_write(alarm);
        }

        /* Cho den chu ky tiep theo. */
        vTaskDelayUntil(&last, pdMS_TO_TICKS(BTN_POLL_MS));
    }
}

BaseType_t task_button_create(void)
{
    return xTaskCreate(prvButtonTask, "BUTTON", TASK_BUTTON_STACK, NULL,
                       TASK_BUTTON_PRIO, &xButtonTaskHandle);
}
