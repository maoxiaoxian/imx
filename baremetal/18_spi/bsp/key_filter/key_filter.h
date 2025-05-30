#ifndef _KEY_FILTER_H
#define _KEY_FILTER_H

#include "nxp.h"

// 函数定义
void key_filter_init(void);
void key_filter_timer_init(uint32_t val);
void key_filter_timer_stop(void);
void key_filter_timer_start(uint32_t val);
void key_filter_timer_irq_handler(uint32_t gicc_iar, void *opaque);
void gpio1_16_31_irq_handler(uint32_t gicc_iar, void *opaque);

#endif /* _KEY_FILTER_H */
