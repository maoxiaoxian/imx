#include "key.h"
#include "int.h"
#include "beep.h"
#include "key_filter.h"
#include "gpio.h"

// 按键初始化：使用 EPIT 定时器中断实现按键消抖
void key_filter_init(void)
{
    pin_cfg key_cfg;

    // 初始化 IO 复用，GPIO1 的 IO18 引脚功能选择为 GPIO1_IO18
    // IOMUXC_SetPinMux() 的最后一个参数为 SION，该位置 1 后，
    // 引脚功能强制为 GPIO1_IO18，同时会忽略 IOMUX 位域中配置的引脚功能
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);

    // 配置 GPIO1_IO18 的电气特性
    // 该寄存器中各字段的含义，以及将要设置的值的含义如下：
    // SRE[0]：压摆率，0b0，低压摆率
    // DSE[5:3]：IO 驱动能力，0b000，关闭引脚输出
    // SPEED[7:6]：IO 速率，0b10，100MHz
    // ODE[11]：开漏输出使能，0b0，禁用
    // PKE[12]：状态保持器使能，0b1，使能
    // PUE[13]：使用上拉/下拉 or 状态保持器控制引脚，0b1，使用上拉/下拉方式控制
    // PUS[15:14]：上拉/下拉电阻值，0b11，22K 上拉
    // HYS[16]：使能迟滞比较器，0b0，禁用
    // 因此，最终要写入寄存器的值为：0xF0B0
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0x0000F080);

    // 初始化 GPIO 引脚中断
    key_cfg.dir = GPIO_INPUT; // 引脚功能为输入
    key_cfg.def_val = LEVEL_HIGH; // 引脚默认输出高电平
    key_cfg.irq_mode = IRQ_MODE_FEDGE; // 下降沿触发中断
    gpio_init(GPIO1, 18, &key_cfg); // 初始化 GPIO

    // GIC 使能引脚中断
    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);

    // 注册中断服务函数
    irq_handler_register(GPIO1_Combined_16_31_IRQn,
                        (irq_handler)gpio1_16_31_irq_handler,
                        ((void *)0));

    // 使能引脚中断
    pin_int_enable(GPIO1, 18);

    // 初始化定时器，定时时长 10ms（66MHz 时钟源）
    key_filter_timer_init(66000000 / 100);
}

// 初始化 EPIT 定时器，默认关闭
void key_filter_timer_init(uint32_t val)
{
    // 清空配置寄存器 CR
    EPIT1->CR = 0;

    // EPIT 配置，CR 寄存器需要配置的位域及含义如下：
    // CLKSRC[25:24]：EPIT 时钟源，0b01，选择 Peripheral Clock = 66MHz
    // RLD[3]：工作模式，0b1，计数器计数到 0 时，从 LR 寄存器重新加载计数值
    // OCIEN[2]：比较中断使能，0b1，使能
    // ENMOD[1]：计数器初始值，0b1，从 LR 寄存器加载初始值
    // EN[0]：EPIT 定时器使能，0b0，初始化时先禁用定时器，不然它会直接开始运行
    // 注意：这里未配置分频系数，默认为 0
    EPIT1->CR = (1 << 24) | (1 << 3) | (1 << 2) | (1 << 1);

    // 设置计数值
    EPIT1->LR = val;

    // 比较寄存器设置，当计数寄存器中的值和比较寄存器中的值相等时产生定时器超时事件
    EPIT1->CMPR = 0;

    // GIC 使能中断
    GIC_EnableIRQ(EPIT1_IRQn);

    // 注册定时器超时中断服务函数
    irq_handler_register(EPIT1_IRQn,
                        (irq_handler)key_filter_timer_irq_handler,
                        ((void *)0));
}

// 关闭定时器
void key_filter_timer_stop(void)
{
    EPIT1->CR &= ~(1 << 0);
}

// 启动定时器
void key_filter_timer_start(uint32_t val)
{
    EPIT1->CR &= ~(1 << 0); // 先关闭定时器
    EPIT1->LR = val;        // 设置定时值
    EPIT1->CR |= (1 << 0);  // 启动定时器
}

// 定时器中断处理函数
void key_filter_timer_irq_handler(uint32_t gicc_iar, void *opaque)
{
    static uint8_t state = ACT_OFF;

    // 检查定时器是否超时
    if (EPIT1->SR & (1 << 0)) {
        key_filter_timer_stop(); // 定时器停止
        if (pin_read(GPIO1, 18) == KEY_DOWN) { // 检测按键是否按下
            state = !state;
            beep_switch(state); // 翻转蜂鸣器
        }
    }

    // 清除中断标志
    EPIT1->SR |= (1 << 0);
}

// 按键中断服务函数
void gpio1_16_31_irq_handler(uint32_t gicc_iar, void *opaque)
{
    // 启动定时器
    key_filter_timer_start(66000000 / 100);

    // 清除 GPIO 中断标志
    pin_int_clear(GPIO1, 18);
}
