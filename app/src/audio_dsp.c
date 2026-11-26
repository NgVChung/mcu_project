/*
 * audio_dsp.c — DC removal + RMS + dB cho 1 khoi mau
 */
#include "audio_dsp.h"
#include <math.h>

float dsp_block_db(const volatile uint16_t *samples, uint32_t n, float cal_offset)
{
    uint32_t sum = 0;
    uint64_t sq  = 0;
    int32_t  mean;
    float    rms;

    if (n == 0) {
        return cal_offset;
    }

    for (uint32_t i = 0; i < n; i++) {
        sum += samples[i];
    }
    mean = (int32_t)(sum / n);              /* thanh phan DC (~1.25 V) */

    for (uint32_t i = 0; i < n; i++) {
        int32_t d = (int32_t)samples[i] - mean;
        sq += (uint64_t)((int64_t)d * d);
    }

    rms = sqrtf((float)sq / (float)n);
    if (rms < 1.0f) {
        rms = 1.0f;                         /* tranh log10(0) */
    }
    return 20.0f * log10f(rms) + cal_offset;
}
