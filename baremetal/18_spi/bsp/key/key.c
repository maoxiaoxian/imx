#include "key.h"
#include "gpio.h"
#include "delay.h"

// 按键初始化
void key_init(void)
{
    pin_cfg key_cfg;

    // GPIO1_IO18 功能复用，SION 强制设置引脚为 GPIO 引脚为 0
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);

    // 引脚电气特性
    // 该寄存器中各字段的含义，以及将要设置的值的含义如下：
    // SRE[0]：压摆率，0b0，低压摆率
    // DSE[5:3]：IO 驱动能力，0b000，输入引脚不配置驱动能力
    // SPEED[7:6]：IO 速率，0b10，100MHz
    // ODE[11]：开漏输出使能，0b0，禁用
    // PKE[12]：状态保持器使能，0b1，使能
    // PUE[13]：使用上拉/下拉 or 状态保持器控制引脚，0b1，使用上拉/下拉方式控制
    // PUS[15:14]：上拉/下拉电阻值，0b11，22K 上拉
    // HYS[16]：使能迟滞比较器，0b0，禁用
    // 因此，最终要写入寄存器的值为：0x0000F080
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0x0000F080);

    // 初始化按键的 IO18 引脚
    key_cfg.dir = GPIO_INPUT;
    gpio_init(GPIO1, 18, &key_cfg);
}

// 获取按键值
// 返回 0 表示没有按键按下，其他值表示对应按键按下
uint8_t key_get_val(void)
{
    uint8_t ret = KEY_NONE;
    uint8_t key_act = KEY_UP;

    if ((key_act == KEY_UP) &&
        (pin_read(GPIO1, 18) == KEY_DOWN)) {
            delay(10); // 延时消抖
            key_act = KEY_DOWN; // 标记按键按下

            if (pin_read(GPIO1, 18) == KEY_DOWN) {
                ret = KEY0_VAL;
            }
    } else if (pin_read(GPIO1, 18) == KEY_UP) {
        ret = KEY_NONE;
        key_act = KEY_UP; // 标记按键释放
    }

    return ret;
}
