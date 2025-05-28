#ifndef _LED_H
#define _LED_H

#include "nxp.h"

// LED 编号
typedef enum LED_ID {
    LED0,
} LED_ID;

// 函数声明
void led_init(void);
void led_switch(uint8_t led, uint8_t action);

#endif /* _LED_H */
