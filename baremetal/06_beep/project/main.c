#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"

int main(void)
{
    // 使能所有时钟
    clock_enable();

    // 初始化 LED 使用的 GPIO
    led_init();

    // 初始化蜂鸣器
    beep_init();

    while (1)
    {
        // 打开 LED
        led_switch(LED0, ACT_ON);
        // 打开蜂鸣器
        beep_switch(ACT_ON);
        // 延时 500ms
        delay_ms(500);

        // 关闭 LED
        led_switch(LED0, ACT_OFF);
        // 关闭蜂鸣器
        beep_switch(ACT_OFF);
        // 延时 500ms
        delay_ms(500);
    }

    return 0;
}
