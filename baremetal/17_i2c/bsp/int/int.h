#ifndef _INT_H
#define _INT_H

#include "nxp.h"

// 中断服务函数类型定义
typedef void (*irq_handler)(uint32_t gicc_iar, void *opaque);

// 中断服务结构体
typedef struct irq_table_t {
    irq_handler irq_handler; // 中断服务函数
    void *opaque; // 用户参数
} irq_table_t;

// 函数声明
void int_init(void);
void irq_table_init(void);
void irq_handler_register(IRQn_Type irq, irq_handler handler, void *opaque);
void sys_irq_handler(uint32_t gicc_iar);
void default_irq_handler(uint32_t gicc_iar, void *opaque);

#endif /* _INT_H */
