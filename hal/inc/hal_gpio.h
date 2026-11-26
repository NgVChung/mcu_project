/*
 * hal_gpio.h — GPIO driver (SPL): LED canh bao, 2 nut nhan (EXTI)
 * Thread-safe: LED dung GPIO_SetBits/ResetBits (BSRR/BRR, ghi nguyen tu), nut chi doc IDR,
 *              ISR EXTI chi dung xTaskNotifyFromISR(), bat/tat line EXTI trong critical section.
 */
#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    HAL_BTN_UP = 0,         /* PB12: tang nguong */
    HAL_BTN_DOWN,           /* PB14: giam nguong */
    HAL_BTN_COUNT
} hal_btn_t;

/* Bit notify ISR gui toi task khi co suon xuong tren nut 'id' */
#define HAL_BTN_NOTIFY(id)      (1UL << (id))

void    hal_gpio_init(void);
void    hal_gpio_led_write(uint8_t on);
uint8_t hal_gpio_button_pressed(hal_btn_t id);     /* 1 = dang nhan */

/* Mo ngat EXTI 2 nut; ISR notify 'task' va KHOA ngat cua nut do
 * cho den khi task chong rung xong va goi hal_gpio_button_rearm(). */
void    hal_gpio_button_start(TaskHandle_t task);
void    hal_gpio_button_rearm(hal_btn_t id);

#endif /* HAL_GPIO_H */
