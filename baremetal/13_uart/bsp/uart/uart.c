#include "uart.h"

/* 初始化串口 UART1 使用的 IO 引脚 */
void uart_io_init(void)
{
    /* 初始化 IO 复用
     * UART1_RX_DATA 引脚功能选择为 UART1_RXD（配置 IOMUXC_SW_MUX_CTL_PAD_UART1_RX_DATA 寄存器）
     * UART1_TX_DATA 引脚功能选择为 UART1_TXD（配置 IOMUXC_SW_MUX_CTL_PAD_UART1_TX_DATA 寄存器）
     */
    IOMUXC_SetPinMux(IOMUXC_UART1_TX_DATA_UART1_TX, 0);
    IOMUXC_SetPinMux(IOMUXC_UART1_RX_DATA_UART1_RX, 0);

    /* 配置 UART1_RXD 和 UART1_TXD 引脚的电气特性
     * 配置 IOMUXC_SW_PAD_CTL_PAD_UART1_TX_DATA 寄存器
     * 配置 IOMUXC_SW_PAD_CTL_PAD_UART1_RX_DATA 寄存器
     * 这两个引脚的电气特性一样，相关位域的配置如下：
     * HYS[16]：使能迟滞比较器，0b0，禁用
     * PUS[15:14]：上拉/下拉电阻值，0b00，下拉
     * PUE[13]：引脚状态保持，0b0，不保持引脚电平值
     * PKE[12]：状态保持器使能，0b1，使能
     * ODE[11]：开漏输出使能，0b0，禁用
     * SPEED[7:6]：IO 速率，0b10，100MHz
     * DSE[5:3]：IO 驱动能力，0b110，R0/6
     * SRE[0]：压摆率，0b0，低压摆率
     * 因此，最终要写入寄存器的值为：0x000010B0
     */
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_TX, 0x000010B0);
    IOMUXC_SetPinConfig(IOMUXC_UART1_TX_DATA_UART1_RX, 0x000010B0);
}

/* 启用指定的 UART 模块 */
void uart_enable(UART_Type *base)
{
    /* 向 UART_UCR1 寄存器的 UARTEN[0] 位写 1 使能 UART 模块 */
    base->UCR1 |= (0b1 << 0);
}

/* 禁用指定的 UART 模块 */
void uart_disable(UART_Type *base)
{
    /* 向 UART_UCR1 寄存器的 UARTEN[0] 位写 0 禁用 UART 模块 */
    base->UCR1 &= ~(0b1 << 0);
}

/* UART 复位 */
void uart_soft_reset(UART_Type *base)
{
    /* 向 UART_UCR2 寄存器的 SRST[0] 位写 0 发送 UART 模块复位请求
     * 复位完成后，SRST[0] 会自动恢复为 1
     */
    base->UCR2 &= ~(0b1 << 0);

    /* 等待复位完成 */
    while (!(base->UCR2 & 0x00000001)) {}
}

/* 设置 UART 的波特率 */
void uart_set_baudrate(UART_Type *base, uint32_t baudrate, uint32_t clk)
{
    uint32_t numerator = 0; /* 分子 */
    uint32_t denominator = 0; /* 分母 */
    uint32_t divisor = 0;
    uint32_t refFreqDiv = 0;
    uint32_t divider = 0;
    uint64_t baudDiff = 0;
    uint64_t tempNumerator = 0;
    uint64_t tempDenominator = 0;

    /* 计算最大的除数 */
    numerator = clk;
    denominator = baudrate << 4;
    divisor = 1;

    while (denominator != 0)
    {
        divisor = denominator;
        denominator = numerator % denominator;
        numerator = divisor;
    }

    numerator = clk / divisor;
    denominator = (baudrate << 4) / divisor;

    /* numerator ranges from 1 ~ 7 * 64k */
    /* denominator ranges from 1 ~ 64k */
    if ((numerator > (UART_UBIR_INC_MASK * 7)) || (denominator > UART_UBIR_INC_MASK))
    {
        uint32_t m = (numerator - 1) / (UART_UBIR_INC_MASK * 7) + 1;
        uint32_t n = (denominator - 1) / UART_UBIR_INC_MASK + 1;
        uint32_t max = m > n ? m : n;
        numerator /= max;
        denominator /= max;
        if (0 == numerator)
        {
            numerator = 1;
        }
        if (0 == denominator)
        {
            denominator = 1;
        }
    }
    divider = (numerator - 1) / UART_UBIR_INC_MASK + 1;

    switch (divider)
    {
        case 1:
            refFreqDiv = 0x05;
            break;
        case 2:
            refFreqDiv = 0x04;
            break;
        case 3:
            refFreqDiv = 0x03;
            break;
        case 4:
            refFreqDiv = 0x02;
            break;
        case 5:
            refFreqDiv = 0x01;
            break;
        case 6:
            refFreqDiv = 0x00;
            break;
        case 7:
            refFreqDiv = 0x06;
            break;
        default:
            refFreqDiv = 0x05;
            break;
    }
    /* Compare the difference between baudRate_Bps and calculated baud rate.
     * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
     * baudDiff = (srcClock_Hz/divider)/( 16 * ((numerator / divider)/ denominator).
     */
    tempNumerator = clk;
    tempDenominator = (numerator << 4);
    divisor = 1;
    /* get the approximately maximum divisor */
    while (tempDenominator != 0)
    {
        divisor = tempDenominator;
        tempDenominator = tempNumerator % tempDenominator;
        tempNumerator = divisor;
    }
    tempNumerator = clk / divisor;
    tempDenominator = (numerator << 4) / divisor;
    baudDiff = (tempNumerator * denominator) / tempDenominator;
    baudDiff = (baudDiff >= baudrate) ? (baudDiff - baudrate) : (baudrate - baudDiff);

    if (baudDiff < (baudrate / 100) * 3)
    {
        base->UFCR &= ~UART_UFCR_RFDIV_MASK;
        base->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
        base->UBIR = UART_UBIR_INC(denominator - 1); // 要先写 UBIR 寄存器，然后再写 UBMR 寄存器
        base->UBMR = UART_UBMR_MOD(numerator / divider - 1);
    }
}

/* UART1 初始化，波特率 115200 */
void uart_init(void)
{
    /* 初始化串口 IO 引脚 */
    uart_io_init();

    /* 初始化 UART1 */
    uart_disable(UART1); /* 先禁用 UART1 */
    uart_soft_reset(UART1); /* 复位 UART1 */

    /* 清除 UCR1 寄存器 */
    UART1->UCR1 = 0;

    /* 关闭自动波特率检测：UART1_UCR1.ADBR[14] 置 0 */
    UART1->UCR1 &= (0b1 << 14);

    /* 配置 UART：UART_UCR2 寄存器
     * IRTS[14]：1，忽略 RTS 引脚
     * PREN[8]： 0，关闭奇偶校验校验
     * STPB[6]： 0，1 位停止位
     * WS[5]：   1，8-bit 数据位
     * TXEN[2]： 1，使能发送
     * RXEN[1]： 1，使能接收
     */
    UART1->UCR2 |= (0b1 << 14) | (0b1 << 5) | (0b1 << 2) | (0b1 << 1);

    /* 配置 UART1_UCR3 寄存器，将 RXDMUXSEL[2] 设置为 1（该位必须始终为 1） */
    UART1->UCR3 |= (0b1 << 2);

    /* 设置波特率
     * 波特率计算公式: BaudRate = RefFreq / (16 * (UBMR + 1) / (UBIR+1))
     * 如果要设置波特率为 115200，则使用如下参数:
     * RefFreq = 80MHz，寄存器 UART_UFCR 的 RFDIV[9:7] = 0b101， 表示 1 分频
     * UBMR = 3124
     * UBIR = 71
     * 因此，波特率 = 80000000 / (16 * (3124 + 1) / (71 + 1))
     *            = 80000000 / (16 * 3125 / 72)
     *            = (80000000 * 72) / (16 * 3125)
     *            = 115200
     */
    UART1->UFCR = (0b101 << 7); /* refFreq = ipg_clk / 1 = 80MHz */
    UART1->UBIR = 71;
    UART1->UBMR = 3124;

    /* 也可以通过以下函数计算并设置波特率 */
#if 0
    uart_set_baudrate(UART1, 115200, 80000000); /* 设置波特率 */
#endif

    /* 串口使能 */
    uart_enable(UART1);
}

/* 通过串口发送一个字符 */
void put_char(char c)
{
    /* 等待上一次发送完成 */
    while (!(UART1->USR2 & 0x00000008)) {}

    /* 发送字符：将字符值（ASCII 码）写入 UTXD 寄存器 */
    UART1->UTXD = (c & 0xFF);
}

/* 发送字符串 */
void put_string(char *str)
{
    char *p = str;

    while (*p)
    {
        put_char(*p);

        p++;
    }
}

/* 接收一个字符 */
char get_char(void)
{
    /* 等待接收完成 */
    while (!(UART1->USR2 & 0x00000001)) {}

    /* 返回接收到的字符 */
    return UART1->URXD;
}

/* 该函数仅用于防止编译器报错 */
void raise(int sig_nr) 
{

}
