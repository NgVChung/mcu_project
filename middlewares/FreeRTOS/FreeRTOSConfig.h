/*
 * FreeRTOSConfig.h — Cau hinh FreeRTOS cho STM32F103C8 (Cortex-M3, 72 MHz)
 * Nhom 08. Chi 01 nguoi (Leader) duoc sua file nay.
 */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* ---- Kernel ---- */
#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configCPU_CLOCK_HZ                      (72000000UL)
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                ((uint16_t)128)
#define configTOTAL_HEAP_SIZE                   ((size_t)(8 * 1024))
#define configMAX_TASK_NAME_LEN                 12
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TIME_SLICING                  1
#define configUSE_TASK_NOTIFICATIONS            1

/* ---- Dong bo ---- */
#define configUSE_MUTEXES                       0
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_RECURSIVE_MUTEXES             0
#define configQUEUE_REGISTRY_SIZE               0

/* ---- Hook / kiem tra ---- */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configUSE_MALLOC_FAILED_HOOK            1
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configASSERT(x) do { if ((x) == 0) { taskDISABLE_INTERRUPTS(); for (;;) { } } } while (0)

/* ---- Khong dung ---- */
#define configUSE_CO_ROUTINES                   0
#define configUSE_TIMERS                        0

/* ---- API tuy chon ---- */
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskDelayUntil                 1
#define INCLUDE_vTaskDelete                     0
#define INCLUDE_vTaskSuspend                    0
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1

/* ---- Uu tien ngat Cortex-M3 (STM32F1: 4 bit) ----
 * ISR goi ...FromISR() phai co muc uu tien so >= 5 (5..15). */
#define configPRIO_BITS                                 4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
#define configKERNEL_INTERRUPT_PRIORITY      (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* ---- Gan handler FreeRTOS vao ten vector CMSIS ---- */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
