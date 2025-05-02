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
    LEVEL_LOW,
    LEVEL_HIGH,
} output_level;

// 引脚配置：输入/输出、默认电平
typedef struct pin_cfg {
    pin_dir dir;     // GPIO 功能：输入 or 输出
    uint8_t def_val; // 引脚为输出时的默认输出电平
} pin_cfg;

// 函数定义
void gpio_init(GPIO_Type *base, uint8_t pin, pin_cfg *cfg);
void set_pin_intput(GPIO_Type *base, uint8_t pin);
void set_pin_output(GPIO_Type *base, uint8_t pin);
void pin_output_low(GPIO_Type *base, uint8_t pin);
void pin_output_high(GPIO_Type *base, uint8_t pin);
int pin_read(GPIO_Type *base, uint8_t pin);
void pin_write(GPIO_Type *base, uint8_t pin, output_level level);

#endif /* _GPIO_H */
