#ifndef _PWM_H
#define _PWM_H

#include "nxp.h"

/* PWM 背光控制结构体 */
typedef struct bl_dev_t {
    uint8_t duty; /* PWM 占空比 */
} bl_dev_t;

/* 函数声明 */
void pwm1_set_sample(uint16_t val);
void pwm1_set_period(uint16_t val);
void pwm1_set_duty(uint8_t duty);

void pwm1_enable(void);

void pwm1_irq_handler(void);

void backlight_init(void);

#endif /* _PWM_H */
