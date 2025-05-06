#include "exit.h"
#include "int.h"
#include "delay.h"
#include "beep.h"
#include "key.h"
#include "gpio.h"

// GPIO 中断初始化
void exit_init(void)
{
    pin_cfg key_cfg;

    // IO18 引脚复用设置为 GPIO1_IO18
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);
    // 设置引脚电气特性
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0x0000F080);

    // 引脚中断配置
    key_cfg.dir = GPIO_INPUT;
    key_cfg.def_val = LEVEL_HIGH;
    key_cfg.irq_mode = IRQ_MODE_FEDGE;
    gpio_init(GPIO1, 18, &key_cfg);

    // GIC 使能中断
    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);

    // 设置 GPIO1_IO18 的中断服务函数
    irq_handler_register(GPIO1_Combined_16_31_IRQn, (irq_handler)gpio1_io18_irq_handler, ((void *)0));

    // 允许 GPIO1_IO18 中断
    pin_int_enable(GPIO1, 18);
}

// GPIO1_IO18 中断处理函数
void gpio1_io18_irq_handler(void)
{
    static uint8_t state = 0;

    delay_ms(10);

    if (pin_read(GPIO1, 18) == KEY_DOWN) {
        state = !state;
        beep_switch(state);
    }

    // 清除中断标准
    pin_int_clear(GPIO1, 18);
}
