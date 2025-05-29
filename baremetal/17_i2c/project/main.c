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
#include "lcdapi.h"
#include "rtc.h"
#include "ap3216c.h"

int main(void)
{
    static uint8_t led_state = ACT_OFF;
    uint16_t ir, als, ps;

    int_init();     // 中断初始化
    clock_init();   // 初始化 PLL
    delay_init();   // 初始化延迟
    clock_enable(); // 使能所有时钟
    led_init();     // 初始化 LED 使用的 GPIO
    uart_init();    // 串口初始化
    lcd_init();     // LCD 初始化
    // beep_init();    // 初始化蜂鸣器
    // key_init();     // 按键初始化
    // rtc_init();     // 初始化 RTC

    tft_lcd.forecolor = LCD_RED;
    lcd_show_string(30, 10, 400, 24, 24, (char *)"ALPHA-IMX6UL I2C TEST"); /* 显示字符串 */

    while (ap3216c_init()) /* 检测不到 AP3216C */
    {
        lcd_show_string(30, 130, 200, 16, 16, (char*)"AP3216C Check Failed!");
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, (char*)"Please Check!        ");
        delay_ms(500);
    }

    lcd_show_string(30, 130, 200, 16, 16, (char*)"AP3216C Ready!");
    lcd_show_string(30, 160, 200, 16, 16, (char*)" IR:");
    lcd_show_string(30, 180, 200, 16, 16, (char*)" PS:");
    lcd_show_string(30, 200, 200, 16, 16, (char*)"ALS:");
    tft_lcd.forecolor = LCD_BLUE;

    while (1)
    {
        ap3216c_read_data(&ir, &ps, &als);      /* 读取数据 */
        lcd_show_num(30 + 32, 160, ir, 5, 16);  /* 显示 IR 数据 */
        lcd_show_num(30 + 32, 180, ps, 5, 16);  /* 显示 PS 数据 */
        lcd_show_num(30 + 32, 200, als, 5, 16); /* 显示 ALS 数据 */

        delay_ms(120);
        led_state = !led_state;
        led_switch(LED0, led_state);
    }

    return 0;
}
