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

    // 配置引脚中断
    pin_int_config(base, pin, cfg->irq_mode);
}

// 引脚中断配置
// GPIOx_EDGE_SEL 配置引脚的中断触发方式，该位置 1 后，上升沿和下降沿均会触发中断
void pin_int_config(GPIO_Type *base, uint8_t pin, pin_irq_mode int_mode)
{
    volatile uint32_t *icr;
    uint32_t icr_offset;
    uint32_t icr_shift;

    icr_offset = pin;

    // 引脚对应的 GPIOx_EDGE_SEL 位置 0，即中断相应方式由 ICR 寄存器控制
    base->EDGE_SEL &= ~(1 << pin);

    if (pin < 16) { // GPIO 组中 0~16 引脚的中断模式由 ICR1 寄存器控制
        icr = &(base->ICR1);
    } else {
        icr = &(base->ICR2); // GPIO 组中 17~31 引脚的中断模式由 ICR2 寄存器控制
        icr_offset -= 16;
    }

    icr_shift = 2 * icr_offset;

    switch (int_mode)
    {
    case IRQ_MODE_LOW:
        *icr &= ~(0x3 << icr_shift);
        break;
    case IRQ_MODE_HIGH:
        *icr = ((*icr & (~(0x3 << icr_shift))) | (0x1 << icr_shift));
        break;
    case IRQ_MODE_REDGE:
        *icr = ((*icr & (~(0x3 << icr_shift))) | (0x2 << icr_shift));
        break;
    case IRQ_MODE_FEDGE:
        *icr |= (0x3 << icr_shift);
        break;
    case IRQ_MODE_RFEDGE:
        base->EDGE_SEL |= (1 << pin);
        break;
    default:
        break;
    }
}

// 引脚中断使能
// IMR 对应位置 1 表示使能该引脚的中断
void pin_int_enable(GPIO_Type *base, uint8_t pin)
{
    base->IMR |= (1 << pin);
}

// 引脚中断禁用
// IMR 对应位置 0 表示禁用该引脚的中断
void pin_int_disable(GPIO_Type *base, uint8_t pin)
{
    base->IMR &= ~(1 << pin);
}

// 清除对应引脚的中断标志
void pin_int_clear(GPIO_Type *base, uint8_t pin)
{
    base->ISR |= (1 << pin);
}
