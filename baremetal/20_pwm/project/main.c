#include "clk.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "gpio.h"
#include "int.h"
#include "delay.h"
#include "uart.h"
#include "stdio.h"
#include "lcd.h"
#include "lcdapi.h"
#include "spi.h"
#include "icm20608.h"
#include "gt9147.h"
#include "pwm.h"

/*
 * @description  : 在屏幕指定的位置显示整数
 * @param - x    : X 轴位置
 * @param - y    : Y 轴位置
 * @param - size : 字体大小
 * @param - num  : 要显示的数据
 * @return       : 无
 */
void integer_display(uint16_t x, uint16_t y, uint8_t size, int32_t num)
{
    char buf[200];

    lcd_fill(x, y, x + 50, y + size, tft_lcd.backcolor);

    memset(buf, 0, sizeof(buf));

    if (num < 0)
        sprintf(buf, "-%d", -num);
    else
        sprintf(buf, "%d", num);

    lcd_show_string(x, y, 50, size, size, buf); 
}

/*
 * @description  : 在屏幕指定的位置显示小数数据，比如 5123，显示为 51.23
 * @param - x    : X 轴位置
 * @param - y    : Y 轴位置
 * @param - size : 字体大小
 * @param - num  : 要显示的数据，实际小数扩大 100 倍
 * @return       : 无
 */
void decimals_display(uint16_t x, uint16_t y, uint8_t size, int32_t num)
{
    int32_t integ; /* 整数部分 */
    int32_t fract; /* 小数部分 */
    int32_t uncomptemp = num;
    char buf[200];

    if (num < 0)
        uncomptemp = -uncomptemp;

    integ = uncomptemp / 100;
    fract = uncomptemp % 100;

    memset(buf, 0, sizeof(buf));

    if (num < 0)
        sprintf(buf, "-%d.%d", integ, fract);
    else
        sprintf(buf, "%d.%d", integ, fract);

    lcd_fill(x, y, x + 60, y + size, tft_lcd.backcolor);
    lcd_show_string(x, y, 60, size, size, buf); 
}

/*
 * @description : 使能 I.MX6U 的硬件 NEON 和 FPU
 * @param       : 无
 * @return      : 无
 */
void imx6ul_hardfpu_enable(void)
{
    uint32_t cpacr;
    uint32_t fpexc;

    /* 使能 NEON 和 FPU */
    cpacr = __get_CPACR();

    cpacr = (cpacr & ~(CPACR_ASEDIS_Msk | CPACR_D32DIS_Msk)) |
            (3UL << CPACR_cp10_Pos) | (3UL << CPACR_cp11_Pos);

    __set_CPACR(cpacr);

    fpexc = __get_FPEXC();
    fpexc |= 0x40000000UL;
    __set_FPEXC(fpexc);
}

int main(void)
{
    static uint8_t led_state = ACT_OFF;
    uint8_t duty = 0;
    uint8_t i = 0;
    uint8_t key_val = 0;

    imx6ul_hardfpu_enable(); /* 使能硬件浮点运算 */
    int_init();     // 中断初始化
    clock_init();   // 初始化 PLL
    delay_init();   // 初始化延迟
    clock_enable(); // 使能所有时钟
    led_init();     // 初始化 LED 使用的 GPIO
    uart_init();    // 串口初始化
    lcd_init();     // LCD 初始化
    beep_init();    // 初始化蜂鸣器
    // key_init();     // 按键初始化
    // rtc_init();     // 初始化 RTC
    backlight_init();  // 背光初始化

    tft_lcd.forecolor = LCD_RED;
    lcd_show_string(30, 10, 400, 24, 24, (char *)"ALPHA-IMX6UL Back Light TEST"); /* 显示字符串 */
    lcd_show_string(50, 90, 400, 16, 16, (char*)"PWM Duty:     ");
    tft_lcd.forecolor = LCD_BLUE;

    /* 默认占空比设置为 10% */
    if (tft_lcd.id == ATKVGA)
        duty = 100; // VGA 只能在满输出时才能亮屏
    else
        duty = 10;

    lcd_show_num(50 + 72, 90, duty, 3, 16);
    pwm1_set_duty(duty);

    while (1)
    {
        key_val = key_get_val();
        if (key_val == KEY0_VAL) {
            duty += 10; /* 按键按下，占空比加 10% */

            if (duty > 100) { /* 如果占空比超过 100%，重新从 10% 开始 */
                duty = 10;
            }
            lcd_show_num(50 + 72, 90, duty, 3, 16);
            pwm1_set_duty(duty);
        }


        delay_ms(10);
        i++;

        if (i == 50) {
            i = 0;
            led_state = !led_state;
            led_switch(LED0, led_state);
        }
    }

    return 0;
}
