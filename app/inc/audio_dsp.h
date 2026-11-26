/*
 * audio_dsp.h — Tinh muc cuong do am tu mot khoi mau ADC
 * Ham thuan (khong phu thuoc FreeRTOS/phan cung) -> test duoc tren PC.
 */
#ifndef AUDIO_DSP_H
#define AUDIO_DSP_H

#include <stdint.h>

/* Bo DC (tru trung binh), tinh RMS, doi ra dB:
 *   dB = 20*log10(RMS) + cal_offset   (RMS < 1 LSB duoc kep ve 1) */
float dsp_block_db(const volatile uint16_t *samples, uint32_t n, float cal_offset);

#endif /* AUDIO_DSP_H */
