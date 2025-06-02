#include "gt9147.h"
#include "i2c.h"
#include "int.h"
#include "delay.h"
#include "stdio.h"

struct gt9147_dev_t gt9147_dev;
int gt_init_fail = 0;

/*
 * GT9147 配置参数表
 * 第一个字节为版本号 (0x61)，
 * 必须保证新的版本号大于等于 GT9147 内部 flash 的原有版本号，才会更新配置
 */
u8 GT9147_CFG_TBL[] = {
    0x41,0xe0,0x01,0x10,0x01,0x05,0x0d,0x00,0x01,0x08,
    0x28,0x05,0x50,0x32,0x03,0x05,0x00,0x00,0xff,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x28,0x0a,
    0x17,0x15,0x31,0x0d,0x00,0x00,0x02,0x9b,0x03,0x25,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
    0x00,0x0f,0x94,0x94,0xc5,0x02,0x07,0x00,0x00,0x04,
    0x8d,0x13,0x00,0x5c,0x1e,0x00,0x3c,0x30,0x00,0x29,
    0x4c,0x00,0x1e,0x78,0x00,0x1e,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,
    0x18,0x1a,0x00,0x00,0x00,0x00,0x1f,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0x00,0x02,0x04,0x05,0x06,0x08,0x0a,0x0c,
    0x0e,0x1d,0x1e,0x1f,0x20,0x22,0x24,0x28,0x29,0xff,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,
};

/*
 * @description    : 初始化触摸屏，初始化 GT9147
 * @param          : 无
 * @return         : 无
 */
void gt9147_init(void)
{
    unsigned char temp[7];
    pin_cfg ctintpin_config;
    pin_cfg ctretpin_config;
    gt9147_dev.initfalg = GT9147_INIT_NOTFINISHED;
    int i;

    printf("[gt9147_init]: Init!\r\n");
    printf("[gt9147_init]: gt9147_dev.initfalg = %d\r\n", gt9147_dev.initfalg);

    /* 避免编译器自动赋值 */
    for (i = 0; i < 5; i++) {
        gt9147_dev.x[i] = 0;
        gt9147_dev.y[i] = 0;
    }

    gt9147_dev.point_num = 0;

    /*
     * 初始化 I2C2 的 IO，引脚连接关系如下
     *      I2C2_SCL --> UART5_TXD
     *      I2C2_SDA --> UART5_RXD
     */
    IOMUXC_SetPinMux(IOMUXC_UART5_TX_DATA_I2C2_SCL, 1);
    IOMUXC_SetPinMux(IOMUXC_UART5_RX_DATA_I2C2_SDA, 1);

    /*
     * 配置 I2C2 IO 的电气属性
     * HYS[16]:    0b0,   HYS 关闭
     * PUS[15:14]: 0b1,   默认 47K 上拉
     * PUE[13]:    0b1,   pull 功能
     * PKE[12]:    0b1,   pull/keeper 使能
     * ODE[11]:    0b0,   关闭开路输出
     * SPEED[7:6]: 0b10,  速度 100Mhz
     * DSE[5:3]:   0b110, 驱动能力为 R0/6
     * SRE[0]:     0b1,   高转换率
     */
    IOMUXC_SetPinConfig(IOMUXC_UART5_TX_DATA_I2C2_SCL, 0x70B0);
    IOMUXC_SetPinConfig(IOMUXC_UART5_RX_DATA_I2C2_SDA, 0X70B0);

    /* 初始化触摸屏的中断 IO 和复位 IO */
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0);        /* 复用为 GPIO1_IO9 */
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09, 0); /* 复用为 GPIO5_IO9 */

    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO09_GPIO1_IO09, 0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER9_GPIO5_IO09, 0X10B0);

    /* 中断 IO 初始化 */
    ctintpin_config.dir = GPIO_OUTPUT;
    ctintpin_config.def_val = LEVEL_HIGH;
    ctintpin_config.irq_mode = IRQ_MODE_NONE;
    gpio_init(GPIO1, 9, &ctintpin_config);

    /* 复位 IO 初始化 */
    ctretpin_config.dir = GPIO_OUTPUT;
    ctretpin_config.def_val = LEVEL_HIGH;
    ctretpin_config.irq_mode = IRQ_MODE_NONE;
    gpio_init(GPIO5, 9, &ctretpin_config);

    /* 初始化 I2C */
    i2c_init(I2C2);

    /* 初始化 GT9147，要严格按照 GT9147 时序要求 */
    pin_write(GPIO5, 9, 0); /* 复位 GT9147 */
    delay_ms(10);
    pin_write(GPIO5, 9, 1); /* 停止复位 GT9147 */
    delay_ms(10);
    pin_write(GPIO1, 9, 0); /* 拉低 INT 引脚 */
    delay_ms(100);

    gt9147_read_len(GT9147_ADDR, GT_PID_REG, 6, temp);/* 读取产品 ID */
    temp[6] = temp[4];
    temp[4] = 0;
    printf("CTP ID: %s\r\n", temp); /* 打印 ID */
    printf("Default Ver: %#x\r\n",((temp[5]<<8) | temp[6])); /* 打印固件版本 */

    /* 重新设置中断 IO，配置为中断功能 */
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO09_GPIO1_IO09,0x0080);
    ctintpin_config.dir = GPIO_INPUT;
    ctintpin_config.def_val = LEVEL_LOW;

    if (temp[0] != 0x31) {
        temp[0] = gt9147_read_byte(GT9147_ADDR, 0x804D) & 0x3; /* 获取中断模式 */
        printf("9xx\r\n");
    } else {
        temp[0]=gt9147_read_byte(GT9147_ADDR, 0x8056) & 0x3;
    }

    /* 设置中断触发方式 */
    switch (temp[0])
    {
        case 0x0:
            printf("InterruptMode: IntRisingEdge\r\n");
            ctintpin_config.irq_mode = IRQ_MODE_REDGE;
            break;
        case 0x1:
            printf("InterruptMode: IntFallingEdge\r\n");
            ctintpin_config.irq_mode = IRQ_MODE_FEDGE;
            break;
        case 0x2:
            printf("InterruptMode: IntLowLevel\r\n");
            ctintpin_config.irq_mode = IRQ_MODE_LOW;
            break;
        case 0x3:
            printf("InterruptMode: IntHighLevel\r\n");
            ctintpin_config.irq_mode = IRQ_MODE_HIGH;
            break;
        default : printf("InterruptMode: Error\r\n");
            ctintpin_config.irq_mode = IRQ_MODE_RFEDGE;
            break;
    }
    gpio_init(GPIO1, 9, &ctintpin_config);

    if (gt9147_write_byte(GT9147_ADDR, GT_CTRL_REG, 0x02)) {
        printf("[gt9147_init]: Init Fail!\r\n");
        gt_init_fail = 1;  // gt 系列初始化失败
        goto done;
    }

    delay_ms(10);
    gt9147_write_byte(GT9147_ADDR, GT_CTRL_REG, 0);
    printf("[gt9147_init]: GIC_EnableIRQ()!\r\n");
    GIC_EnableIRQ(GPIO1_Combined_0_15_IRQn); /* 使能 GIC 中对应的中断 */
    printf("[gt9147_init]: irq_handler_register()!\r\n");
    irq_handler_register(GPIO1_Combined_0_15_IRQn, (irq_handler)gt9147_irqhandler, NULL); /* 注册中断服务函数 */
    printf("[gt9147_init]: pin_int_enable()!\r\n");
    pin_int_enable(GPIO1, 9); /* 使能 GPIO1_IO09 的中断功能 */

    printf("[gt9147_init]: Mark Done!\r\n");
    // delay_ms(50); // 这里如果延时，会导致程序卡死，原因待分析
    printf("[gt9147_init]: initfalg = \r\n");
    gt9147_dev.initfalg = GT9147_INIT_FINISHED; /* 标记 GT9147 初始化完成 */
    gt9147_dev.intflag = 0;

    printf("[gt9147_init]: Init Done!\r\n");

done:
    delay_ms(10);
}

/*
 * @description : GPIO1_IO9 最终的中断处理函数
 * @param       : 无
 * @return      : 无
 */
void gt9147_irqhandler(void)
{ 
    if (gt9147_dev.initfalg == GT9147_INIT_FINISHED) {
        gt9147_dev.intflag = 1;
        gt9147_read_tpcoord();
    }

    pin_int_clear(GPIO1, 9); /* 清除中断标志位 */
}

/*
 * @description    : 向 GT9147 写入数据
 * @param - addr   : 设备地址
 * @param - reg    : 要写入的寄存器
 * @param - data   : 要写入的数据
 * @return         : 操作结果
 */
unsigned char gt9147_write_byte(unsigned char addr,unsigned int reg, unsigned char data)
{
    unsigned char status=0;
    unsigned char writedata=data;
    i2c_transfer masterXfer;

    // printf("[gt9147_write_byte]: Ready to Write!\r\n");

    /* 配置I2C xfer结构体 */
    masterXfer.slave_addr = addr;   /* 设备地址 */
    masterXfer.dir = I2C_WRITE;     /* 写入数据 */
    masterXfer.reg_addr = reg;      /* 要写入的寄存器地址 */
    masterXfer.reg_addr_len = 2;    /* 地址长度一个字节 */
    masterXfer.data = &writedata;   /* 要写入的数据    */
    masterXfer.data_len = 1;        /* 写入数据长度1个字节 */

    if (i2c_master_transfer(I2C2, &masterXfer))
        status = 1;

    return status;
}

/*
 * @description    : 从 GT9147 读取一个字节的数据
 * @param - addr   : 设备地址
 * @param - reg    : 要读取的寄存器
 * @return         : 读取到的数据。
 */
unsigned char gt9147_read_byte(unsigned char addr,unsigned int reg)
{
    unsigned char val=0;
    i2c_transfer masterXfer;

    masterXfer.slave_addr = addr; /* 设备地址 */
    masterXfer.dir = I2C_READ;    /* 读取数据 */
    masterXfer.reg_addr = reg;    /* 要读取的寄存器地址 */
    masterXfer.reg_addr_len = 2;  /* 地址长度一个字节   */
    masterXfer.data = &val;       /* 接收数据缓冲区     */
    masterXfer.data_len = 1;      /* 读取数据长度1个字节 */

    i2c_master_transfer(I2C2, &masterXfer);

    return val;
}

/*
 * @description    : 从 GT9147 读取多个字节的数据
 * @param - addr   : 设备地址
 * @param - reg    : 要读取的开始寄存器地址
 * @param - len    : 要读取的数据长度.
 * @param - buf    : 读取到的数据缓冲区
 * @return         : 无
 */
void gt9147_read_len(unsigned char addr,unsigned int reg,unsigned int len,unsigned char *buf)
{
    i2c_transfer masterXfer;

    masterXfer.slave_addr = addr; /* 设备地址 */
    masterXfer.dir = I2C_READ;    /* 读取数据 */
    masterXfer.reg_addr = reg;    /* 要读取的寄存器地址 */
    masterXfer.reg_addr_len = 2;  /* 地址长度一个字节   */
    masterXfer.data = buf;        /* 接收数据缓冲区     */
    masterXfer.data_len = len;    /* 读取数据长度1个字节 */

    i2c_master_transfer(I2C2, &masterXfer);
}

/*
 * @description    : 向 GT9147 多个寄存器写入数据
 * @param - addr   : 设备地址
 * @param - reg    : 要写入的开始寄存器地址
 * @param - len    : 要写入的数据长度.
 * @param - buf    : 写入到的数据缓冲区
 * @return         : 无
 */
void gt9147_write_len(unsigned char addr,unsigned int reg,unsigned int len, unsigned char *buf)
{
    struct i2c_transfer masterXfer;

    masterXfer.slave_addr = addr; /* 设备地址         */
    masterXfer.dir = I2C_WRITE;   /* 读取数据         */
    masterXfer.reg_addr = reg;    /* 要读取的寄存器地址 */
    masterXfer.reg_addr_len = 2;  /* 地址长度一个字节   */
    masterXfer.data = buf;        /* 接收数据缓冲区       */
    masterXfer.data_len = len;    /* 读取数据长度 1 个字节 */

    i2c_master_transfer(I2C2, &masterXfer);
}

/*
 * @description    : 发送 GT9147 配置参数
 * @param - mode   : 0, 参数不保存到 flash
 *                   1, 参数保存到 flash
 * @return         : 无
 */
void gt9147_send_cfg(unsigned char mode)
{
    unsigned char buf[2];
    unsigned int i = 0;

    buf[0] = 0;
    buf[1] = mode;    /* 是否写入到 GT9147 FLASH? 即是否掉电保存 */

    for (i = 0; i < (sizeof(GT9147_CFG_TBL)); i++) /* 计算校验和 */
        buf[0] += GT9147_CFG_TBL[i];

    buf[0] = (~buf[0]) + 1;

    /* 发送寄存器配置 */
    gt9147_write_len(GT9147_ADDR, GT_CFGS_REG, sizeof(GT9147_CFG_TBL), GT9147_CFG_TBL);
    gt9147_write_len(GT9147_ADDR, GT_CHECK_REG, 2, buf); /* 写入校验和,配置更新标记 */
} 

const u16 GT9147_TPX_TBL[5] = {GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
/*
 * @description    : 读取当前所有触摸点的坐标
 * @param          : 无
 * @return         : 无
 */
void gt9147_read_tpcoord(void)
{
    u8 buf[4];
    u8 i = 0;
    u8 regvalue = 0;

    regvalue = gt9147_read_byte(GT9147_ADDR, GT_GSTID_REG);
    gt9147_write_byte(GT9147_ADDR, GT_GSTID_REG, 0x00);
    gt9147_dev.point_num = regvalue & 0xF; /* 计算读取了多少个点 */

    /* 读取当前所有的触摸坐标值 */
    for (i = 0; i < gt9147_dev.point_num; i++) {
        gt9147_read_len(GT9147_ADDR, GT9147_TPX_TBL[i], 4, buf); /* 读取坐标值 */

        gt9147_dev.x[i] = ((u16)buf[1] << 8) + buf[0];
        gt9147_dev.y[i] = (((u16)buf[3] << 8) + buf[2]);
    }
}
