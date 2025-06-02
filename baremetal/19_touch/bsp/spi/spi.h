#ifndef _SPI_H
#define _SPI_H

#include "nxp.h"

/* 函数声明 */
void spi_init(ECSPI_Type *base);
uint8_t spi_ch0_rw_byte(ECSPI_Type *base, uint8_t data);

#endif /* _SPI_H */
