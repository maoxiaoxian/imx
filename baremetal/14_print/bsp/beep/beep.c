#include "beep.h"

// 关闭蜂鸣器
static void beep_off()
{
    GPIO5->DR |= (1 << 1);
}

// 打开蜂鸣器
static void beep_on()
{
    GPIO5->DR &= ~(1 << 1);
}

// 初始化蜂鸣器对应的 GPIO
void beep_init(void)
{
    // 设置 IO01 引脚的功能服用，设置为 GPIO5_IO01
    // IOMUXC_SetPinMux() 的最后一个参数为 inputOnfield，即寄存器对应的 SION 位
    // 若 SION 置 1，则该引脚强制为 GPIO5_IO01，同时会忽略 MUX_MODE 字段中设置的值
    // 这里传入的参数为 0，因此引脚功能由 IOMUXC_GPIO1_IO03_GPIO5_IO01 定义为 GPIO5_IO01
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0);

    // 设置 GPIO5_IO01 电气特性
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
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0x000010B0);

    // GPIO5_IO01 设置为输出模式
    // GDIR 寄存器控制引脚为输出还是输入，每个位对应一个引脚
    // 这里 IO01 对应 bit-1，将对应位置 1，表示对应引脚为输出模式
    // 对应位为 0 则为输入模式
    GPIO5->GDIR |= (1 << 1);

    // 关闭蜂鸣器
    beep_off();
}

// 设置蜂鸣器打开 or 关闭
void beep_switch(uint8_t action)
{
    if (action == ACT_ON) {
        beep_on();
    } else if (action == ACT_OFF) {
        beep_off();
    }
}
