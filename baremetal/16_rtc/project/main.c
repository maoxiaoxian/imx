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

int main(void)
{
    static uint8_t led_state = ACT_OFF;
    uint8_t key = 0;
    int i = 3, t = 0;
    char buf[160];
    rtc_date_time rtcdate;

    int_init(); // 中断初始化

    clock_init(); // 初始化 PLL

    delay_init(); // 初始化延迟

    clock_enable(); // 使能所有时钟

    led_init(); // 初始化 LED 使用的 GPIO

    beep_init(); // 初始化蜂鸣器

    uart_init(); // 串口初始化

    key_init(); // 按键初始化

    lcd_init(); // LCD 初始化

    rtc_init(); // 初始化 RTC

    tft_lcd.forecolor = LCD_RED;
    lcd_show_string(50, 10, 400, 24, 24, (char *)"ALPHA-IMX6UL RTC TEST"); /* 显示字符串 */
    lcd_show_string(50, 40, 200, 16, 16, (char *)"MaoXian");
    lcd_show_string(50, 60, 200, 16, 16, (char *)"2019/03/01");
    tft_lcd.forecolor = LCD_BLUE;
    memset(buf, 0, sizeof(buf));

    while (1)
    {
        if (t == 100) { // 1s 时间到了
            t = 0;

            printf("will be running %d s......\r", i);

            lcd_fill(50, 90, 370, 110, tft_lcd.backcolor); /* 清屏 */
            sprintf(buf, "will be running %ds......", i);
            lcd_show_string(50, 90, 300, 16, 16, buf);

            i--;
            if (i < 0)
                break;
        }

        // 倒计时 3 秒内按下按键，设置时间日期
        key = key_get_val();
        if (key == KEY0_VAL) { // 设置时间
            rtcdate.year = 2018;
            rtcdate.month = 1;
            rtcdate.day = 15;
            rtcdate.hour = 16;
            rtcdate.minute = 23;
            rtcdate.second = 0;
            rtc_set_date_time(&rtcdate); /* 初始化时间和日期 */
            printf("\r\n RTC Init finish\r\n");
            break;
        }

        delay_ms(10);
        t++;
    }

    tft_lcd.forecolor = LCD_RED;
    lcd_fill(50, 90, 370, 110, tft_lcd.backcolor); /* 清屏 */
    lcd_show_string(50, 90, 200, 16, 16, (char *)"Current Time:"); /* 显示字符串 */
    tft_lcd.forecolor = LCD_BLUE;

    while(1)
    {
        rtc_get_datetime(&rtcdate);
        sprintf(buf,"%d/%d/%d %d:%d:%d",rtcdate.year, rtcdate.month, rtcdate.day, rtcdate.hour, rtcdate.minute, rtcdate.second);
        lcd_fill(50,110, 300,130, tft_lcd.backcolor);
        lcd_show_string(50, 110, 250, 16, 16,(char *)buf); /* 显示字符串 */

        led_state = !led_state;
        led_switch(LED0, led_state);
        delay_ms(1000); /* 延时一秒 */
    }

    return 0;
}
