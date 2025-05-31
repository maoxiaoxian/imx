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

    imx6ul_hardfpu_enable(); /* 使能硬件浮点运算 */
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

    while (icm20608_init()) /* 初始化 ICM20608 */
    {
        lcd_show_string(50, 100, 200, 16, 16, (char*)"ICM20608 Check Failed!");
        delay_ms(500);
        lcd_show_string(50, 100, 200, 16, 16, (char*)"Please Check!        ");
        delay_ms(500);
    }

    lcd_show_string(50, 100, 200, 16, 16, (char*)"ICM20608 Ready");

    lcd_show_string(50, 130, 200, 16, 16, (char*)"accel x:");
    lcd_show_string(50, 150, 200, 16, 16, (char*)"accel y:");
    lcd_show_string(50, 170, 200, 16, 16, (char*)"accel z:");
    lcd_show_string(50, 190, 200, 16, 16, (char*)"gyro  x:");
    lcd_show_string(50, 210, 200, 16, 16, (char*)"gyro  y:");
    lcd_show_string(50, 230, 200, 16, 16, (char*)"gyro  z:");
    lcd_show_string(50, 250, 200, 16, 16, (char*)"temp   :");

    lcd_show_string(50 + 181, 130, 200, 16, 16, (char*)"g");
    lcd_show_string(50 + 181, 150, 200, 16, 16, (char*)"g");
    lcd_show_string(50 + 181, 170, 200, 16, 16, (char*)"g");
    lcd_show_string(50 + 181, 190, 200, 16, 16, (char*)"o/s");
    lcd_show_string(50 + 181, 210, 200, 16, 16, (char*)"o/s");
    lcd_show_string(50 + 181, 230, 200, 16, 16, (char*)"o/s");
    lcd_show_string(50 + 181, 250, 200, 16, 16, (char*)"C");

    tft_lcd.forecolor = LCD_BLUE;

    // printf("Ready to Read Data!\r\n");

    while (1)
    {
        // printf("Enter while(1)\r\n");
        icm20608_get_data();

        integer_display(50 + 70, 130, 16, icm20608_dev.accel_x_adc);
        integer_display(50 + 70, 150, 16, icm20608_dev.accel_y_adc);
        integer_display(50 + 70, 170, 16, icm20608_dev.accel_z_adc);
        integer_display(50 + 70, 190, 16, icm20608_dev.gyro_x_adc);
        integer_display(50 + 70, 210, 16, icm20608_dev.gyro_y_adc);
        integer_display(50 + 70, 230, 16, icm20608_dev.gyro_z_adc);
        integer_display(50 + 70, 250, 16, icm20608_dev.temp_adc);

        decimals_display(50 + 70 + 50, 130, 16, icm20608_dev.accel_x_act);
        decimals_display(50 + 70 + 50, 150, 16, icm20608_dev.accel_y_act);
        decimals_display(50 + 70 + 50, 170, 16, icm20608_dev.accel_z_act);
        decimals_display(50 + 70 + 50, 190, 16, icm20608_dev.gyro_x_act);
        decimals_display(50 + 70 + 50, 210, 16, icm20608_dev.gyro_y_act);
        decimals_display(50 + 70 + 50, 230, 16, icm20608_dev.gyro_z_act);
        decimals_display(50 + 70 + 50, 250, 16, icm20608_dev.temp_act);

        delay_ms(120);
        led_state = !led_state;
        led_switch(LED0, led_state);
    }

    return 0;
}
