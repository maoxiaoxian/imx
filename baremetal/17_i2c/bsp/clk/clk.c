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

// 初始化系统时钟
// CPU 主频设置为 792MHz
// PLL2、PLL3 及各自的 PFD 设置为官方推荐值
void clock_init(void)
{
    uint32_t reg_val = 0;

    // ARM 内核时钟设置为 792 MHz
    // 判断当前 ARM 内核的时钟源，正常情况为 pll1_sw_clk
    // pll1_sw_clk 的时钟源有两个：pll1_main_clk 和 step_clk
    // 如果要设置 ARM 的时钟频率为 792MHz 则必须选择 pll1_main_clk 作为时钟源
    // 修改 ARM 内核时钟前，需要先将 PLL1 的时钟源切换为 step_clk（24MHz 晶振）
    // 然后修改 pll1_main_clk，修改完成后再将 PLL1 的时钟源切换回 pll1_main_clk
    if (((CCM->CCSR >> 2) & 0x1) == 0) { // 检查 PLL1_SW_CLK_SEL[2] 位，判断当前 PLL1 的时钟源为 pll1_main_clk（0）还是 step_clk（1）
        // 如果 PLL1 的时钟源为 pll1_main_clk（PLL1_SW_CLK_SEL 位为 0）
        CCM->CCSR &= ~(1 << 8); // 将 step_clk 的时钟源设置为 24MHz 晶振（STEP_SEL[8] 置 0）
        CCM->CCSR |= (1 << 2); // 将 PLL1 的时钟源设置为 step_clk（PLL1_SW_CLK_SEL[2] 置 0）
    }

    // 修改 pll1_main_clk
    // pll1_sw_clk 为 ARM 内核提供时钟前会进行 2 分频
    // ARM 内核时钟计算公式：Fout = Fin * div_select / 2.0，792 = 24 * div_select / 2.0
    // 如果要将 ARM 内核时钟频率设置为 792MHz，则 div_select = 66
    // 修改 CCM_ANALOG_PLL_ARM 寄存器，修改修改的字段即其含义如下：
    // ENABLE[13]：时钟使能位，设置为 1，使能时钟输出
    // DIV_SELECT[6:0]: 根据公式，将其设置为 66（0x42）
    CCM_ANALOG->PLL_ARM = (1 << 13) | ((0x42 << 0) & 0x7F); // 修改 CCM_ANALOG_PLL_ARM 寄存器
    CCM->CCSR &= ~(1 << 2); // PLL1 时钟源切换为 pll1_main_clk
    CCM->CACRR = 0x00000000; // 分频倍率 ARM_PODF[2:0] 设置为 0，即不进行分频

    // 设置 PLL2 SYS_PLL 及其 4 路 PFD
    reg_val = CCM_ANALOG->PFD_528; // 读取 CCM_ANALOG_PFD_528 寄存器中的值
    reg_val &= ~(0x3F3F3F3F); // 清空寄存器
    reg_val |= 32 << 24; // PLL2_PFD3 = 528 * 18 / 32 = 297Mhz
    reg_val |= 24 << 16; // PLL2_PFD2 = 528 * 18 / 24 = 396Mhz （DDR 使用的时钟，最大 400Mhz）
    reg_val |= 16 << 8; // PLL2_PFD1 = 528 * 18 / 16 = 594Mhz
    reg_val |= 27 << 0; // PLL2_PFD0 = 528 * 18 / 27 = 352Mhz
    CCM_ANALOG->PFD_528 = reg_val; // 将值写回寄存器（设置 PLL2_PFD0 ~ PLL2_PFD3）

    // 设置 PLL3 USB1PLL 及其 4 路 PFD
    reg_val = 0; // 清空临时变量
    reg_val = CCM_ANALOG->PFD_480; // 读取 CCM_ANALOG_PFD_480 寄存器中的值
    reg_val &= ~(0x3F3F3F3F); // 清空寄存器
    reg_val |= 19 << 24; // PLL3_PFD3 = 480 * 18 / 19 = 454.74Mhz
    reg_val |= 17 << 16; // PLL3_PFD2 = 480 * 18 / 17 = 508.24Mhz
    reg_val |= 16 << 8; // PLL3_PFD1 = 480 * 18 / 16 = 540Mhz
    reg_val |= 12 << 0; // PLL3_PFD0 = 480 * 18 / 12 = 720Mhz
    CCM_ANALOG->PFD_480 = reg_val; // 将值写回寄存器（设置 PLL3_PFD0 ~ PLL3_PFD3）

    // 设置 AHB 时钟，最小 6MHz，最大 132MHz (boot rom 中已设置该时钟，可以不用设置)
    // CCM_CBCMR 寄存器用设置 AHB 总线外设的时钟源及分频倍率，相关字段的含义如下：
    // PRE_PERIPH_CLK_SEL[19:18]：外设时钟源，这里设置为 0b01，选择 PLL2 PFD2 = 396MHz
    CCM->CBCMR &= ~(3 << 18); // 清空 PRE_PERIPH_CLK_SEL
    CCM->CBCMR |= (1 << 18); // pre_periph_clk = PLL2_PFD2 = 396MHz

    // CCM_CBCDR 寄存器设置外设时钟源，其相关位及含义如下：
    // PERIPH_CLK_SEL[25]：外设时钟源，置 1 选择 periph_clk2_clk
    CCM->CBCDR &= ~(1 << 25);
    while (CCM->CDHIPR & (1 << 5)) {} // 修改 PERIPH_CLK_SEL[25] 需要等待握手完成

    // 修改 AHB_PODF 位时，需要先禁止 AHB_CLK_ROOT 输出
    // 但是没有找到关闭 AHB_CLK_ROOT 输出的的寄存器，所以无法设置
    // 下面设置 AHB_PODF 的代码仅供学习参考，不能直接拿来使用！！
    // 内部 boot rom 将 AHB_PODF 设置为 3 分频，即使不设置 AHB_PODF，AHB_ROOT_CLK 也依旧等于 396 / 3 = 132Mhz
#if 0
    // 需要先关闭 AHB_ROOT_CLK 输出，否则时钟设置会出错
    CCM->CBCDR &= ~(7 << 10); // CBCDR 的 AHB_PODF 清零
    CCM->CBCDR |= 2 << 10; // AHB_PODF 设置为 3 分频：AHB_CLK_ROOT = 132MHz
    while(CCM->CDHIPR & (1 << 1)) {} // 等待握手完成
#endif

    // 配置 IPG_CLK_ROOT，最小 3MHz，最大 66 MHz（boot room 已经配置，可以不用再配置）
    CCM->CBCDR &= ~(3 << 8); // 清空 CBCDR.IPG_PODF
    CCM->CBCDR |= 1 << 8; // IPG_PODF 设置为 2 分频，IPG_CLK_ROOT = 66MHz

    // 配置 PERCLK_CLK_ROOT
    CCM->CSCMR1 &= ~(1 << 6); // PERCLK_CLK_ROOT 时钟源为 IPG
    CCM->CSCMR1 &= ~(7 << 0); // PERCLK_PODF 清零，即 1 分频
}
