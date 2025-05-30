#ifndef _GPIO_H
#define _GPIO_H

#include "nxp.h"

// 引脚功能：输入 or 输出
typedef enum pin_dir {
    GPIO_INPUT,
    GPIO_OUTPUT,
} pin_dir;

// 引脚电平
typedef enum output_level {
    LEVEL_LOW  = 0,
    LEVEL_HIGH = 1,
} output_level;

// GPIO 中断触发方式
typedef enum pin_irq_mode {
    IRQ_MODE_NONE   = 0,
    IRQ_MODE_LOW    = 1, // 低电平触发
    IRQ_MODE_HIGH   = 2, // 高电平触发
    IRQ_MODE_REDGE  = 3, // 上升沿触发
    IRQ_MODE_FEDGE  = 4, // 下降沿触发
    IRQ_MODE_RFEDGE = 5, // 上升沿和下降沿均触发
} pin_irq_mode;

// 引脚配置：输入/输出、默认电平
typedef struct pin_cfg {
    pin_dir dir;           // GPIO 功能：输入 or 输出
    uint8_t def_val;       // 引脚为输出时的默认输出电平
    pin_irq_mode irq_mode; // 中断触发方式
} pin_cfg;

// 函数定义
void gpio_init(GPIO_Type *base, uint8_t pin, pin_cfg *cfg);
void set_pin_intput(GPIO_Type *base, uint8_t pin);
void set_pin_output(GPIO_Type *base, uint8_t pin);
void pin_output_low(GPIO_Type *base, uint8_t pin);
void pin_output_high(GPIO_Type *base, uint8_t pin);
int pin_read(GPIO_Type *base, uint8_t pin);
void pin_write(GPIO_Type *base, uint8_t pin, output_level level);

void pin_int_config(GPIO_Type *base, uint8_t pin, pin_irq_mode int_mode);
void pin_int_enable(GPIO_Type *base, uint8_t pin);
void pin_int_disable(GPIO_Type *base, uint8_t pin);
void pin_int_clear(GPIO_Type *base, uint8_t pin);

#endif /* _GPIO_H */
