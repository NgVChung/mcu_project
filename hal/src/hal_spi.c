/*
 * hal_spi.c — SPI1 master, khung 16 bit, CPOL=0 CPHA=0, 4.5 MHz (SPL)
 * PA5 = SCK, PA7 = MOSI, PA4 = CS (dieu khien bang phan mem)
 */
#include "hal_spi.h"
#include "bsp.h"

#define MAX7219_REG_DIGIT0      0x01U
#define MAX7219_REG_DECODE      0x09U
#define MAX7219_REG_INTENSITY   0x0AU
#define MAX7219_REG_SCANLIMIT   0x0BU
#define MAX7219_REG_SHUTDOWN    0x0CU
#define MAX7219_REG_TEST        0x0FU

void hal_spi_init(void)
{
    GPIO_InitTypeDef g;
    SPI_InitTypeDef  s;

    g.GPIO_Pin   = BSP_SPI_CS_PIN;
    g.GPIO_Mode  = GPIO_Mode_Out_PP;
    g.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(BSP_SPI_PORT, &g);
    GPIO_SetBits(BSP_SPI_PORT, BSP_SPI_CS_PIN);         /* CS = 1 */

    g.GPIO_Pin   = BSP_SPI_SCK_PIN | BSP_SPI_MOSI_PIN;
    g.GPIO_Mode  = GPIO_Mode_AF_PP;
    g.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(BSP_SPI_PORT, &g);

    /* Master, NSS mem, 16 bit, PCLK2/16 = 4.5 MHz (MAX7219 toi da 10 MHz) */
    SPI_StructInit(&s);
    s.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    s.SPI_Mode              = SPI_Mode_Master;
    s.SPI_DataSize          = SPI_DataSize_16b;
    s.SPI_CPOL              = SPI_CPOL_Low;
    s.SPI_CPHA              = SPI_CPHA_1Edge;
    s.SPI_NSS               = SPI_NSS_Soft;
    s.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    s.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &s);
    SPI_Cmd(SPI1, ENABLE);
}

void hal_spi_send16(uint16_t frame)
{
    GPIO_ResetBits(BSP_SPI_PORT, BSP_SPI_CS_PIN);
    SPI_I2S_SendData(SPI1, frame);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) { }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) { }
    GPIO_SetBits(BSP_SPI_PORT, BSP_SPI_CS_PIN);         /* suon len = chot */
}

static void max7219_write(uint8_t reg, uint8_t data)
{
    hal_spi_send16((uint16_t)((reg << 8) | data));
}

void hal_max7219_init(void)
{
    max7219_write(MAX7219_REG_TEST,      0x00U);   /* tat display test */
    max7219_write(MAX7219_REG_DECODE,    0x00U);   /* khong giai ma BCD */
    max7219_write(MAX7219_REG_SCANLIMIT, 0x07U);   /* quet 8 hang */
    max7219_write(MAX7219_REG_INTENSITY, 0x04U);   /* do sang 0..15 */
    for (uint8_t r = 0; r < 8U; r++) {
        hal_max7219_set_row(r, 0x00U);
    }
    max7219_write(MAX7219_REG_SHUTDOWN,  0x01U);   /* hoat dong */
}

void hal_max7219_set_row(uint8_t row, uint8_t pattern)
{
    if (row < 8U) {
        max7219_write((uint8_t)(MAX7219_REG_DIGIT0 + row), pattern);
    }
}
