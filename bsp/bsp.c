/*
 * bsp.c — Bat clock ngoai vi, ADC prescaler, NVIC group (SPL V3.5.0)
 * Clock he thong 72 MHz do SystemInit() (system_stm32f10x.c, SYSCLK_FREQ_72MHz)
 * dat truoc khi vao main: HSE 8 MHz x9, AHB 72, APB1 36, APB2 72 MHz.
 */
#include "bsp.h"

/* Link voi -nostartfiles (khong crt0/crti): startup ST goi __libc_init_array()
 * can _init(). Khong co constructor C++ nen de trong. */
void _init(void);
void _init(void) { }

void bsp_system_init(void)
{
    /* SystemInit() bo qua im lang neu HSE khong chay -> dung tai day thay vi
     * chay 8 MHz sai thoi gian (dung debugger se thay treo o dong nay). */
    while (RCC_GetSYSCLKSource() != 0x08U) { }          /* 0x08 = PLL */

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                   /* ADC 12 MHz (toi da 14) */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB
                         | RCC_APB2Periph_AFIO  | RCC_APB2Periph_ADC1
                         | RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    /* TIM3 kich ADC 8 kHz */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* FreeRTOS yeu cau 4 bit preempt, 0 bit sub-priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}
