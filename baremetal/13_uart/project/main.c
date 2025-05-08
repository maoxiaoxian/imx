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

int main(void)
{
    static uint8_t led_state = ACT_OFF;
    static char c = 0;

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
        put_string("Please Enter a char:\n\r");
        c = get_char(); /* 读取一个字符 */
        put_char(c); /* 回显 */
        put_string("\r\n");

        /* 显示输入的字符 */
        put_string("Your Enter is:\n\r");
        put_char(c);
        put_string("\r\n");

        led_state = !led_state;
        led_switch(LED0, led_state);
    }

    return 0;
}
