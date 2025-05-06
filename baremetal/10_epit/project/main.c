#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "gpio.h"
#include "exit.h"
#include "int.h"
#include "epit.h"

int main(void)
{
    // 中断初始化
    int_init();

    // 初始化 PLL
    clock_init();

    // 使能所有时钟
    clock_enable();

    // 初始化 LED 使用的 GPIO
    led_init();

    // 初始化蜂鸣器
    beep_init();

    // 按键初始化
    key_init();

    // 按键中断初始化
    // exit_init();

    // 初始化 EPIT1 定时器
    // 66MHz 时钟源，1 分频计数值 66,000,000 / 2，即定时 500ms 定时
    epit1_init(0, 66000000 / 2);

    while (1)
    {

    }

    return 0;
}
