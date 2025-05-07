#include "clk.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "gpio.h"
#include "exit.h"
#include "int.h"
#include "epit.h"
#include "key_filter.h"
#include "delay.h"

int main(void)
{
    static uint8_t led_state = ACT_OFF;

    // 中断初始化
    int_init();

    // 初始化 PLL
    clock_init();

    // 初始化延迟
    delay_init();

    // 使能所有时钟
    clock_enable();

    // 初始化 LED 使用的 GPIO
    led_init();

    // 初始化蜂鸣器
    // beep_init();

    // 初始化带有消抖功能的按键
    // key_filter_init();

    while (1)
    {
        led_state = !led_state;
        led_switch(LED0, led_state);
        delay_ms(500);
    }

    return 0;
}
