#include "lcd.h"
#include "gpio.h"
#include "delay.h"
#include "stdio.h"

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
 */

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
    if (lcd_id == ATK4342) {
        tft_lcd.height = 272;
        tft_lcd.width  = 480;
        tft_lcd.vspw   = 1;
        tft_lcd.vbpd   = 8;
        tft_lcd.vfpd   = 8;
        tft_lcd.hspw   = 1;
        tft_lcd.hbpd   = 40;
        tft_lcd.hfpd   = 5;
        lcdclk_init(27, 8, 8);	/* 初始化LCD时钟 10.1MHz */
    } else if (lcd_id == ATK4384) {
        tft_lcd.height = 480;	
        tft_lcd.width = 800;
        tft_lcd.vspw = 3;
        tft_lcd.vbpd = 32;
        tft_lcd.vfpd = 13;
        tft_lcd.hspw = 48;
        tft_lcd.hbpd = 88;
        tft_lcd.hfpd = 40;
        lcdclk_init(42, 4, 8);	/* 初始化LCD时钟 31.5MHz */
    } else if (lcd_id == ATK7084) {
        tft_lcd.height = 480;	
        tft_lcd.width = 800;
        tft_lcd.vspw = 1;
        tft_lcd.vbpd = 23;
        tft_lcd.vfpd = 22;
        tft_lcd.hspw = 1;
        tft_lcd.hbpd = 46;
        tft_lcd.hfpd = 210;	
        lcdclk_init(30, 3, 7);	/* 初始化LCD时钟 34.2MHz */
    } else if (lcd_id == ATK7016) {
        tft_lcd.height = 600;	
        tft_lcd.width = 1024;
        tft_lcd.vspw = 3;
        tft_lcd.vbpd = 20;
        tft_lcd.vfpd = 12;
        tft_lcd.hspw = 20;
        tft_lcd.hbpd = 140;
        tft_lcd.hfpd = 160;
        lcdclk_init(32, 3, 5);	/* 初始化LCD时钟 51.2MHz */
    } else if (lcd_id == ATK1018) {
        tft_lcd.height = 800;	
        tft_lcd.width = 1280;
        tft_lcd.vspw = 3;
        tft_lcd.vbpd = 10;
        tft_lcd.vfpd = 10;
        tft_lcd.hspw = 10;
        tft_lcd.hbpd = 80;
        tft_lcd.hfpd = 70;
        lcdclk_init(35, 3, 5);	/* 初始化LCD时钟 56MHz */
    } else if (lcd_id == ATKVGA) {  
        tft_lcd.height = 768;	
        tft_lcd.width = 1366;
        tft_lcd.vspw = 3;
        tft_lcd.vbpd = 24;
        tft_lcd.vfpd = 3;
        tft_lcd.hspw = 143;
        tft_lcd.hbpd = 213;
        tft_lcd.hfpd = 70;
        lcdclk_init(32, 3, 3);	/* 初始化LCD时钟 85MHz */
    }
}
