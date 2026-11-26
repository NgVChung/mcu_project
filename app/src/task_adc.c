/*
 * task_adc.c
 * Cho DMA bao nua bo dem day, tinh dB, tinh trung binh truot
 * va ghi ket qua vao hai mailbox cho Task_SPI va Task_BUTTON.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "task_adc.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "audio_dsp.h"
#include "hal_adc.h"

static TaskHandle_t xAdcTaskHandle = NULL;

/* Muc on hien tai (dB), dung de theo doi khi hieu chuan. */
//volatile float g_noise_db = 0.0f;

static void prvAdcTask(void *pvParameters)
{
    float hist[DB_AVG_LEN] = { 0 };
    uint32_t idx = 0;
    uint32_t filled = 0;
    uint32_t half;
    uint32_t i;
    float sum;
    float db;

    (void)pvParameters;

    hal_adc_start(xTaskGetCurrentTaskHandle());

    for (;;) {
        if (xTaskNotifyWait(0, 0, &half,
                            pdMS_TO_TICKS(ADC_WAIT_TIMEOUT_MS)) == pdFALSE) {
            continue;   /* Het thoi gian cho, quay lai cho tiep. */
        }

        hist[idx] = dsp_block_db(hal_adc_half(half),
                                 ADC_HALF_SIZE, DB_CAL_OFFSET);

        idx = (idx + 1) % DB_AVG_LEN;
        if (filled < DB_AVG_LEN) {
            filled++;
        }

        sum = 0.0f;
        for (i = 0; i < filled; i++) {
            sum += hist[i];
        }
        db = sum / (float)filled;

//        g_noise_db = db;
        (void)xQueueOverwrite(xQueueDbToSpi, &db);
        (void)xQueueOverwrite(xQueueDbToButton, &db);
    }
}

BaseType_t task_adc_create(void)
{
    return xTaskCreate(prvAdcTask, "ADC", TASK_ADC_STACK, NULL,
                       TASK_ADC_PRIO, &xAdcTaskHandle);
}
