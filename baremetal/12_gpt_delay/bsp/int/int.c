#include "int.h"

// 中断嵌套计数器
static uint32_t irq_count;

// 中断向量表
static irq_table_t irq_table[NUMBER_OF_INT_VECTORS];

// 中断初始化
void int_init(void)
{
    GIC_Init(); // 初始化 GIC
    irq_table_init(); // 初始化中断向量表
    __set_VBAR((uint32_t)0x87800000); // 设置中断向量表偏移，移动到中断向量表起始位置
}

// 初始化中断向量表
void irq_table_init(void)
{
    irq_count = 0;
    int i = 0;

    for (i = 0; i < NUMBER_OF_INT_VECTORS; i++) {
        irq_handler_register((IRQn_Type)i, default_irq_handler, ((void *)0));
    }
}

// 给指定的中断号注册中断服务函数
void irq_handler_register(IRQn_Type irq, irq_handler handler, void *opaque)
{
    irq_table[irq].irq_handler = handler;
    irq_table[irq].opaque = opaque;
}

// 根据传入的中断号调用对应的中断服务函数
void sys_irq_handler(uint32_t gicc_iar)
{
    uint32_t irq_id = gicc_iar & 0x3FF;

    // 检查中断号是否符合要求
    if (irq_id >= NUMBER_OF_INT_VECTORS) {
        return;
    }

    // 中断嵌套计数器加一
    irq_count++;

    // 根据传入的中断号，在 irq_table 中调用对应的中断服务函数
    irq_table[irq_id].irq_handler(irq_id, irq_table[irq_id].opaque);

    // 中断执行完成，中断嵌套计数器减一
    irq_count--;
}

// 默认中断服务函数
void default_irq_handler(uint32_t gicc_iar, void *opaque)
{
    while (1) {}
}
