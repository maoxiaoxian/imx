#include "spi.h"
#include "gpio.h"
#include "stdio.h"

/* 初始化 ECSPI 模块 */
void spi_init(ECSPI_Type *base)
{
    /*
     * 配置 ECSPI
     * 控制寄存器 ECSPIx_CONREG 的相关位域：
     * BURST_LENGTH[31:20]:   0x007,  单次突发传输的长度为 8-bit
     * CHANNEL_SELECT[19:18]: 0b00,   选择通道 0
     * CHANNEL_MODE[7:4]:     0b0001, 通道 0 设置为主机模式
     * SMC[3]: 0b1, 向 TXFIFO 写入数据后立即启动 SPI 突发传输
     * EN[0]:  0b1, 使能 ECSPI
     */
    base->CONREG = 0; // 清空 ECSPIx_CONREG 寄存器
    base->CONREG |= (1 << 0) | (1 << 3) | (1 << 4) | (7 << 20); // 配置 ECSPIx_CONREG 寄存器

    /*
     * 配置 ECSPI 的通道 0
     * 配置寄存器 ECSPIx_CONFIGREG 的相关位域：
     * SCLK_CTL[23:20]: 0b0000, 通道 0 空闲时 SCLK 信号为低电平
     * DATA_CTL[19:16]: 0b0000, 通道 0 空闲时数据线信号为高电平
     * SS_POL[15:12]:   0b0000, 通道 0 的从机选择信号 SS 极性为低电平
     * SS_CTL[11:8]:    0b0000, 通道 0 的片选信号，SMC 为 1 时该位无效
     * SCLK_POL[7:4]:   0b0000, 通道 0 的 SCLK 信号极性 CPOL，设置为高电平有效
     * SCLK_PHA[0]:     0b0000, 通道 0 的时钟相位 CPHA，设置为 0
     */
    base->CONFIGREG = 0; // 配置 SPI 通道 0

    /*
     * 配置 ECSPI 通道 0 的采样周期
     * 采样周期控制寄存器 ECSPIx_PERIODREG 的相关位域：
     * CSD_CTL[21:16]:      0x0,    片选信号 CS/SS 的延迟时长，可配置范围 0~63
     * CSRC[15]:            0b1,    时钟源选择 SPI Clock
     * SAMPLE_PERIOD[14:0]: 0x2000, 采样等待周期，例如，当 SPI 的时钟频率为 10MHz 时，
     *                              0x2000 等于 1 / 10000 * 0x2000 = 0.8192ms，
     *                              即连续读取数据时，两次读取之间的间隔为 0.8ms
     */
    base->PERIODREG = 0x2000; // 采样周期

    /*
     * 配置 ECSPI 的时钟频率
     * 
     * ECSPI 的时钟源为 pll3_sw_clk / 8 = 480 / 8 = 60MHz
     * 
     * 配置 ECSPIx_CONREG 寄存器：
     * PRE_DIVIDER[15:12]: 前置分频系数
     * POST_DIVIDER[11:8]: 后置分频系数
     * 
     * ECSPI 时钟频率的计算公式如下：
     * SPI CLK = (SourceCLK / PER_DIVIDER) / (2^POST_DIVEDER)
     * 
     * 例如，需要将 ECSPI 的时钟频率设置为 6MHz，则有：
     * PRE_DIVIDER = 0x9
     * POST_DIVIDER = 0x0
     * SPI CLK = 60000000 / (0x9 + 1) = 60000000 = 6MHz
     */
    base->CONREG &= ~((0xF << 12) | (0xF << 8)); // 清除 PRE_DIVIDER 和 POST_DIVIDER
    base->CONREG |= (0x9 << 12); // PRE_DIVIDER 设置为 9，POST_DIVIDER 设置为 0
}

/* 通过 SPI 读取/发送一个字节的数据 */
uint8_t spi_ch0_rw_byte(ECSPI_Type *base, uint8_t data)
{
    uint32_t rx_data = 0;
    uint32_t tx_data = data;

    /* 选择 ECSPI 的通道 0 */
    base->CONREG &= ~(0b11 << 18); // 清除 CHANNEL_SELECT[19:18] 通道选择位域
    base->CONREG |= (0b00 << 18); // 选择 SPI 通道 0，即 SPI0

    while (!(base->STATREG & (1 << 0))) {} // 等待 TxFIFO 为空
    base->TXDATA = tx_data; // 将数据写入 ECSPIx_TXDATA 寄存器 (发送数据)

    while (!(base->STATREG & (1 << 3))) {} // 等待 RxFIFO 中存在数据 (接收到的数据存入 ECSPIx_RXDATA 寄存器)
    rx_data = base->RXDATA; // 获取输入的数据

    return rx_data;
}
