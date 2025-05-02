#include "clk.h"

// 使能所有时钟
void clock_enable(void)
{
    CCM->CCGR0 = CCGR_RST_VAL;
    CCM->CCGR1 = CCGR_RST_VAL;
    CCM->CCGR2 = CCGR_RST_VAL;
    CCM->CCGR3 = CCGR_RST_VAL;
    CCM->CCGR4 = CCGR_RST_VAL;
    CCM->CCGR5 = CCGR_RST_VAL;
    CCM->CCGR6 = CCGR_RST_VAL;
}
