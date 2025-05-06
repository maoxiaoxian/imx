#ifndef _EPIT_H
#define _EPIT_H

#include "nxp.h"

// 函数声明
void epit1_init(uint32_t frac, uint32_t val);
void epit1_irq_handler(uint32_t gicc_iar, void *opaque);

#endif /* _EPIT_H */
