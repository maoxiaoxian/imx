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
#include "lcd.h"

int main(void)
{
    static uint8_t led_state = ACT_OFF;

    int_init(); // 中断初始化

    clock_init(); // 初始化 PLL

    delay_init(); // 初始化延迟

    clock_enable(); // 使能所有时钟

    led_init(); // 初始化 LED 使用的 GPIO

    beep_init(); // 初始化蜂鸣器

    uart_init(); // 串口初始化

    lcd_init(); // LCD 初始化

    while (1)
    {
        led_state = !led_state;
        led_switch(LED0, led_state); // 翻转 LED
        delay_ms(1000); // 延时 1s
    }

    return 0;
}
