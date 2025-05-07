#ifndef _DELAY_H
#define _DELAY_H

#include "nxp.h"

// 函数声明
void delay(volatile uint32_t ms);

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

void delay_init(void);

void gpt1_irq_handler(uint32_t gicc_iar, void *opaque);

#endif /* _DELAY_H */
