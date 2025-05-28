#include "lcd.h"
#include "gpio.h"
#include "delay.h"
#include "stdio.h"

/* 屏幕参数结构体 */
tft_lcd_t tft_lcd;

/*
 * 初始化 SGM3157 模拟开关使用的控制引脚，以及 Panel ID 相关的三个输入引脚
 * Panel 的 R7、G7、B7 引脚连接了上拉/下拉电阻，用于识别屏幕 ID (屏幕 ID 由硬件厂商定义)
 * Panel 引脚与芯片 GPIO 的连接如下所示 (括号中的 M0/M1/M2 为模拟开关)：
 *      LCD_DATA23 <--> R7 (M0)
 *      LCD_DATA15 <--> G7 (M1)
 *      LCD_DATA07 <--> B7 (M2)
 * 三个模拟开关的值与 Panel ID 的对应关系如下：
 *      M0: M1: M2 --> Panel ID
 *      0 : 0 : 0  --> 4.3 Inch,  480*272,  ID = 0x4342
 *      0 : 0 : 1  --> 7 Inch,    800*480,  ID = 0x7084
 *      0 : 1 : 0  --> 7 Inch,    1024*600, ID = 0x7016
 *      1 : 0 : 1  --> 10.1 Inch, 1280*800, ID = 0x1018
 *      1 : 0 : 0  --> 4.3 Inch,  800*480,  ID = 0x4384
 */
void lcd_panel_id_gpio_init(void)
{
    /* 配置模拟开关的控制引脚：将 LCD_VSYNC 引脚配置为输出引脚 */
    /*
     * GPIO3_IO03 引脚功能选择为 GPIO 输出
     * 配置寄存器 IOMUXC_SW_MUX_CTL_PAD_LCD_VSYNC (地址：0x020E0110)
     * MUX_MODE[3:0]: 设置为 0b0101 (0x5) 引脚功能选择为 GPIO3_IO03
     * SION[4]: 即 IOMUXC_SetPinMux() 的最后一个参数，
     *          设置为 1 会强制引脚功能为对应的输入引脚，这里设置为 0，
     *          即引脚功能由 MUX_MODE[3:0] 决定
     */
    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0);

    /*
     * 设置 GPIO3_IO03 引脚的电气特性
     * 配置寄存器 IOMUXC_SW_PAD_CTL_PAD_LCD_VSYNC (地址：0x020E039C)
     * 该寄存器中各位域的含义，以及设置值的含义如下：
     * HYS[16]:    使能迟滞比较器， 0b0，    禁用
     * PUS[15:14]: 上拉/下拉电阻值，0b00，   100K 下拉
     * PUE[13]:    使用上拉/下拉(脉冲输出/输入) or 状态保持器(维持输出电平)，0b0，维持输出电平
     * PKE[12]:    状态保持器使能, 0b1，   使能
     * ODE[11]:    开漏输出使能，  0b0，   禁用
     * SPEED[7:6]: IO 速率,      0b10，  100MHz
     * DSE[5:3]:   IO 驱动能力，  0b110， R0/6
     * SRE[0]:     压摆率，       0b0，   低压摆率
     * 因此，最终要写入寄存器的值为：0x000010B0
     */
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0x000010B0);

    /* 打开模拟开关 SGM3157 */
    pin_cfg sgm_ctrl;

    sgm_ctrl.dir = GPIO_OUTPUT;     // 引脚功能为输出
    sgm_ctrl.def_val = LEVEL_HIGH;  // 引脚默认输出高电平
    gpio_init(GPIO3, 3, &sgm_ctrl); // 初始化引脚：SGM3157 的控制引脚 LCD_VSYNC 输出高电平，打开模拟开关

    /* 配置屏幕 ID 识别使用的三个引脚：LCD_DATA23、LCD_DATA15、LCD_DATA07 */
    /*
     * 以 LCD_DATA07 为例进行说明，将该引脚配置为 GPIO 输入
     * 设置寄存器 IOMUXC_SW_MUX_CTL_PAD_LCD_DATA07 (地址：0x020E0134)
     * MUX_MODE[3:0]: 设置为 0b0101 (0x5) 引脚功能选择为 GPIO3_IO12
     * SION[4]：设置为 0，引脚功能由 MUX_MODE[3:0] 决定
     */
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_GPIO3_IO12, 0); // B7 (LCD_DATA07) <--> M2
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_GPIO3_IO20, 0); // G7 (LCD_DATA15) <--> M1
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_GPIO3_IO28, 0); // R7 (LCD_DATA23) <--> M0

    /*
     * 设置 GPIO3_IO12 引脚的电气特性
     * 配置寄存器 IOMUXC_SW_PAD_CTL_PAD_LCD_DATA07（地址：0x020E03C0）
     * 该寄存器中各位域的含义，以及设置值的含义如下：
     * HYS[16]:    使能迟滞比较器， 0b0，    禁用
     * PUS[15:14]: 上拉/下拉电阻值，0b11，   22K 上拉
     * PUE[13]:    使用上拉/下拉(脉冲输出/输入) or 状态保持器(维持输出电平)，0b1，脉冲输出/输入
     * PKE[12]:    状态保持器使能, 0b1，   启用
     * ODE[11]:    开漏输出使能，  0b0，   禁用
     * SPEED[7:6]: IO 速率,      0b10，  100MHz
     * DSE[5:3]:   IO 驱动能力，  0b000， 禁用输出功能
     * SRE[0]:     压摆率，       0b0，   低压摆率
     * 因此，最终要写入寄存器的值为：0x0000F080
     */
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_GPIO3_IO12, 0x0000F080);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_GPIO3_IO20, 0x0000F080);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_GPIO3_IO28, 0x0000F080);

    /* 初始化 LCD_DATA23、LCD_DATA15、LCD_DATA07 引脚 */
    pin_cfg id_m2;
    pin_cfg id_m1;
    pin_cfg id_m0;

    id_m2.dir = GPIO_INPUT; // LCD_DATA07 配置为输入引脚
    id_m1.dir = GPIO_INPUT; // LCD_DATA15 配置为输入引脚
    id_m0.dir = GPIO_INPUT; // LCD_DATA23 配置为输入引脚

    gpio_init(GPIO3, 12, &id_m2); // LCD_DATA07 为 GPIO3 组的第 12 个引脚，M2
    gpio_init(GPIO3, 20, &id_m1); // LCD_DATA15 为 GPIO3 组的第 20 个引脚，M1
    gpio_init(GPIO3, 28, &id_m0); // LCD_DATA23 为 GPIO3 组的第 28 个引脚，M0
}

/*
 * 获取 Panel ID（读取 M0、M1、M2 的值，确定 ID）
 * Panel 的 R7、G7、B7 引脚连接了上拉/下拉电阻，用于识别屏幕 ID (屏幕 ID 由硬件厂商定义)
 * Panel 引脚与芯片 GPIO 的连接如下所示 (括号中的 M0/M1/M2 为模拟开关)：
 *      LCD_DATA23 <--> R7 (M0)
 *      LCD_DATA15 <--> G7 (M1)
 *      LCD_DATA07 <--> B7 (M2)
 * 三个模拟开关的值与 Panel ID 的对应关系如下：
 *      M0: M1: M2 --> Panel ID
 *      0 : 0 : 0  --> 4.3 Inch,  480*272,  ID = 0x4342
 *      0 : 0 : 1  --> 7 Inch,    800*480,  ID = 0x7084
 *      0 : 1 : 0  --> 7 Inch,    1024*600, ID = 0x7016
 *      1 : 0 : 1  --> 10.1 Inch, 1280*800, ID = 0x1018
 *      1 : 0 : 0  --> 4.3 Inch,  800*480,  ID = 0x4384
 */
uint16_t lcd_read_panel_id(void)
{
    uint8_t panel_id = 0;
    uint16_t panel = 0;
    uint8_t id_m0 = 0;
    uint8_t id_m1 = 0;
    uint8_t id_m2 = 0;

    id_m0 = (uint8_t)pin_read(GPIO3, 28); // 读取 GPIO3_IO28 (LCD_DATA23)，获取 M0 的值
    id_m1 = (uint8_t)pin_read(GPIO3, 20); // 读取 GPIO3_IO20 (LCD_DATA15)，获取 M1 的值
    id_m2 = (uint8_t)pin_read(GPIO3, 12); // 读取 GPIO3_IO12 (LCD_DATA07)，获取 M2 的值

    /* 确认 Panel ID */
    panel_id = id_m0 | (id_m1 << 1) | (id_m2 << 2);
    switch (panel_id)
    {
    case ID_ATK4342:
        panel = ATK4342; // 4.3 英寸，480*272 分辨率
        break;
    case ID_ATK7084:
        panel = ATK7084; // 7 英寸，800*480 分辨率
        break;
    case ID_ATK7016:
        panel = ATK7016; // 7 英寸，1024*600 分辨率
        break;
    case ID_ATK4384:
        panel = ATK4384; // 4.3 英寸，800*480 分辨率
        break;
    case ID_ATK1018:
        panel = ATK1018; // 10.1 英寸，1280*800 分辨率
        break;
    case ID_ATKVGA:
        panel = ATKVGA; // VGA 模块，1366*768 分辨率
        break;
    default:
        panel = ATKUNKNOWN;
        break;
    }

    return panel;
}

/*
 * 初始化 IMX 与屏幕连接的引脚：数据引脚和控制引脚
 * 
 * IMX 的引脚 LCD_DATA00 ~ LCD_DATA07 连接屏幕的 LCD_B0 ~ LCD_B7 引脚，
 * 用于传输 RGB 图像数据的 BLUE 通道，8-bit 长度对应 8 个引脚；
 * 
 * IMX 的引脚 LCD_DATA08 ~ LCD_DATA015 连接屏幕的 LCD_G0 ~ LCD_G7 引脚，
 * 用于传输 RGB 图像数据的 GREEN 通道，8-bit 长度对应 8 个引脚；
 * 
 * IMX 的引脚 LCD_DATA16 ~ LCD_DATA23 连接屏幕的 LCD_R0 ~ LCD_R7 引脚，
 * 用于传输 RGB 图像数据的 RED 通道，8-bit 长度对应 8 个引脚；
 * 
 * 四个控制信号引脚 LCD_CLK, LCD_VSYNC, LCD_HSYNC, LCD_DE 连接关系如下：
 * IMX 的引脚 LCD_CLK 和屏幕的 LCD_PCLK 引脚连接；
 * IMX 的引脚 LCD_VSYNC 和屏幕的 LCD_VSYNC 引脚连接；
 * IMX 的引脚 LCD_HSYNC 和屏幕的 LCD_HSYNC 引脚连接；
 * IMX 的引脚 LCD_ENABLE 和屏幕的 LCD_DE 引脚连接；
 * 
 * 屏幕的 BLT_PWM 引脚用于背光控制，和 IMX 的 GPIO1_IO08 引脚连接
 */
void lcd_gpio_init(void)
{
    /* 设置 IMX 引脚的功能复用 */
    /* IMX 的 LCD_DATA00 ~ LCD_DATA07 引脚为 RGB 图像数据的 BLUE[7:0] 通道 */
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA00_LCDIF_DATA00, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA01_LCDIF_DATA01, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA02_LCDIF_DATA02, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA03_LCDIF_DATA03, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA04_LCDIF_DATA04, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA05_LCDIF_DATA05, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA06_LCDIF_DATA06, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_LCDIF_DATA07, 0);

    /* IMX 的 LCD_DATA08 ~ LCD_DATA15 引脚为 RGB 图像数据的 GREEN[7:0] 通道 */
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA08_LCDIF_DATA08, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA09_LCDIF_DATA09, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA10_LCDIF_DATA10, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA11_LCDIF_DATA11, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA12_LCDIF_DATA12, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA13_LCDIF_DATA13, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA14_LCDIF_DATA14, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_LCDIF_DATA15, 0);

    /* IMX 的 LCD_DATA16 ~ LCD_DATA23 引脚为 RGB 图像数据的 RED[7:0] 通道 */
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA16_LCDIF_DATA16, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA17_LCDIF_DATA17, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA18_LCDIF_DATA18, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA19_LCDIF_DATA19, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA20_LCDIF_DATA20, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA21_LCDIF_DATA21, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA22_LCDIF_DATA22, 0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_LCDIF_DATA23, 0);

    /* 屏幕的控制信号引脚：ENABLE, CLK, VSYNC, HSYNC */
    IOMUXC_SetPinMux(IOMUXC_LCD_ENABLE_LCDIF_ENABLE, 0); // IMX 的 LCD_ENABLE 引脚连接屏幕的 LCD_DE 引脚
    IOMUXC_SetPinMux(IOMUXC_LCD_HSYNC_LCDIF_HSYNC, 0); // IMX 的 LCD_HSYNC 引脚连接屏幕的 LCD_HSYNC 引脚
    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_LCDIF_VSYNC, 0); // IMX 的 LCD_VSYNC 引脚连接屏幕的 LCD_VSYNC 引脚
    IOMUXC_SetPinMux(IOMUXC_LCD_CLK_LCDIF_CLK, 0); // IMX 的 LCD_CLK 引脚连接屏幕的 LCD_PCLK 引脚

    /* IMX 的 GPIO1_IO08 引脚连接屏幕的背光控制引脚 BLT_PWM */
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_GPIO1_IO08, 0);

    /*
     * 设置引脚的电气特性
     * 设置引脚对应的 IOMUXC_SW_PAD_CTL_PAD_LCD_xxx 寄存器
     * 引脚对应寄存器位域的值，含义如下所示：
     * HYS[16]:    使能迟滞比较器， 0b0，    禁用
     * PUS[15:14]: 上拉/下拉电阻值，0b00，   100K 下拉
     * PUE[13]:    使用上拉/下拉(脉冲输出/输入) or 状态保持器(维持输出电平)，0b0，维持输出电平
     * PKE[12]:    状态保持器使能, 0b0，   禁用
     * ODE[11]:    开漏输出使能，  0b0，   禁用
     * SPEED[7:6]: IO 速率,      0b10，  100MHz
     * DSE[5:3]:   IO 驱动能力，  0b111， R0/7
     * SRE[0]:     压摆率，       0b1，   高压摆率
     * 因此，最终要写入寄存器的值为：0x000000B9
     */
    /* RGB 图像数据的 BLUE[7:0] 通道引脚的电气特性 */
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA00_LCDIF_DATA00, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA01_LCDIF_DATA01, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA02_LCDIF_DATA02, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA03_LCDIF_DATA03, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA04_LCDIF_DATA04, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA05_LCDIF_DATA05, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA06_LCDIF_DATA06, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_LCDIF_DATA07, 0x000000B9);

    /* RGB 图像数据的 GREEN[7:0] 通道引脚的电气特性 */
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA08_LCDIF_DATA08, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA09_LCDIF_DATA09, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA10_LCDIF_DATA10, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA11_LCDIF_DATA11, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA12_LCDIF_DATA12, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA13_LCDIF_DATA13, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA14_LCDIF_DATA14, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_LCDIF_DATA15, 0x000000B9);

    /* RGB 图像数据的 RED[7:0] 通道引脚的电气特性 */
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA16_LCDIF_DATA16, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA17_LCDIF_DATA17, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA18_LCDIF_DATA18, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA19_LCDIF_DATA19, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA20_LCDIF_DATA20, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA21_LCDIF_DATA21, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA22_LCDIF_DATA22, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_LCDIF_DATA23, 0x000000B9);

    /* 控制信号 CLK, ENABLE, HSYNC, VSYNC 引脚的电气特性 */
    IOMUXC_SetPinConfig(IOMUXC_LCD_CLK_LCDIF_CLK, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_ENABLE_LCDIF_ENABLE, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_HSYNC_LCDIF_HSYNC, 0x000000B9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_LCDIF_VSYNC, 0x000000B9);

    /* 背光控制引脚 GPIO1_IO08 引脚的电气特性 */
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_GPIO1_IO08, 0x000010B0);

    /*
     * 初始化背光控制引脚
     * 背光默认打开
     */
    pin_cfg bl_cfg;
    bl_cfg.dir = GPIO_OUTPUT;
    bl_cfg.def_val = LEVEL_HIGH; // 默认打开背光
    gpio_init(GPIO1, 8, &bl_cfg);

    pin_write(GPIO1, 8, 1); // 打开屏幕背光
}

/*
 * eLCDIF 模块复位
 * eLCDIF 模块的 LCDIF_CTRL 寄存器的 SFTRST[31] 为复位控制位
 * SFTRST[31] 置 1 强制复位
 * eLCDIF 正常运行时必须将该位置 0
 */
void lcd_reset(void)
{
    LCDIF->CTRL |= (1 << 31); // eLCDIF 模块复位
}

void lcd_noreset(void)
{
    LCDIF->CTRL &= ~(1 << 31); // eLCDIF 模块取消复位
}

/*
 * eLCDIF 模块使能
 * eLCDIF 模块的 LCDIF_CTRL 寄存器的 RUN[0] 为使能控制位
 * RUN[0] 置 1 使能 eLCDIF，开始将数据从 SoC 传输至屏幕
 * RUN[0] 置 0 禁用 eLCDIF 模块
 */
void lcd_enable(void)
{
    LCDIF->CTRL |= (1 << 0); // eLCDIF 模块使能
}

void lcd_disable(void)
{
    LCDIF->CTRL &= ~(1 << 0); // eLCDIF 模块禁用
}

/*
 * eLCDIF 时钟初始化
 * 
 * eLCDIF 的时钟源为 PLL5，其时钟频率计算公式如下：
 *      PLL5_CLK = OSC24M * (loopDivider + (numerator / denominator))
 * 这里为了简化计算，不使用小数分频，因此不设置 denominator 和 numerator
 * 因此，公式简化为：
 *      PLL5_CLK = OSC24M * (loopDivider)
 * 其中，loopDivider 的值由寄存器 CCM_ANALOG_PLL_VIDEO 的 DIV_SELECT[6:0] 设置
 * 
 * 时钟源 PLL5 输出时可以进行分频
 * 由寄存器 CCM_ANALOG_PLL_VIDEO 的 POST_DIV_SELECT[20:19] 位域，
 * 以及寄存器 CCM_ANALOG_MISC2 的 VIDEO_DIV[31:30] 共同决定
 * 输出频率的计算方式如下：
 *      OUTPUT_CLK = PLL5_CLK / POST_DIV / VIDEO_DIV
 *                 = PLL5_CLK / postDivider
 * 
 * eLCDIF 接口的时钟频率由时钟源经过分频器 1 和分频器 2 分频获得
 * 分频器 1 由寄存器 CCM_CSCDR2 的 LCDIF1_PRED[14:12] 位域控制，可以设置 0b000~0b111，对应 1~8 分频
 * 分频器 2 由寄存器 CCM_CBCMR 的 LCDIF1_PODF[25:23] 位域控制，可以设置 0b000~0b111，对应 1~8 分频
 * 因此，eLCDIF 的输出频率为：
 *      CLK = OUTPUT_CLK / PRED / PODF
 */
void lcd_clk_init(uint8_t loop_div, uint8_t pre_div, uint8_t post_div)
{
    /* 设置时钟源 PLL5 的频率 */
    /*
     * 不使用小数分频，因此：
     * CCM_ANALOG_PLL_VIDEO_DENOM 寄存器的 B[29:0] 位域设置为 1
     * CCM_ANALOG_PLL_VIDEO_NUM 寄存器的 A[29:0] 位域设置为 0
     */
    CCM_ANALOG->PLL_VIDEO_DENOM = 1; // 分母设置为 1
    CCM_ANALOG->PLL_VIDEO_NUM = 0; // 分子设置为 0

    /*
     * 设置时钟源 PLL5 的 loopDivider
     * CCM_ANALOG_PLL_VIDEO 寄存器的 DIV_SELECT[6:0] 位域设置 loopDivider 的值
     */
    CCM_ANALOG->PLL_VIDEO &= ~(0xFFFFFFFF); // 清空 DIV_SELECT[6:0]
    CCM_ANALOG->PLL_VIDEO |= (loop_div << 0); // 将 DIV_SELECT[6:0] 设置为用户给定的值 loop_div

    /*
     * 设置时钟源 PLL5 输出频率的 postDivider
     * postDivider 由寄存器 CCM_ANALOG_PLL_VIDEO 的 POST_DIV_SELECT[20:19] 位域，
     * 以及寄存器 CCM_ANALOG_MISC2 的 VIDEO_DIV[31:30] 位域共同决定
     */
    CCM_ANALOG->PLL_VIDEO &= ~(0b11 << 19); // 清空 POST_DIV_SELECT[20:19]
    CCM_ANALOG->PLL_VIDEO |= (0b10 << 19); // POST_DIV_SELECT[20:19] 设置为 0b10 (1 分频)
    CCM_ANALOG->MISC2 &= ~(0b11 << 30); // 清空 VIDEO_DIV[31:30]
    CCM_ANALOG->MISC2 |= (0b00 << 30); // VIDEO_DIV[31:30] 设置为 0b00 (1 分频)

    /* 使能 VIDEO PLL (PLL5) 时钟输出：ENABLE[13] 置 1 */
    CCM_ANALOG->PLL_VIDEO |= (0b1 << 13);

    printf("[lcd_clk_init] CCM_ANALOG->PLL_VIDEO = 0x%x\r\n", CCM_ANALOG->PLL_VIDEO);

    /*
     * eLCDIF 的时钟源选择 PLL5 (VIDEO PLL)
     * 寄存器 CCM_CSCDR2 的 LCDIF1_PRE_CLK_SEL[17:15] 设置 eLCDIF 的时钟源
     * 将其设置为 0b010，选择 PLL5 (VIDEO PLL) 作为 eLCDIF 的时钟源
     */
    CCM->CSCDR2 &= ~(0b111 << 15); // 清空 LCDIF1_PRE_CLK_SEL[17:15]
    CCM->CSCDR2 |= (0b010 << 15); // 选择 PLL5 (VIDEO PLL) 作为 eLCDIF 的时钟源

    /*
     * 设置 eLCDIF 接口的时钟频率
     * 分频器 1 由寄存器 CCM_CSCDR2 的 LCDIF1_PRED[14:12] 位域控制，可以设置 0b000~0b111，对应 1~8 分频
     * 分频器 2 由寄存器 CCM_CBCMR 的 LCDIF1_PODF[25:23] 位域控制，可以设置 0b000~0b111，对应 1~8 分频
     */
    /* LCDIF1_PRED[14:12] 设置为用户传入的值 pre_div */
    CCM->CSCDR2 &= ~(0b111 << 12); // 清空 LCDIF1_PRED[14:12]
    CCM->CSCDR2 |= ((pre_div - 1) << 12); // 设置 LCDIF1_PRED[14:12] 为用户传入的值 pre_div

    /* LCDIF1_PODF[25:23] 设置为用户传入的值 post_div */
    CCM->CBCMR &= ~(0b111 << 23); // 清空 LCDIF1_PODF[25:23]
    CCM->CBCMR |= ((post_div - 1) << 23); // 设置 LCDIF1_PODF[25:23] 为用户传入的值 post_div

    /* eLCDIF 的时钟源选择 PLL5 输出的时钟源 */
    CCM->CSCDR2 &= ~(0b111 << 9); // 清空 LCDIF1_CLK_SEL[11:9]
    CCM->CSCDR2 |= (0b000 << 9);
}

/* 在屏幕上指定位置画一个点 */
inline void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    uint32_t *buf_addr;
    uint32_t *target_addr;
    uint32_t position;

    buf_addr = (uint32_t *)tft_lcd.buf; // 获取显存的起始地址
    position = tft_lcd.pixel_size * (tft_lcd.width * y + x);
    target_addr = (uint32_t *)((uint32_t)buf_addr + position); // 计算目标地址

    *target_addr = color;
}

/* 读取指定像素点的颜色 */
inline uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint32_t *buf_addr;
    uint32_t *target_addr;
    uint32_t position;

    buf_addr = (uint32_t *)tft_lcd.buf; // 获取显存的起始地址
    position = tft_lcd.pixel_size * (tft_lcd.width * y + x);
    target_addr = (uint32_t *)((uint32_t)buf_addr + position); // 计算目标地址

    return *target_addr;
}

/* 绘制一个矩形块，矩形的左上角点坐标为 (x0, y0)，右下角点坐标为 (x1, y1) */
void lcd_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color)
{
    uint16_t x = 0;
    uint16_t y = 0;

    x0 = (x0 < 0) ? 0 : x0;
    y0 = (y0 < 0) ? 0 : y0;

    x1 = (x1 > tft_lcd.width) ? tft_lcd.width : x1;
    y1 = (y1 > tft_lcd.height) ? tft_lcd.height : y1;

    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            lcd_draw_point(x, y, color);
        }
    }
}

/* 清屏接口 (将全屏设置为指定的颜色) */
void lcd_clear(uint32_t color)
{
    uint32_t *buf_addr; // 显存首地址
    uint32_t frame_size; // 一帧图像包含多少个像素点
    uint32_t pixel = 0;

    buf_addr = (uint32_t *)tft_lcd.buf; // 获取显存起始地址
    frame_size = (uint32_t)(tft_lcd.width * tft_lcd.height); // 计算一帧图像包含多少个像素点

    for (pixel = 0; pixel < frame_size; pixel++) {
        buf_addr[pixel] = color; // 将每个像素点填充为指定的颜色
    }
}

/* LCD Panel 初始化 */
void lcd_init(void)
{
    uint16_t lcd_id = 0;

    lcd_panel_id_gpio_init(); // 初始化 Panel ID 相关的引脚
    lcd_id = lcd_read_panel_id(); // 获取 Panel ID
    printf("Panel ID = 0x%x\r\n", lcd_id); // 通过串口输出屏幕 ID

    lcd_gpio_init(); // 初始化数据、控制、背光引脚
    lcd_reset(); // eLCDIF 模块复位
    delay_ms(10); // 等待 10ms，等待 eLCDIF 模块复位完成
    lcd_noreset(); // 取消 eLCDIF 模块复位

    /* 屏幕参数初始化：根据 ID 设置对应的屏幕参数 */
    if (lcd_id == ATK4342) { // 4.3 英寸大小，480*272 分辨率
        tft_lcd.height = 272;
        tft_lcd.width  = 481;
        tft_lcd.vspw   = 1;
        tft_lcd.vbp    = 8;
        tft_lcd.vfp    = 8;
        tft_lcd.hspw   = 1;
        tft_lcd.hbp    = 40;
        tft_lcd.hfp    = 5;
        lcd_clk_init(27, 8, 8); // eLCDIF 时钟频率初始化：10.1 MHz
    } else if (lcd_id == ATK4384) { // 4.3 英寸大小， 800*480 分辨率
        tft_lcd.height = 480;
        tft_lcd.width  = 800;
        tft_lcd.vspw   = 3;
        tft_lcd.vbp    = 32;
        tft_lcd.vfp    = 13;
        tft_lcd.hspw   = 48;
        tft_lcd.hbp    = 88;
        tft_lcd.hfp    = 40;
        lcd_clk_init(42, 4, 8); // eLCDIF 时钟频率初始化：31.5MHz
    } else if (lcd_id == ATK7084) { // 7 英寸大小，800*480 分辨率
        tft_lcd.height = 480;
        tft_lcd.width  = 800;
        tft_lcd.vspw   = 1;
        tft_lcd.vbp    = 23;
        tft_lcd.vfp    = 22;
        tft_lcd.hspw   = 1;
        tft_lcd.hbp    = 46;
        tft_lcd.hfp    = 210;
        lcd_clk_init(30, 3, 7); // eLCDIF 时钟频率初始化：34.2MHz
    } else if (lcd_id == ATK7016) { // 7 英寸大小，1024*600 分辨率
        tft_lcd.height = 600;
        tft_lcd.width  = 1024;
        tft_lcd.vspw   = 3;
        tft_lcd.vbp    = 20;
        tft_lcd.vfp    = 12;
        tft_lcd.hspw   = 20;
        tft_lcd.hbp    = 140;
        tft_lcd.hfp    = 160;
        lcd_clk_init(32, 3, 5); // eLCDIF 时钟频率初始化：51.2MHz
    } else if (lcd_id == ATK1018) { // 10.1 英寸大小，1280*800 分辨率
        tft_lcd.height = 800;
        tft_lcd.width  = 1280;
        tft_lcd.vspw   = 3;
        tft_lcd.vbp    = 10;
        tft_lcd.vfp    = 10;
        tft_lcd.hspw   = 10;
        tft_lcd.hbp    = 80;
        tft_lcd.hfp    = 70;
        lcd_clk_init(35, 3, 5); // eLCDIF 时钟频率初始化：56MHz
    } else if (lcd_id == ATKVGA) { // VGA 接口，1366*768 分辨率
        tft_lcd.height = 768;
        tft_lcd.width  = 1366;
        tft_lcd.vspw   = 3;
        tft_lcd.vbp    = 24;
        tft_lcd.vfp    = 3;
        tft_lcd.hspw   = 143;
        tft_lcd.hbp    = 213;
        tft_lcd.hfp    = 70;
        lcd_clk_init(32, 3, 3); // eLCDIF 时钟频率初始化：85MHz
    }

    tft_lcd.id = lcd_id; // 屏幕 ID
    tft_lcd.pixel_size = PIXEL_SIZE_ARGB8888; // 像素点大小
    tft_lcd.buf = LCD_FRAMEBUF_ADDR; // 显存首地址
    tft_lcd.backcolor = LCD_WHITE; // 背景色为黑色
    tft_lcd.forecolor = LCD_WHITE; // 前景色为白色

    printf("CCM_ANALOG->PLL_VIDEO_NUM = 0x%x\r\n", CCM_ANALOG->PLL_VIDEO_NUM);
    printf("CCM_ANALOG->PLL_VIDEO_DENOM = 0x%x\r\n", CCM_ANALOG->PLL_VIDEO_DENOM);
    printf("CCM_ANALOG->MISC2 = 0x%x\r\n", CCM_ANALOG->MISC2);
    printf("CCM->CSCDR2 = 0x%x\r\n", CCM_ANALOG->PLL_VIDEO);
    printf("CCM->CBCMR = 0x%x\r\n", CCM_ANALOG->PLL_VIDEO);

    /* 初始化 eLCDIF 接口 */
    /*
     * 配置 LCDIF_CTRL 寄存器，相关位域的配置值及含义如下：
     * SFTRST[31]：                软件复位控制位，设置为 0，停止复位
     * BYPASS_COUNT[19]：          使能旁路计数器模式，设置为 1，使能计数器
     * DOTCLK_MODE[17]：           DOTCLK 模式使能，设置为1，LCD 工作在 DOTCLK 模式
     * INPUT_DATA_SWIZZLE[15:14]： 输入数据位置交换，设置为 0b00，输入数据不交换位置
     * CSC_DATA_SWIZZLE[13:12]：   CSC 数据位置交换，设置为 0b00，CSC 不交换位置
     * LCD_DATABUS_WIDTH[11:10]：  总线数据宽度，设置为 0b11，24 位总线宽度
     * WORD_LENGTH[9:8]：          输入数据宽度，设置为 0b11，24 位数据宽度，即 RGB888
     * MASTER[5]：                 主机模式，设置为 1，即 eLCDIF 工作在主机模式
     * DATA_FORMAT_24_BIT[1]：     输入数据格式，设置为 0，即所有的 24 位均有效
     */
    LCDIF->CTRL &= ~(0xFFFFFFFF); // 清空 LCDIF_CTRL 寄存器
    LCDIF->CTRL |= (1 << 19) | (1 << 17) | (3 << 10) | (3 << 8) | (1 << 5);
    printf("[lcd_init] LCDIF->CTRL = 0x%x\r\n", LCDIF->CTRL);

    /*
     * 配置 LCDIF_CTRL1 寄存器，相关位域的配置值及含义如下：
     * BYTE_PACKING_FORMAT[19:16]：有效数据标记，设置为 0b0111，即 ARGB8888 格式，不使用 Alpha 通道
     */
    LCDIF->CTRL1 &= ~(0b1111 << 16); // 清空 BYTE_PACKING_FORMAT[19:16]
    LCDIF->CTRL1 |= (0b0111 << 16); // 设置输入格式为 ARGB888，不使用 Alpha 通道的数据

    /*
     * 配置 LCDIF_TRANSFER_COUNT 寄存器，设置屏幕分辨率，相关位域的配置值及含义如下：
     * V_COUNT[31:16]：每一帧图像包含的有效行的数量
     * H_COUNT[15:0]：每一行包含的有效像素点的数量（即有效列的数量）
     */
    LCDIF->TRANSFER_COUNT &= ~(0xFFFFFFFF); // 清空 LCDIF_TRANSFER_COUNT 寄存器
    LCDIF->TRANSFER_COUNT |= ((tft_lcd.height << 16) | (tft_lcd.width << 0)); // 设置分辨率

    /*
     * VSYNC/DOTCLK 模式控制寄存器 LCDIF_VDCTRL0 配置，相关位域的配置值及含义如下：
     * VSYNC_OEB[29]：              0 : VSYNC 为输出信号
     * ENABLE_PRESENT[28]：         1 : 使能 ENABLE 信号输出
     * VSYNC_POL[27]：              0 : VSYNC 低电平有效
     * HSYNC_POL[26]：              0 : HSYNC 低电平有效
     * DOTCLK_POL[25]：             0 : DOTCLK 上升沿有效
     * ENABLE_POL[24]：             1 : ENABLE 信号高电平有效
     * VSYNC_PERIOD_UNIT[21]：      1 : DOTCLK 模式下设置为 1
     * VSYNC_PULSE_WIDTH_UNIT[20]： 1 : DOTCLK 模式下设置为 1
     * VSYNC_PULSE_WIDTH[17:0]： VSPW : VSYNC 信号的宽度
     */
    LCDIF->VDCTRL0 &= ~(0xFFFFFFFF); // 清空 LCDIF_VDCTRL0 寄存器
    if (lcd_id == ATKVGA) { // VGA 需要特殊处理
        LCDIF->VDCTRL0 |= (1 << 28) | (1 << 25) | (1 << 21) | (1 << 20) | (tft_lcd.vspw << 0);
    } else {
        LCDIF->VDCTRL0 |= (1 << 28) | (1 << 24) | (1 << 21) | (1 << 20) | (tft_lcd.vspw << 0);
    }

    /*
     * 配置 LCDIF_VDCTRL1 寄存器，设置 VSYNC 信号的周期
     * VSYNC_PERIOD[31:0]：设置一个 VSYNC 信号的周期（一个低电平时间加上一个高电平的总时间），即：屏幕高度 + VSPW + VBP + VFP
     */
    LCDIF->VDCTRL1 &= ~(0xFFFFFFFF); // 清空 LCDIF_VDCTRL1 寄存器
    LCDIF->VDCTRL1 |= (tft_lcd.height + tft_lcd.vspw + tft_lcd.vfp + tft_lcd.vbp);

    /*
     * 配置 LCDIF_VDCTRL2 寄存器，设置 HSYNC 信号的宽度和周期，相关位域的配置值及含义如下：
     * HSYNC_PULSE_WIDTH[31:18]：HSYNC 信号低电平的宽度，即 HSPW
     * HSYNC_PERIOD[17:0]：一个 HSYNC 信号的周期（一个低电平时间加上一个高电平的总时间）
     */
    LCDIF->VDCTRL2 &= ~(0xFFFFFFFF); // 清空 LCDIF_VDCTRL2 寄存器
    LCDIF->VDCTRL2 |= ((tft_lcd.hspw << 18) |
                    (tft_lcd.width + tft_lcd.hspw + tft_lcd.hfp + tft_lcd.hbp));

    /*
     * 配置 LCDIF_VDCTRL3 寄存器，设置 VSYNC 和 HSYNC 信号的 Porch 时间，相关位域的配置值及含义如下：
     * HORIZONTAL_WAIT_CNT[27:16]：DOTCLK 模式中，为 HSYNC 信号发送有效数据前的等待时间，即 HSPW + HBP
     * VERTICAL_WAIT_CNT[15:0]：DOTCLK 模式中，为 VSPW + VBP
     */
    LCDIF->VDCTRL3 &= ~(0xFFFFFFFF); // 清空 LCDIF_VDCTRL3 寄存器
    LCDIF->VDCTRL3 |= ((tft_lcd.hbp + tft_lcd.hspw) << 16) | (tft_lcd.vbp + tft_lcd.vspw);

    /*
     * 配置 LCDIF_VDCTRL4 寄存器，DOTCLK 模式控制，相关位域的配置值及含义如下：
     * SYNC_SIGNALS_ON[18]：DOTCLK 模式中该位为 1
     * DOTCLK_H_VALID_DATA_CNT[17:0]：DOTCLK 模式中，为一行所包含的有效像素的数量，即 LCD 屏幕的宽度
     */
    LCDIF->VDCTRL4 &= ~(0xFFFFFFFF); // 清空 LCDIF_VDCTRL4 寄存器
    LCDIF->VDCTRL4 |= ((1 << 18) | (tft_lcd.width));

    /*
     * 设置 eLCDIF 接口显存的首地址，配置 LCDIF_CUR_BUF 和 LCDIF_NEXT_BUF 寄存器
     */
    LCDIF->CUR_BUF = (uint32_t)tft_lcd.buf;
    LCDIF->NEXT_BUF = (uint32_t)tft_lcd.buf;

    printf("LCDIF->CTRL = 0x%x\r\n", LCDIF->CTRL);
    printf("LCDIF->CTRL1 = 0x%x\r\n", LCDIF->CTRL1);
    printf("LCDIF->TRANSFER_COUNT = 0x%x\r\n", LCDIF->TRANSFER_COUNT);
    printf("LCDIF->VDCTRL0 = 0x%x\r\n", LCDIF->VDCTRL0);
    printf("LCDIF->VDCTRL1 = 0x%x\r\n", LCDIF->VDCTRL1);
    printf("LCDIF->VDCTRL2 = 0x%x\r\n", LCDIF->VDCTRL2);
    printf("LCDIF->VDCTRL3 = 0x%x\r\n", LCDIF->VDCTRL3);
    printf("LCDIF->VDCTRL4 = 0x%x\r\n", LCDIF->VDCTRL4);
    printf("LCDIF->CUR_BUF = 0x%x\r\n", LCDIF->CUR_BUF);
    printf("LCDIF->NEXT_BUF = 0x%x\r\n", LCDIF->NEXT_BUF);

    /* 屏幕配置完成后使能 eLCDIF 并清除屏幕 */
    lcd_enable(); // eLCDIF 使能
    delay_ms(10); // 延时 10ms

    lcd_clear(LCD_RED); // 清屏（将屏幕设置为指定颜色）

    lcd_fill(100, 100, 200, 200, LCD_BLUE);
}
