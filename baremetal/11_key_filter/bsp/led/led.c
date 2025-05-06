#include "led.h"

// 打开 LED
static void led_on(void)
{
    // GPIO1_DR 寄存器的 bit3 置 0，打开 LED
    GPIO1->DR &= ~(1 << 3);
}

// 关闭 LED
static void led_off(void)
{
    // GPIO1_DR 寄存器的 bit3 置 1，关闭 LED
    GPIO1->DR |= (1 << 3);
}

// 初始化 LED 对应的 GPIO
void led_init(void)
{
    // 设置 GPIO1_IO03 功能复用，IO03 功能选择为通用 GPIO
    // IOMUXC_SetPinMux() 的最后一个参数为 inputOnfield，即寄存器对应的 SION 位
    // 若 SION 置 1，则该引脚强制为 GPIO1_IO03，同时会忽略 MUX_MODE 字段中设置的值
    // 这里传入的参数为 0，因此引脚功能由 IOMUXC_GPIO1_IO03_GPIO1_IO03 定义为 GPIO1_IO03
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);

    // 设置 GPIO1_IO03 电气特性
    // 该寄存器中各字段的含义，以及将要设置的值的含义如下：
    // SRE[0]：压摆率，0b0，低压摆率
    // DSE[5:3]：IO 驱动能力，0b110，R0/6
    // SPEED[7:6]：IO 速率，0b10，100MHz
    // ODE[11]：开漏输出使能，0b0，禁用
    // PKE[12]：状态保持器使能，0b1，使能
    // PUE[13]：使用上拉/下拉 or 状态保持器，0b0，使用状态保持器
    // PUS[15:14]：上拉/下拉电阻值，0b00，下拉
    // HYS[16]：使能迟滞比较器，0b0，禁用
    // 因此，最终要写入寄存器的值为：0x10B0
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0x000010B0);

    // GPIO1_IO03 设置为输出模式
    GPIO1->GDIR |= (1 << 3);

    // 设置 GPIO1_IO03 输出高电平，关闭 LED
    led_off();
}

void led_switch(uint8_t led, uint8_t action)
{
    // 选择对应的 LED
    switch (led)
    {
    case LED0:
        if (action == ACT_ON) {
            led_on();
        } else {
            led_off();
        }
        break;
    default:
        break;
    }
}
