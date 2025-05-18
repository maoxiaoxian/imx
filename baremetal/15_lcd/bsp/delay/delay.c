#include "delay.h"
#include "int.h"
#include "led.h"

// 初始化 GPT 定时器，使用 GPT1 模块，时钟源选择 ipg_clk = 66MHz
void delay_init(void)
{
    // 清空配置寄存器 CR，禁用 GPT
    GPT1->CR = 0;

    // 复位 GPT
    // SWR[15]：软件复位，向该位写 1 触发 GPT 模块复位
    // 复位完成后该位自动清 0
    GPT1->CR = (1 << 15);
    while (GPT1->CR & 0x00008000) {} // 等待复位完成

    // GPT 配置
    // OM1[22:20]：输出比较运行模式，0b000，关闭通道 1 的输出功能，即对应的引脚不响应
    // FRR[9]：GPT 工作模式，0b0，设置为 Restart 模式，当 CNT == OCR1 时产生中断
    // CLKSRC[8:6]：GPT 时钟源选择，0b001，选择 ipg_clk = 66MHz
    // ENMOD[1]：该位置 1 后，当 GPT 关闭后，计数值清 0，该位置 0 时，GPT 在关闭时也会保存其中的值
    GPT1->CR |= (1 << 1) | (1 << 6);

    // GPT 分频值设置（PR 寄存器）
    // PRESCALER[11:0]：非晶振时钟源的分频值，0 表示 1 分频（最大 0xFFF 对应 4096 分频）
    GPT1->PR = 0x41; // 设置为 65，即 66 分频，则 GPT1 的时钟频率为 66MHz / 66 = 1MHz

    // 设置 OCR1 比较寄存器，GPT 输出比较 1 的计数值
    // GPT 时钟频率为 1MHz，每个 Tick 为 1us
    // 将其设置为 0xFFFF,FFFF，一次计满为：0xFFFFFFFFus = 4294967296us = 4295s = 71.5min
    // 注意计数值会溢出
    GPT1->OCR[0] = 0xFFFFFFFF;

    // 使能 GPT1
    GPT1->CR |= (1 << 0);

    // 以下屏蔽的代码为 GPT 定时器中断功能
    // 如果要学习 GPT 中断可以参考该代码
#if 0
    // 配置 PR 寄存器中的分频值
    // PRESCALER[11:0]：非晶振时钟源的分频值，0 表示 1 分频（最大 0xFFF 对应 4096 分频）
    GPT1->PR = 0x41; // 设置为 65，即 66 分频，则 GPT1 的时钟频率为 66MHz / 66 = 1MHz

    // 配置输出比较寄存器 OCR1，当计数值与 OCR1 寄存器中的值相等时产生比较事件
    // 这里设置为 500ms 产生一次中断，因此将其设置为 1000000 / 2 = 500000d
    GPT1->OCR[0] = 500000;

    // GPT 中断使能寄存器 IR 设置
    // OF1IE[0]：使能输出通道 1 的输出比较中断
    GPT1->IR |= (1 << 0);

    // GIC 使能对应的中断
    GIC_EnableIRQ(GPT1_IRQn);

    // 注册中断服务函数
    irq_handler_register(GPT1_IRQn, (irq_handler)gpt1_irq_handler, ((void *)0));

    // 使能 GPT1
    GPT1->CR |= (1 << 0);
#endif
}

#if 0
// GPT1 中断处理函数
void gpt1_irq_handler(uint32_t gicc_iar, void *opaque)
{
    static uint8_t led_state = ACT_OFF;

    led_state = !led_state;

    // 检查 GPT 产生中断（SR 寄存器）
    // OF1[0]：输出通道 1 是否产生中断
    if (GPT1->SR & (1 << 0)) { // 产生中断表示定时器超时
        led_switch(LED0, led_state);
    }

    // 清除中断标志位
    GPT1->SR |= (1 << 0);
}
#endif

// 微秒级延时
void delay_us(uint32_t us)
{
    uint64_t cnt_old = 0;
    uint64_t cnt_new = 0;
    uint64_t time = 0;

    // 读取当前计数值（时间）
    cnt_old = GPT1->CNT;

    while (1)
    {
        // 再次读取计数值
        cnt_new = GPT1->CNT;

        // 检查计数器是否正常工作
        if (cnt_new != cnt_old) {
            // GPT 为向上计数定时器，检查是否产生溢出
            if (cnt_new > cnt_old) {
                time += cnt_new - cnt_old; // 没有溢出
            } else { // 产生溢出
                time += 0xFFFFFFFF - cnt_old + cnt_new;
            }

            // 更新计数值
            cnt_old = cnt_new;

            // 如果时间到达设定值则跳出
            if (time >= us) {
                break;
            }
        }
    }
}

// 毫秒级延时
void delay_ms(uint32_t ms)
{
    int i = 0;

    for (i = 0; i < ms; i++) {
        delay_us(1000);
    }
}

// CPU 执行空操作，实现短时间延时
// 注意：这里的变量必须使用 volatile 修饰，不然会被编译器优化掉！
static void delay_short(volatile uint32_t tick)
{
    while (tick--) {}
}

// 延时函数，CPU 主频为 396MHz 时，执行一次耗时约 1ms
// 注意：这里的变量必须使用 volatile 修饰，不然会被编译器优化掉！
void delay(volatile uint32_t ms)
{
    while (ms--)
    {
        delay_short(0x7FF);
    }
}
