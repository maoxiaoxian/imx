#ifndef _REG_DEF_H
#define _REG_DEF_H

// CCM_CCGR 寄存器的复位值
#define CCGR_RST_VAL 0xFFFFFFFF

typedef volatile unsigned int uint32_t;

// CCM_CCGR 寄存器定义
#define CCM_CCGR0 *((uint32_t *)0x020C4068)
#define CCM_CCGR1 *((uint32_t *)0x020C406C)
#define CCM_CCGR2 *((uint32_t *)0x020C4070)
#define CCM_CCGR3 *((uint32_t *)0x020C4074)
#define CCM_CCGR4 *((uint32_t *)0x020C4078)
#define CCM_CCGR5 *((uint32_t *)0x020C407C)
#define CCM_CCGR6 *((uint32_t *)0x020C4080)

// IOMUXC 复用控制寄存器定义 - MUX_CTL
#define IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 *((uint32_t *)0x020E0068)

// IOMUXC 电气特性寄存器定义 - PAD_CTL
#define IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 *((uint32_t *)0x020E02F4)

// GPIO 控制寄存器定义
#define GPIO1_DR       *((uint32_t *)0x0209C000)
#define GPIO1_GDIR     *((uint32_t *)0x0209C004)
#define GPIO1_PSR      *((uint32_t *)0x0209C008)
#define GPIO1_ICR1     *((uint32_t *)0x0209C00C)
#define GPIO1_ICR2     *((uint32_t *)0x0209C010)
#define GPIO1_IMR      *((uint32_t *)0x0209C014)
#define GPIO1_ISR      *((uint32_t *)0x0209C018)
#define GPIO1_EDGE_SEL *((uint32_t *)0x0209C01C)

#endif /* _REG_DEF_H */
