/*
 * main.c — He thong phan tich va canh bao muc do on (Nhom 08)
 * STM32F103C8 + FreeRTOS: Task_ADC, Task_SPI, Task_BUTTON
 */
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "hal_spi.h"
#include "app_ipc.h"
#include "task_adc.h"
#include "task_spi.h"
#include "task_button.h"

/* Loi nghiem trong: LED canh bao nhay nhanh (~5 Hz), dung he thong */
static void fatal_error(void)
{
    taskDISABLE_INTERRUPTS();
    hal_gpio_led_write(1);     /* LED sáng liên t?c = có l?i nghiêm tr?ng */
    for (;;) { }                /* d?ng h?n */
}
int main(void)
{
    /* 1. Hardware & HAL init */
    bsp_system_init();
    hal_gpio_init();
    hal_spi_init();
    hal_max7219_init();
    hal_adc_init();         /* chua chay; Task_ADC goi hal_adc_start() */

    /* 2. FreeRTOS IPC object creation */
    if (app_ipc_init() != pdPASS) { fatal_error(); }

    /* 3. Create tasks */
    if (task_adc_create()    != pdPASS) { fatal_error(); }
    if (task_spi_create()    != pdPASS) { fatal_error(); }
    if (task_button_create() != pdPASS) { fatal_error(); }

    /* 4. Start scheduler */
    vTaskStartScheduler();

    /* Chi den day neu khong du heap cho Idle task */
    fatal_error();
}

/* ---- FreeRTOS hooks ---- */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    fatal_error();
}

void vApplicationMallocFailedHook(void)
{
    fatal_error();
}
