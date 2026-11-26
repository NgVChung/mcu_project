/*
 * hal_spi.h — SPI1 master 16 bit + driver MAX7219 (LED matrix 8x8)
 * Thread-safe: chi Task_SPI su dung sau khi scheduler chay (1 owner duy nhat).
 */
#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>

void hal_spi_init(void);
void hal_spi_send16(uint16_t frame);

void hal_max7219_init(void);
void hal_max7219_set_row(uint8_t row, uint8_t pattern);   /* row 0..7 */

#endif /* HAL_SPI_H */
