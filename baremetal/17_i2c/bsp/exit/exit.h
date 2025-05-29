#ifndef _EXIT_H
#define _EXIT_H

#include "nxp.h"

// 函数声明
void exit_init(void); // 中断初始化
void gpio1_io18_irq_handler(void); // GPIO1_IO18 中断处理

#endif /* _EXIT_H */
