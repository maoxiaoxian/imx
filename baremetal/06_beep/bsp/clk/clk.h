#ifndef _CLK_H
#define _CLK_H

#include "nxp.h"

// CCM_CCGR 寄存器复位值
#define CCGR_RST_VAL 0xFFFFFFFF

// 函数声明
void clock_enable(void);

#endif /* _CLK_H */
