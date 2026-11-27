/*
 * task_spi.c — Task_SPI
 * 50 khung/s: doi dB (40..100) -> 0..8 cot, thanh len ngay / roi dan,
 * cham dinh giu 600 ms roi roi dan (hieu ung thanh nhay).
 */
#include "FreeRTOS.h"
#include "task.h"
#include "task_spi.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "hal_spi.h"

static TaskHandle_t xSpiTaskHandle = NULL;

static uint8_t db_to_cols(float db)
{
    if (db <= VU_DB_MIN) return 0U;
    if (db >= VU_DB_MAX) return 8U;
    return (uint8_t)((db - VU_DB_MIN) * 8.0f / (VU_DB_MAX - VU_DB_MIN));
}

static void draw(uint8_t level, uint8_t peak)
{
    uint8_t pattern = (uint8_t)((1U << level) - 1U);
    uint8_t reversed = 0U;
    uint8_t i;

    /* Them vach giu dinh vao mau hien thi. */
    if (peak > 0U) {
        pattern |= (uint8_t)(1U << (peak - 1U));
    }

    /* Dao thu tu 8 cot: bit 0 <-> bit 7, bit 1 <-> bit 6, ... */
    for (i = 0U; i < 8U; i++) {
        reversed = (uint8_t)((reversed << 1U) | (pattern & 1U));
        pattern >>= 1U;
    }

    for (i = 0U; i < 8U; i++) {
        hal_max7219_set_row(i, reversed);
    }
}
static void prvSpiTask(void *pvParameters)
{
    TickType_t last    = xTaskGetTickCount();
    float      db      = 0.0f;
    uint8_t    level   = 0U;
    uint8_t    peak    = 0U;
    uint32_t   fall_ms = 0U;
    uint32_t   peak_ms = 0U;

    (void)pvParameters;

    for (;;) {
        (void)xQueueReceive(xQueueDbToSpi, &db, 0);   /* giu gia tri cu neu chua co moi */
        uint8_t target = db_to_cols(db);

        /* Thanh: len ngay, roi 1 cot moi VU_FALL_MS */
        if (target >= level) {
            level   = target;
            fall_ms = 0U;
        } else if ((fall_ms += SPI_REFRESH_MS) >= VU_FALL_MS) {
            level--;
            fall_ms = 0U;
        }

        /* Dinh: giu VU_PEAK_HOLD_MS, sau do roi 1 cot moi VU_PEAK_FALL_MS */
        if (level >= peak) {
            peak    = level;
            peak_ms = 0U;
        } else if ((peak_ms += SPI_REFRESH_MS) >= VU_PEAK_HOLD_MS) {
            peak--;
            peak_ms = VU_PEAK_HOLD_MS - VU_PEAK_FALL_MS;
        }

        draw(level, peak);
        vTaskDelayUntil(&last, pdMS_TO_TICKS(SPI_REFRESH_MS));
    }
}

BaseType_t task_spi_create(void)
{
    return xTaskCreate(prvSpiTask, "SPI", TASK_SPI_STACK, NULL,
                       TASK_SPI_PRIO, &xSpiTaskHandle);
}
