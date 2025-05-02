#include "delay.h"
#include "clk.h"
#include "led.h"
#include "beep.h"
#include "key.h"

int main(void)
{
    int i = 0;
    int key_val = 0;
    uint8_t led_state = ACT_OFF;
    uint8_t beep_state = ACT_OFF;

    // 使能所有时钟
    clock_enable();

    // 初始化 LED 使用的 GPIO
    led_init();

    // 初始化蜂鸣器
    beep_init();

    while (1)
    {
        key_val = key_get_val();
        if (key_val) {
            switch (key_val)
            {
            case KEY0_VAL:
                beep_state = !beep_state;
                beep_switch(beep_state);
                break;
            default:
                break;
            }
        }

        i++;
        if (i == 50) {
            i = 0;
            led_state = !led_state;
            led_switch(LED0, led_state);
        }
        delay_ms(10);
    }

    return 0;
}
