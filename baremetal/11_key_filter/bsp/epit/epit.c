#include "epit.h"
#include "int.h"
#include "led.h"
#include "cc.h"

// IMX6ULL 包含两个 EPIT 定时器，初始化定时器 EPIT_1
// EPIT 向下计数器，时钟源 ipg_clk = 66MHz
// frac 为分频值，范围为 0~4095，对应分频值 1~ 4096
// val 为用户设置的计数值
void epit1_init(uint32_t frac, uint32_t val)
{
    // 防止分频系数溢出
    frac = frac < 0xFFF ? frac : 0xFFF;

    // 清空配置寄存器 CR
    EPIT1->CR = 0;

    // EPIT 配置，CR 寄存器需要配置的位域及含义如下：
    // CLKSRC[25:24]：EPIT 时钟源，0b01，选择 Peripheral Clock = 66MHz
    // PRESCALAR[15:4]：分频系数，frac，由用户定义
    // RLD[3]：工作模式，0b1，计数器计数到 0 时，从 LR 寄存器重新加载计数值
    // OCIEN[2]：比较中断使能，0b1，使能
    // ENMOD[1]：计数器初始值，0b1，从 LR 寄存器加载初始值
    // EN[0]：EPIT 定时器使能，0b0，初始化时先禁用定时器，不然它会直接开始运行
    EPIT1->CR = (1 << 24) | (frac << 4) | (1 << 3) | (1 << 2) | (1 << 1);

    // 用户设置的计数值
    EPIT1->LR = val;

    // 设置比较寄存器值，当计数器的值和此寄存器值相等时产生中断
    EPIT1->CMPR = 0;

    // 使能 GIC 对应的中断
    GIC_EnableIRQ(EPIT1_IRQn);

    // 注册中断服务函数
    irq_handler_register(EPIT1_IRQn, (irq_handler)epit1_irq_handler, ((void *)0));

    // EPIT 定时器启动
    EPIT1->CR |= (1 << 0);
}

// EPIT 定时器中断处理函数
void epit1_irq_handler(uint32_t gicc_iar, void *opaque)
{
    // 注意!!! 这里必须要使用静态变量!!!
    // 否则每次中断产生并进入该函数时，state 都会被重新初始化为 ACT_OFF
    // 只有当该变量为静态变量时，该变量才不会被重新赋值
    static uint8_t state = ACT_OFF;

    state = !state;

    // 判断是否有比较事件发生
    if (EPIT1->SR & (1 << 0)) {
        // 如果有事件产生，说明定时器超时，此时翻转 LED
        led_switch(LED0, state);
    }

    // 清除中断标志（比较事件）
    EPIT1->SR |= (1 << 0);
}
