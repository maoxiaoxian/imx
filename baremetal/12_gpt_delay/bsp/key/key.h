#ifndef _KEY_H
#define _KEY_H

#include "nxp.h"

// 按键值定义
typedef enum key_val {
    KEY_NONE,
    KEY0_VAL,
    KEY1_VAL,
    KEY2_VAL,
} key_val;

// 按键动作
typedef enum key_act {
    KEY_DOWN,
    KEY_UP,
} key_act;

// 函数声明
void key_init(void);
uint8_t key_get_val(void);

#endif /* _KEY_H */
