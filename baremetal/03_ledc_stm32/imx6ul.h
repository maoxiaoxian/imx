#ifndef _IMX6UL_H
#define _IMX6UL_H

typedef volatile unsigned int uint32_t;

// CCM_CCGR 寄存器复位值
#define CCGR_RST_VAL 0xFFFFFFFF

// 时钟控制模块寄存器基地址
#define CCM_BASE        (0x020C4000)
#define CCM_ANALOG_BASE (0x020C8000)

// IO 控制寄存器基地址
#define IOMUX_SW_MUX_BASE (0x020E0014)
#define IOMUX_SW_PAD_BASE (0x020E0204)

// GPIO 控制寄存器基地址
#define GPIO1_BASE (0x0209C000)
#define GPIO2_BASE (0x020A0000)
#define GPIO3_BASE (0x020A4000)
#define GPIO4_BASE (0x020A8000)
#define GPIO5_BASE (0x020AC000)

// 时钟控制寄存器定义 - CCM
typedef struct {
    uint32_t CCR;
    uint32_t CCDR;
    uint32_t CSR;
    uint32_t CCSR;
    uint32_t CACRR;
    uint32_t CBCDR;
    uint32_t CBCMR;
    uint32_t CSCMR1;
    uint32_t CSCMR2;
    uint32_t CSCDR1;
    uint32_t CS1CDR;
    uint32_t CS2CDR;
    uint32_t CDCDR;
    uint32_t CHSCCDR;
    uint32_t CSCDR2;
    uint32_t CSCDR3;
    uint32_t RESERVED_1[2];
    uint32_t CDHIPR;  
    uint32_t RESERVED_2[2];
    uint32_t CLPCR;
    uint32_t CISR;
    uint32_t CIMR;
    uint32_t CCOSR;
    uint32_t CGPR;
    uint32_t CCGR0;
    uint32_t CCGR1;
    uint32_t CCGR2;
    uint32_t CCGR3;
    uint32_t CCGR4;
    uint32_t CCGR5;
    uint32_t CCGR6;
    uint32_t RESERVED_3[1];
    uint32_t CMEOR;
} CCM_Type;

// 时钟控制寄存器定义 - CCM_ANALOG
typedef struct {
    uint32_t PLL_ARM;
    uint32_t PLL_ARM_SET;
    uint32_t PLL_ARM_CLR;
    uint32_t PLL_ARM_TOG;
    uint32_t PLL_USB1;
    uint32_t PLL_USB1_SET;
    uint32_t PLL_USB1_CLR;
    uint32_t PLL_USB1_TOG;
    uint32_t PLL_USB2;
    uint32_t PLL_USB2_SET;
    uint32_t PLL_USB2_CLR;
    uint32_t PLL_USB2_TOG;
    uint32_t PLL_SYS;
    uint32_t PLL_SYS_SET;
    uint32_t PLL_SYS_CLR;
    uint32_t PLL_SYS_TOG;
    uint32_t PLL_SYS_SS;
    uint32_t RESERVED_1[3];
    uint32_t PLL_SYS_NUM;
    uint32_t RESERVED_2[3];
    uint32_t PLL_SYS_DENOM; 
    uint32_t RESERVED_3[3];
    uint32_t PLL_AUDIO;
    uint32_t PLL_AUDIO_SET;
    uint32_t PLL_AUDIO_CLR;
    uint32_t PLL_AUDIO_TOG;
    uint32_t PLL_AUDIO_DENOM;
    uint32_t RESERVED_4[3];
    uint32_t PLL_VIDEO;
    uint32_t PLL_VIDEO_SET;
    uint32_t PLL_VIDEO_CLR;
    uint32_t PLL_VIDEO_TOG;
    uint32_t PLL_VIDEO_NUM;
    uint32_t RESERVED_5[3];
    uint32_t PLL_VIDEO_DENOM;
    uint32_t RESERVED_6[7];
    uint32_t PLL_ENET;
    uint32_t PLL_ENET_SET;
    uint32_t PLL_ENET_CLR;
    uint32_t PLL_ENET_TOG;
    uint32_t PFD_480;
    uint32_t PFD_480_SET;
    uint32_t PFD_480_CLR;
    uint32_t PFD_480_TOG;
    uint32_t PFD_528;
    uint32_t PFD_528_SET;
    uint32_t PFD_528_CLR;
    uint32_t PFD_528_TOG;
    uint32_t RESERVED_7[16];
    uint32_t MISC0;
    uint32_t MISC0_SET;
    uint32_t MISC0_CLR;
    uint32_t MISC0_TOG;
    uint32_t MISC1;
    uint32_t MISC1_SET;
    uint32_t MISC1_CLR;
    uint32_t MISC1_TOG;
    uint32_t MISC2;
    uint32_t MISC2_SET;
    uint32_t MISC2_CLR;
    uint32_t MISC2_TOG;
} CCM_ANALOG_Type;

// IOMUX 寄存器定义 - IOMUX_SW_MUX
typedef struct {
    uint32_t BOOT_MODE0;
    uint32_t BOOT_MODE1;
    uint32_t SNVS_TAMPER0;
    uint32_t SNVS_TAMPER1;
    uint32_t SNVS_TAMPER2;
    uint32_t SNVS_TAMPER3;
    uint32_t SNVS_TAMPER4;
    uint32_t SNVS_TAMPER5;
    uint32_t SNVS_TAMPER6;
    uint32_t SNVS_TAMPER7;
    uint32_t SNVS_TAMPER8;
    uint32_t SNVS_TAMPER9;
    uint32_t JTAG_MOD;
    uint32_t JTAG_TMS;
    uint32_t JTAG_TDO;
    uint32_t JTAG_TDI;
    uint32_t JTAG_TCK;
    uint32_t JTAG_TRST_B;
    uint32_t GPIO1_IO00;
    uint32_t GPIO1_IO01;
    uint32_t GPIO1_IO02;
    uint32_t GPIO1_IO03;
    uint32_t GPIO1_IO04;
    uint32_t GPIO1_IO05;
    uint32_t GPIO1_IO06;
    uint32_t GPIO1_IO07;
    uint32_t GPIO1_IO08;
    uint32_t GPIO1_IO09;
    uint32_t UART1_TX_DATA;
    uint32_t UART1_RX_DATA;
    uint32_t UART1_CTS_B;
    uint32_t UART1_RTS_B;
    uint32_t UART2_TX_DATA;
    uint32_t UART2_RX_DATA;
    uint32_t UART2_CTS_B;
    uint32_t UART2_RTS_B;
    uint32_t UART3_TX_DATA;
    uint32_t UART3_RX_DATA;
    uint32_t UART3_CTS_B;
    uint32_t UART3_RTS_B;
    uint32_t UART4_TX_DATA;
    uint32_t UART4_RX_DATA;
    uint32_t UART5_TX_DATA;
    uint32_t UART5_RX_DATA;
    uint32_t ENET1_RX_DATA0;
    uint32_t ENET1_RX_DATA1;
    uint32_t ENET1_RX_EN;
    uint32_t ENET1_TX_DATA0;
    uint32_t ENET1_TX_DATA1;
    uint32_t ENET1_TX_EN;
    uint32_t ENET1_TX_CLK;
    uint32_t ENET1_RX_ER;
    uint32_t ENET2_RX_DATA0;
    uint32_t ENET2_RX_DATA1;
    uint32_t ENET2_RX_EN;
    uint32_t ENET2_TX_DATA0;
    uint32_t ENET2_TX_DATA1;
    uint32_t ENET2_TX_EN;
    uint32_t ENET2_TX_CLK;
    uint32_t ENET2_RX_ER;
    uint32_t LCD_CLK;
    uint32_t LCD_ENABLE;
    uint32_t LCD_HSYNC;
    uint32_t LCD_VSYNC;
    uint32_t LCD_RESET;
    uint32_t LCD_DATA00;
    uint32_t LCD_DATA01;
    uint32_t LCD_DATA02;
    uint32_t LCD_DATA03;
    uint32_t LCD_DATA04;
    uint32_t LCD_DATA05;
    uint32_t LCD_DATA06;
    uint32_t LCD_DATA07;
    uint32_t LCD_DATA08;
    uint32_t LCD_DATA09;
    uint32_t LCD_DATA10;
    uint32_t LCD_DATA11;
    uint32_t LCD_DATA12;
    uint32_t LCD_DATA13;
    uint32_t LCD_DATA14;
    uint32_t LCD_DATA15;
    uint32_t LCD_DATA16;
    uint32_t LCD_DATA17;
    uint32_t LCD_DATA18;
    uint32_t LCD_DATA19;
    uint32_t LCD_DATA20;
    uint32_t LCD_DATA21;
    uint32_t LCD_DATA22;
    uint32_t LCD_DATA23;
    uint32_t NAND_RE_B;
    uint32_t NAND_WE_B;
    uint32_t NAND_DATA00;
    uint32_t NAND_DATA01;
    uint32_t NAND_DATA02;
    uint32_t NAND_DATA03;
    uint32_t NAND_DATA04;
    uint32_t NAND_DATA05;
    uint32_t NAND_DATA06;
    uint32_t NAND_DATA07;
    uint32_t NAND_ALE;
    uint32_t NAND_WP_B;
    uint32_t NAND_READY_B;
    uint32_t NAND_CE0_B;
    uint32_t NAND_CE1_B;
    uint32_t NAND_CLE;
    uint32_t NAND_DQS;
    uint32_t SD1_CMD;
    uint32_t SD1_CLK;
    uint32_t SD1_DATA0;
    uint32_t SD1_DATA1;
    uint32_t SD1_DATA2;
    uint32_t SD1_DATA3;
    uint32_t CSI_MCLK;
    uint32_t CSI_PIXCLK;
    uint32_t CSI_VSYNC;
    uint32_t CSI_HSYNC;
    uint32_t CSI_DATA00;
    uint32_t CSI_DATA01;
    uint32_t CSI_DATA02;
    uint32_t CSI_DATA03;
    uint32_t CSI_DATA04;
    uint32_t CSI_DATA05;
    uint32_t CSI_DATA06;
    uint32_t CSI_DATA07;
} IOMUX_SW_MUX_Type;

// IOMUX 寄存器定义 - IOMUX_SW_PAD
typedef struct {
    uint32_t DRAM_ADDR00;
    uint32_t DRAM_ADDR01;
    uint32_t DRAM_ADDR02;
    uint32_t DRAM_ADDR03;
    uint32_t DRAM_ADDR04;
    uint32_t DRAM_ADDR05;
    uint32_t DRAM_ADDR06;
    uint32_t DRAM_ADDR07;
    uint32_t DRAM_ADDR08;
    uint32_t DRAM_ADDR09;
    uint32_t DRAM_ADDR10;
    uint32_t DRAM_ADDR11;
    uint32_t DRAM_ADDR12;
    uint32_t DRAM_ADDR13;
    uint32_t DRAM_ADDR14;
    uint32_t DRAM_ADDR15;
    uint32_t DRAM_DQM0;
    uint32_t DRAM_DQM1;
    uint32_t DRAM_RAS_B;
    uint32_t DRAM_CAS_B;
    uint32_t DRAM_CS0_B;
    uint32_t DRAM_CS1_B;
    uint32_t DRAM_SDWE_B;
    uint32_t DRAM_ODT0;
    uint32_t DRAM_ODT1;
    uint32_t DRAM_SDBA0;
    uint32_t DRAM_SDBA1;
    uint32_t DRAM_SDBA2;
    uint32_t DRAM_SDCKE0;
    uint32_t DRAM_SDCKE1;
    uint32_t DRAM_SDCLK0_P;
    uint32_t DRAM_SDQS0_P;
    uint32_t DRAM_SDQS1_P;
    uint32_t DRAM_RESET;
    uint32_t TEST_MODE;
    uint32_t POR_B;
    uint32_t ONOFF;
    uint32_t SNVS_PMIC_ON_REQ;
    uint32_t CCM_PMIC_STBY_REQ;
    uint32_t BOOT_MODE0;
    uint32_t BOOT_MODE1;
    uint32_t SNVS_TAMPER0;
    uint32_t SNVS_TAMPER1;
    uint32_t SNVS_TAMPER2;
    uint32_t SNVS_TAMPER3;
    uint32_t SNVS_TAMPER4;
    uint32_t SNVS_TAMPER5;
    uint32_t SNVS_TAMPER6;
    uint32_t SNVS_TAMPER7;
    uint32_t SNVS_TAMPER8;
    uint32_t SNVS_TAMPER9;
    uint32_t JTAG_MOD;
    uint32_t JTAG_TMS;
    uint32_t JTAG_TDO;
    uint32_t JTAG_TDI;
    uint32_t JTAG_TCK;
    uint32_t JTAG_TRST_B;
    uint32_t GPIO1_IO00;
    uint32_t GPIO1_IO01;
    uint32_t GPIO1_IO02;
    uint32_t GPIO1_IO03;
    uint32_t GPIO1_IO04;
    uint32_t GPIO1_IO05;
    uint32_t GPIO1_IO06;
    uint32_t GPIO1_IO07;
    uint32_t GPIO1_IO08;
    uint32_t GPIO1_IO09;
    uint32_t UART1_TX_DATA;
    uint32_t UART1_RX_DATA;
    uint32_t UART1_CTS_B;
    uint32_t UART1_RTS_B;
    uint32_t UART2_TX_DATA;
    uint32_t UART2_RX_DATA;
    uint32_t UART2_CTS_B;
    uint32_t UART2_RTS_B;
    uint32_t UART3_TX_DATA;
    uint32_t UART3_RX_DATA;
    uint32_t UART3_CTS_B;
    uint32_t UART3_RTS_B;
    uint32_t UART4_TX_DATA;
    uint32_t UART4_RX_DATA;
    uint32_t UART5_TX_DATA;
    uint32_t UART5_RX_DATA;
    uint32_t ENET1_RX_DATA0;
    uint32_t ENET1_RX_DATA1;
    uint32_t ENET1_RX_EN;
    uint32_t ENET1_TX_DATA0;
    uint32_t ENET1_TX_DATA1;
    uint32_t ENET1_TX_EN;
    uint32_t ENET1_TX_CLK;
    uint32_t ENET1_RX_ER;
    uint32_t ENET2_RX_DATA0;
    uint32_t ENET2_RX_DATA1;
    uint32_t ENET2_RX_EN;
    uint32_t ENET2_TX_DATA0;
    uint32_t ENET2_TX_DATA1;
    uint32_t ENET2_TX_EN;
    uint32_t ENET2_TX_CLK;
    uint32_t ENET2_RX_ER;
    uint32_t LCD_CLK;
    uint32_t LCD_ENABLE;
    uint32_t LCD_HSYNC;
    uint32_t LCD_VSYNC;
    uint32_t LCD_RESET;
    uint32_t LCD_DATA00;
    uint32_t LCD_DATA01;
    uint32_t LCD_DATA02;
    uint32_t LCD_DATA03;
    uint32_t LCD_DATA04;
    uint32_t LCD_DATA05;
    uint32_t LCD_DATA06;
    uint32_t LCD_DATA07;
    uint32_t LCD_DATA08;
    uint32_t LCD_DATA09;
    uint32_t LCD_DATA10;
    uint32_t LCD_DATA11;
    uint32_t LCD_DATA12;
    uint32_t LCD_DATA13;
    uint32_t LCD_DATA14;
    uint32_t LCD_DATA15;
    uint32_t LCD_DATA16;
    uint32_t LCD_DATA17;
    uint32_t LCD_DATA18;
    uint32_t LCD_DATA19;
    uint32_t LCD_DATA20;
    uint32_t LCD_DATA21;
    uint32_t LCD_DATA22;
    uint32_t LCD_DATA23;
    uint32_t NAND_RE_B;
    uint32_t NAND_WE_B;
    uint32_t NAND_DATA00;
    uint32_t NAND_DATA01;
    uint32_t NAND_DATA02;
    uint32_t NAND_DATA03;
    uint32_t NAND_DATA04;
    uint32_t NAND_DATA05;
    uint32_t NAND_DATA06;
    uint32_t NAND_DATA07;
    uint32_t NAND_ALE;
    uint32_t NAND_WP_B;
    uint32_t NAND_READY_B;
    uint32_t NAND_CE0_B;
    uint32_t NAND_CE1_B;
    uint32_t NAND_CLE;
    uint32_t NAND_DQS;
    uint32_t SD1_CMD;
    uint32_t SD1_CLK;
    uint32_t SD1_DATA0;
    uint32_t SD1_DATA1;
    uint32_t SD1_DATA2;
    uint32_t SD1_DATA3;
    uint32_t CSI_MCLK;
    uint32_t CSI_PIXCLK;
    uint32_t CSI_VSYNC;
    uint32_t CSI_HSYNC;
    uint32_t CSI_DATA00;
    uint32_t CSI_DATA01;
    uint32_t CSI_DATA02;
    uint32_t CSI_DATA03;
    uint32_t CSI_DATA04;
    uint32_t CSI_DATA05;
    uint32_t CSI_DATA06;
    uint32_t CSI_DATA07;
    uint32_t GRP_ADDDS;
    uint32_t GRP_DDRMODE_CTL;
    uint32_t GRP_B0DS;
    uint32_t GRP_DDRPK;
    uint32_t GRP_CTLDS;
    uint32_t GRP_B1DS;
    uint32_t GRP_DDRHYS;
    uint32_t GRP_DDRPKE;
    uint32_t GRP_DDRMODE;
    uint32_t GRP_DDR_TYPE;
} IOMUX_SW_PAD_Type;

// GPIO 控制寄存器定义 - GPIO
typedef struct {
    uint32_t DR;
    uint32_t GDIR;
    uint32_t PSR;
    uint32_t ICR1;
    uint32_t ICR2;
    uint32_t IMR;
    uint32_t ISR;
    uint32_t EDGE_SEL;
} GPIO_Type;

// 外设指针
#define CCM          ((CCM_Type *)CCM_BASE)
#define CCM_ANALOG   ((CCM_ANALOG_Type *)CCM_ANALOG_BASE)
#define IOMUX_SW_MUX ((IOMUX_SW_MUX_Type *)IOMUX_SW_MUX_BASE)
#define IOMUX_SW_PAD ((IOMUX_SW_PAD_Type *)IOMUX_SW_PAD_BASE)
#define GPIO1        ((GPIO_Type *)GPIO1_BASE)
#define GPIO2        ((GPIO_Type *)GPIO2_BASE)
#define GPIO3        ((GPIO_Type *)GPIO3_BASE)
#define GPIO4        ((GPIO_Type *)GPIO4_BASE)
#define GPIO5        ((GPIO_Type *)GPIO5_BASE)

#endif /* _IMX6UL_H */
