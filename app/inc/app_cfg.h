/*
 * app_cfg.h — Bang tai nguyen RTOS & tham so ung dung
 */
#ifndef APP_CFG_H
#define APP_CFG_H

/* ---- Muc uu tien: ADC > BUTTON > SPI ----
 * ADC cao nhat: phai xu ly xong nua bo dem trong 16 ms truoc khi DMA ghi de. */
#define TASK_ADC_PRIO           3
#define TASK_BUTTON_PRIO        2
#define TASK_SPI_PRIO           1

/* ---- Stack (word = 4 byte) ---- */
#define TASK_ADC_STACK          256    /* soft-float sqrtf/log10f */
#define TASK_BUTTON_STACK       128
#define TASK_SPI_STACK          160

/* ---- Task_ADC ---- */
#define ADC_WAIT_TIMEOUT_MS     100    /* khong block vinh vien */
#define DB_AVG_LEN              8      /* trung binh truot 8 block = 128 ms */
#define DB_CAL_OFFSET           15.5f

/* ---- Task_SPI (VU meter) ---- */
#define VU_DB_MIN               40.0f   /* 0 cot  */
#define VU_DB_MAX               100.0f  /* 8 cot  */
#define SPI_REFRESH_MS          20     /* 50 khung/s */
#define VU_FALL_MS              60     /* thanh roi 1 cot moi 60 ms  */
#define VU_PEAK_HOLD_MS         600    /* giu dinh 600 ms            */
#define VU_PEAK_FALL_MS         120U    /* dinh roi 1 cot moi 120 ms  */

/* ---- Task_BUTTON (nguong & canh bao) ---- */
#define BTN_POLL_MS             10
#define BTN_DEBOUNCE_MS         30     /* muc nut on dinh 30 ms moi tinh */
#define ALARM_HOLD_MS           3000   /* vuot nguong lien tuc 3 s */
#define ALARM_THRESH_DEFAULT    80      /* dB */
#define ALARM_THRESH_MIN        30      /* nut giam dung o day */
#define ALARM_THRESH_MAX        120      /* nut tang dung o day */
#define ALARM_THRESH_STEP       5

#endif /* APP_CFG_H */
