#include "delay.h"

// CPU 执行空操作，实现短时间延时
// 注意：这里的变量必须使用 volatile 修饰，不然会被编译器优化掉！
static void delay_short(volatile uint32_t tick)
{
    while (tick--) {}
}

// 延时函数，CPU 主频为 396MHz 时，执行一次耗时约 1ms
// 注意：这里的变量必须使用 volatile 修饰，不然会被编译器优化掉！
void delay_ms(volatile uint32_t ms)
{
    while (ms--)
    {
        delay_short(0x7FF);
    }
}
