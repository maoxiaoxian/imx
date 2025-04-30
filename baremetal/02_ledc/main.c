#include "reg_def.h"

// 使能所有时钟
static void clock_enable(void)
{
    CCM_CCGR0 = CCGR_RST_VAL;
    CCM_CCGR1 = CCGR_RST_VAL;
    CCM_CCGR2 = CCGR_RST_VAL;
    CCM_CCGR3 = CCGR_RST_VAL;
    CCM_CCGR4 = CCGR_RST_VAL;
    CCM_CCGR5 = CCGR_RST_VAL;
    CCM_CCGR6 = CCGR_RST_VAL;
}

// 初始化 LED 对应的 GPIO
static void led_init(void)
{
    // 设置 GPIO1_IO03 功能复用，IO03 功能选择为通用 GPIO
    IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 = 0x00000005;

    // 设置 GPIO1_IO03 电气特性
    // 该寄存器中各字段的含义，以及将要设置的值的含义如下：
    // SRE[0]：压摆率，0b0，低压摆率
    // DSE[5:3]：IO 驱动能力，0b110，R0/6
    // SPEED[7:6]：IO 速率，0b10，100MHz
    // ODE[11]：开漏输出使能，0b0，禁用
    // PKE[12]：状态保持器使能，0b1，使能
    // PUE[13]：使用上拉/下拉 or 状态保持器控制引脚，0b0，使用上拉/下拉方式控制
    // PUS[15:14]：上拉/下拉电阻值，0b00，下拉
    // HYS[16]：使能迟滞比较器，0b0，禁用
    // 因此，最终要写入寄存器的值为：0x10B0
    IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 = 0x000010B0;

    // GPIO1_IO03 设置为输出模式
    GPIO1_GDIR = 0x00000008;

    // 设置 GPIO1_IO03 输出低电平，打开 LED
    GPIO1_DR = 0x00000000;
}

// 打开 LED
static void led_on(void)
{
    // GPIO1_DR 寄存器的 bit3 置 0，打开 LED
    GPIO1_DR &= ~(1 << 3);
}

// 关闭 LED
static void led_off(void)
{
    // GPIO1_DR 寄存器的 bit3 置 1，关闭 LED
    GPIO1_DR |= (1 << 3);
}

// CPU 执行空操作，实现短时间延时
static void delay_short(uint32_t tick)
{
    while (tick--) {}
}

// 延时函数，CPU 主频为 396MHz 时，执行一次耗时约 1ms
static void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        delay_short(0x7FF);
    }
}

// main() 函数
int main(void)
{
    clock_enable(); // 使能所有时钟
    led_init();     // 初始化 LED 对应的 GPIO

    while (1)
    {
        led_off();     // 关闭 LED
        delay_ms(500); // 延时 500ms

        led_on();      // 打开 LED
        delay_ms(500); // 延时 500ms
    }

    return 0;
}
