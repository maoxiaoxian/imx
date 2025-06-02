#include "pwm.h"
#include "int.h"
#include "stdio.h"

bl_dev_t bl_dev;

/*
 * 设置 PWM 的采样值：配置 PWMx_PWMSAR 寄存器
 * 配置的采样值会写入 FIFO 中，
 * PWMx_PWMSAR 寄存器相当于一个比较寄存器，
 * 若控制寄存器 PWMx_PWMCR 的 POUTC[19:18] 设置为 0b00，
 * 表示输出引脚在 PWM 电平翻转时置 1，在比较中断产生时清零
 * 若 PWM 计数器中的值小于采样率时输出高电平，否则输出低电平
 * 因此，PWMx_PWMSAR 寄存器实际控制 PWM 的占空比
 */
void pwm1_set_sample(uint16_t val)
{
    PWM1->PWMSAR = (val & 0xFFFF);
}

/*
 * 设置 PWM 的周期：配置 PWMx_PWMPR 寄存器
 * PWM 周期的计算公式如下：
 *      PWM_FRE = PWM_CLK / (PERIOD + 2)
 * 若 PWM_CLK = 1MHz，要产生 1KHz 的 PWM，则 PERIOD = 1000000/1K - 2 = 998
 */
void pwm1_set_period(uint16_t val)
{
    uint16_t reg_val = 0;

    if (val < 2) {
        reg_val = 2;
    } else {
        reg_val = val - 2;
    }

    PWM1->PWMPR = (reg_val & 0xFFFF);
}

/*
 * 设置 PWM 的占空比
 * 占空比 0~100，对应 0%~100%
 */
void pwm1_set_duty(uint8_t duty)
{
    uint16_t period = 0;
    uint16_t sample = 0;

    bl_dev.duty = duty;
    period = PWM1->PWMPR + 2;
    sample = period * bl_dev.duty / 100;

    pwm1_set_sample(sample);
}

/* PWM1 使能：寄存器 PWMx_PWMCR 的 EN[0] 置 1 */
void pwm1_enable(void)
{
    PWM1->PWMCR |= (1 << 0);
}

/* PWM 中断处理函数 */
void pwm1_irq_handler(void)
{
    /* 检查状态寄存器 PWMx_PWMSR 的 FE[3]，判断是否产生 FIFO Empty 中断 */
    if (PWM1->PWMSR & (1 << 3)) {
        /* 将占空比写入 FIFO (设置占空比) */
        pwm1_set_duty(bl_dev.duty);

        PWM1->PWMSR |= (1 << 3); /* 写 1 清除该中断标志 */
    }
}

/* 背光初始化 */
void backlight_init(void)
{
    uint8_t i = 0;

    /* PWM IO 初始化 */
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_PWM1_OUT, 0); /* GPIO1_IO08 复用为 PWM1_OUT */

    /*
     * 配置引脚的电气特性
     * 配置 IOMUXC_SW_PAD_CTL_PAD_xxx 寄存器，相关位域配置如下：
     *      HYS[16]:    0b0,   禁用迟滞比较器
     *      PUS[15:14]: 0b10,  上拉/下拉电阻值，设置为 100K 上拉
     *      PUE[13]:    0b1,   引脚状态保持，设置为保持引脚电平值
     *      PKE[12]:    0b1,   使能状态保持器
     *      ODE[11]:    0b0,   禁用开漏输出
     *      SPEED[7:6]: 0b10,  IO 速率设置为 100MHz
     *      DSE[5:3]:   0b010, IO 驱动能力设置为 R0/2
     *      SRE[0]:     0b0,   低压摆率
     */
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_PWM1_OUT, 0xB090);

    /*
     * 初始化 PWM1
     * 配置寄存器 PWMCR：
     * FWM[27:26]:    0b01, 当 FIFO 中空余位置大于等于 2 时 FIFO 空标志置 1
     * STOPEN[25]:    0b0,  停止模式下 PWM 不工作
     * DOZEN[24]:     0b0,  休眠模式下 PWM 不工作
     * WAITEN[23]:    0b0,  等待模式下 PWM 不工作
     * DBGEN[22]:     0b0,  调试模式下 PWM 不工作
     * BCTR[21]:      0b0,  关闭字节交换
     * HCTR[20]:      0b0,  关闭半字数据交换
     * POUTC[19:18]:  0b00, PWM 输出引脚在计数器重新计数时输出高电平，在计数值达到比较值后输出低电平
     * CLKSRC[17:16]: 0b01, PWM 时钟源选择 IPG_CLK = 66MHz
     * PRESCALER[15:4]: 65, 分频系数为 65 + 1 = 66，PWM 时钟源 = 66MHZ/66 = 1MHz
     * SWR[3]:      0b0,  PWM 不复位
     * REPEAT[2:1]: 0b00, FIFO 中的 Sample 数据只能使用一次
     * EN[0]:       0b0,  先关闭 PWM，后面再使能
     */
    PWM1->PWMCR = 0; /* 清空寄存器 */
    PWM1->PWMCR |= (1 << 26) | (1 << 16) | (65 << 4);

    /* 设置 PWM 周期为 1000，则 PWM 频率为 1M/1000 = 1KHz */
    pwm1_set_period(1000);

    /* 设置占空比，默认 50% 占空比，写 4 次是因为有 4 个 FIFO */
    bl_dev.duty = 50;
    for (i = 0; i < 4; i++) {
        pwm1_set_duty(bl_dev.duty);
    }

    /* 使能 FIFO 空中断，设置寄存器 PWMIR 的 FIE[0] 为 1，使能 FIFO Empty 中断 */
    PWM1->PWMIR |= 1 << 0;

    /* 注册中断服务函数 */
    irq_handler_register(PWM1_IRQn, (irq_handler)pwm1_irq_handler, NULL);
    /* 使能 GIC 中对应的中断 */
    GIC_EnableIRQ(PWM1_IRQn);

    /* PWM 中断状态寄存器清零 */
    PWM1->PWMSR = 0;

    pwm1_enable(); /* 使能 PWM1 */
}
