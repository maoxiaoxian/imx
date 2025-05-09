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
#include "uart.h"
#include "stdio.h"

int main(void)
{
    static uint8_t led_state = ACT_OFF;
    int a = 0;
    int b = 0;

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
    beep_init();

    // 串口初始化
    uart_init();

    while (1)
    {
        printf("Please Enter Two Number, Separate by Spaces: "); // 输入两个整数
        scanf("%d %d", &a, &b); // 从串口获取输入
        printf("\r\n %d + %d = %d \r\n\r\n", a, b, a + b); // 计算两数之和并通过串口输出结果

        led_state = !led_state;
        led_switch(LED0, led_state);
    }

    return 0;
}
