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

/* 背景色索引 */
uint32_t backcolor[10] = {
    LCD_BLUE,      LCD_GREEN,    LCD_RED,   LCD_CYAN,  LCD_YELLOW,
    LCD_LIGHTBLUE, LCD_DARKBLUE, LCD_WHITE, LCD_BLACK, LCD_ORANGE
};

int main(void)
{
    static uint8_t led_state = ACT_OFF;
    uint32_t color = 0;
    uint8_t index = 0;

    int_init(); // 中断初始化

    clock_init(); // 初始化 PLL

    delay_init(); // 初始化延迟

    clock_enable(); // 使能所有时钟

    led_init(); // 初始化 LED 使用的 GPIO

    beep_init(); // 初始化蜂鸣器

    uart_init(); // 串口初始化

    lcd_init(); // LCD 初始化

    // 测试屏幕四个角显示是否正常
    lcd_draw_point(1, 0, LCD_GREEN);
    lcd_draw_point(tft_lcd.width - 1, 0, LCD_GREEN);
    lcd_draw_point(1, tft_lcd.height - 1, LCD_GREEN);
    lcd_draw_point(tft_lcd.width - 1, tft_lcd.height - 1, LCD_GREEN);

    // 读取指定位置的颜色
    color = lcd_read_point(1, 0);
    printf("Position (1, 0) color = 0x%x", color);

    delay_ms(1000);

    // 显示字符串
    tft_lcd.forecolor = LCD_BLACK; // 前景色设置为黑色

    while (1)
    {
        lcd_clear(backcolor[index]);
        delay_ms(10);

        lcd_show_string(10, 40, 260, 32, 32, (u8*)"ALPHA IMX6U");
        lcd_show_string(10, 80, 240, 24, 24, (u8*)"RGBLCD TEST");
        lcd_show_string(10, 110, 240, 16, 16, (u8*)"~MaoXian~");

        lcd_draw_circle(140, 200, 50);

        index++;

        index = (index == 10) ? 0 : index;

        led_state = !led_state;
        led_switch(LED0, led_state); // 翻转 LED
        delay_ms(1000); // 延时 1s
    }

    return 0;
}
