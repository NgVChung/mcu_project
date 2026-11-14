/*
 * hal_adc.h — ADC1 (PA0) lay mau 8 kHz do TIM3 TRGO kich, DMA1_CH1 vong 256 mau
 * Thread-safe: ISR DMA chi dung xTaskNotifyFromISR() de danh thuc task dang ky.
 */
#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define ADC_SAMPLE_RATE_HZ  8000
#define ADC_BUF_SIZE        256
#define ADC_HALF_SIZE       (ADC_BUF_SIZE / 2)    /* 128 mau = 16 ms */

/* Bit notify gui toi task */
#define ADC_NOTIFY_HALF     1   /* nua dau [0..127] da day   */
#define ADC_NOTIFY_FULL     2   /* nua sau [128..255] da day */

void hal_adc_init(void);

/* Bat dau lay mau; ISR se notify 'task' moi khi mot nua bo dem day */
void hal_adc_start(TaskHandle_t task);

/* Con tro toi nua bo dem: half = 0 (nua dau) hoac 1 (nua sau) */
const volatile uint16_t *hal_adc_half(uint32_t half);

#endif /* HAL_ADC_H */
