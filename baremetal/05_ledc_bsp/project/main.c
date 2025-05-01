#include "clk.h"
#include "delay.h"
#include "led.h"

int main(void)
{
    // 使能所有时钟
    clock_enable();

    // 初始化 LED 使用的 GPIO
    led_init();

    while (1)
    {
        // 打开 LED
        led_switch(LED0, ACT_ON);
        delay_ms(500);

        // 关闭 LED
        led_switch(LED0, ACT_OFF);
        delay_ms(500);
    }

    return 0;
}
