#include "gpio.h"

// GPIO 引脚功能设置为输入
// *base 为 GPIO 组的地址
// pin 为该引脚在该 GPIO 组内的编号
void set_pin_intput(GPIO_Type *base, uint8_t pin)
{
    base->GDIR &= ~(1 << pin);
}

// GPIO 引脚功能设置为输出
// *base 为 GPIO 组的地址
// pin 为该引脚在该 GPIO 组内的编号
void set_pin_output(GPIO_Type *base, uint8_t pin)
{
    base->GDIR |= (1 << pin);
}

// 引脚输出低电平
void pin_output_low(GPIO_Type *base, uint8_t pin)
{
    base->DR &= ~(1 << pin);
}

// 引脚输出高电平
void pin_output_high(GPIO_Type *base, uint8_t pin)
{
    base->DR |= (1 << pin);
}

// 读取引脚电平
int pin_read(GPIO_Type *base, uint8_t pin)
{
    return (((base->DR) >> pin) & 0x1);
}

// 引脚输出
// val 为 1 引脚输出高电平，为 0 输出低电平
void pin_write(GPIO_Type *base, uint8_t pin, output_level level)
{
    if (level == LEVEL_LOW) {
        pin_output_low(base, pin);
    } else {
        pin_output_high(base, pin);
    }
}

// GPIO 初始化
// *base 为要初始化的 GPIO 组
// pin 为该引脚在该 GPIO 组内的编号
// cfg 为对该引脚的配置：输入/输出、默认输出电平等
void gpio_init(GPIO_Type *base, uint8_t pin, pin_cfg *cfg)
{
    if (cfg->dir == GPIO_INPUT) {
        set_pin_intput(base, pin);
    } else {
        set_pin_output(base, pin);
        // 如果引脚功能为输出，设置默认的输出电平
        pin_write(base, pin, cfg->def_val);
    }
}
