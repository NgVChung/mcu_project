/*
 * app_ipc.c — Tao cac doi tuong IPC dung chung (goi truoc vTaskStartScheduler)
 */
#include "app_ipc.h"

QueueHandle_t xQueueDbToSpi    = NULL;
QueueHandle_t xQueueDbToButton = NULL;

BaseType_t app_ipc_init(void)
{
    xQueueDbToSpi    = xQueueCreate(1, sizeof(float));
    xQueueDbToButton = xQueueCreate(1, sizeof(float));

    return (xQueueDbToSpi != NULL && xQueueDbToButton != NULL) ? pdPASS : pdFAIL;
}
