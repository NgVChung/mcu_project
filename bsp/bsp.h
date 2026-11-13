/*
 * bsp.h — Board Support Package: pin mapping & system init (SPL V3.5.0)
 * Target: STM32F103C8T6 (Blue Pill), thach anh 8 MHz
 */
#ifndef BSP_H
#define BSP_H

#include "stm32f10x.h"

/* ---- Pin mapping ---- */

/* ADC1 — Micro MAX9814 */
#define BSP_MIC_PORT            GPIOA
#define BSP_MIC_PIN             GPIO_Pin_0      /* PA0 = ADC1_IN0 */
#define BSP_ADC_CHANNEL         ADC_Channel_0

/* SPI1 — MAX7219 + LED matrix 8x8 */
#define BSP_SPI_PORT            GPIOA
#define BSP_SPI_SCK_PIN         GPIO_Pin_5      /* PA5 */
#define BSP_SPI_MOSI_PIN        GPIO_Pin_7      /* PA7 */
#define BSP_SPI_CS_PIN          GPIO_Pin_4      /* PA4, dieu khien bang phan mem */

/* 2 nut nhan (noi GND, pull-up trong, EXTI suon xuong) */
#define BSP_BTN_PORT            GPIOB
#define BSP_BTN_PORT_SOURCE     GPIO_PortSourceGPIOB
#define BSP_BTN_UP_PIN          GPIO_Pin_12     /* PB12: tang nguong */
#define BSP_BTN_UP_PIN_SOURCE   GPIO_PinSource12
#define BSP_BTN_UP_EXTI_LINE    EXTI_Line12
#define BSP_BTN_DOWN_PIN        GPIO_Pin_14     /* PB14: giam nguong */
#define BSP_BTN_DOWN_PIN_SOURCE GPIO_PinSource14
#define BSP_BTN_DOWN_EXTI_LINE  EXTI_Line14
#define BSP_BTN_IRQn            EXTI15_10_IRQn

/* LED canh bao */
#define BSP_LED_PORT            GPIOB
#define BSP_LED_PIN             GPIO_Pin_13     /* PB13, output push-pull */

void bsp_system_init(void);

#endif /* BSP_H */
