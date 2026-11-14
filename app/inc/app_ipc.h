/*
 * app_ipc.h — Co che IPC giua cac task
 *
 *   Task_ADC --(float dB)--> xQueueDbToSpi    --> Task_SPI
 *            \-(float dB)--> xQueueDbToButton --> Task_BUTTON
 *
 * Moi queue dai 1 phan tu (mailbox): Task_ADC dung xQueueOverwrite() nen
 * khong bao gio bi block; ben nhan luon doc duoc gia tri moi nhat.
 */
#ifndef APP_IPC_H
#define APP_IPC_H

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xQueueDbToSpi;
extern QueueHandle_t xQueueDbToButton;

BaseType_t app_ipc_init(void);

#endif /* APP_IPC_H */
