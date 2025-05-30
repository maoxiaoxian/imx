#include "ap3216c.h"
#include "i2c.h"
#include "delay.h"
#include "cc.h"
#include "stdio.h"

/*
 * @description    : 初始化 AP3216C
 * @param          : 无
 * @return         : 0 表示成功，其他值表示错误
 */
unsigned char ap3216c_init(void)
{
    unsigned char data = 0;

    /*
     * IO初始化
     * I2C1_SCL -> UART4_TXD
     * I2C1_SDA -> UART4_RXD
     */
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);

    /*
     * HYS[16]:    0b0,   HYS 关闭
     * PUS[15:14]: 0b1,   默认 47K 上拉
     * PUE[13]:    0b1,   pull 功能
     * PKE[12]:    0b1,   pull/keeper 使能
     * ODE[11]:    0b0,   关闭开路输出
     * SPEED[7:6]: 0b10,  速度 100Mhz
     * DSE[5:3]:   0b110, 驱动能力为 R0/6
     * SRE[0]:     0b1,   高转换率
     */
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0x70B0);

    i2c_init(I2C1); /* 初始化 I2C1 */

    /* 初始化 AP3216C */
    ap3216c_write_byte(AP3216C_ADDR, AP3216C_SYSTEMCONG, 0x04); /* 复位 AP3216C */

    delay_ms(50); /* AP33216C 复位至少需要 10ms */

    ap3216c_write_byte(AP3216C_ADDR, AP3216C_SYSTEMCONG, 0x03); /* 开启 ALS、PS+IR */

    data = ap3216c_read_byte(AP3216C_ADDR, AP3216C_SYSTEMCONG); /* 读取刚刚写入的值 0x03 */
    if (data == 0x03)
        return 0; /* AP3216C 正常 */
    else
        return 1; /* AP3216C 失败 */
}

/*
 * @description    : 向 AP3216C 写入数据
 * @param - addr   : 设备地址
 * @param - reg    : 要写入的寄存器
 * @param - data   : 要写入的数据
 * @return         : 操作结果
 */
unsigned char ap3216c_write_byte(unsigned char addr,unsigned char reg, unsigned char data)
{
    unsigned char status=0;
    unsigned char writedata=data;
    i2c_transfer masterXfer;

    /* 配置 I2C xfer 结构体 */
    masterXfer.slave_addr = addr; /* 设备地址 */
    masterXfer.dir = I2C_WRITE;   /* 写入数据 */
    masterXfer.reg_addr = reg;    /* 要写入的寄存器地址 */
    masterXfer.reg_addr_len = 1;  /* 地址长度一个字节 */
    masterXfer.data = &writedata; /* 要写入的数据 */
    masterXfer.data_len = 1;      /* 写入数据长度 1 个字节 */

    if (i2c_master_transfer(I2C1, &masterXfer))
        status = 1;

    return status;
}

/*
 * @description    : 从 AP3216C 读取一个字节的数据
 * @param - addr   : 设备地址
 * @param - reg    : 要读取的寄存器
 * @return         : 读取到的数据
 */
unsigned char ap3216c_read_byte(unsigned char addr,unsigned char reg)
{
    unsigned char val=0;
    i2c_transfer masterXfer;

    masterXfer.slave_addr = addr; /* 设备地址 */
    masterXfer.dir = I2C_READ;    /* 读取数据 */
    masterXfer.reg_addr = reg;    /* 要读取的寄存器地址 */
    masterXfer.reg_addr_len = 1;  /* 地址长度一个字节 */
    masterXfer.data = &val;       /* 接收数据缓冲区 */
    masterXfer.data_len = 1;      /* 读取数据长度1个字节 */

    i2c_master_transfer(I2C1, &masterXfer);

    return val;
}

/*
 * @description    : 读取 AP3216C 的数据，读取原始数据，包括 ALS, PS 和 IR
 *                 : 注意！如果同时打开 ALS, IR+PS，两次数据读取的时间间隔要大于 112.5ms
 * @param - ir     : ir 数据
 * @param - ps     : ps 数据
 * @param - ps     : als 数据
 * @return         : 无
 */
void ap3216c_read_data(unsigned short *ir, unsigned short *ps, unsigned short *als)
{
    unsigned char buf[6];
    unsigned char i;

    /* 循环读取所有传感器数据 */
    for (i = 0; i < 6; i++) {
        buf[i] = ap3216c_read_byte(AP3216C_ADDR, AP3216C_IRDATALOW + i);
    }

    if (buf[0] & 0x80) /* IR_OF 位为 1 则数据无效 */
        *ir = 0;
    else /* 读取 IR 传感器的数据 */
        *ir = ((unsigned short)buf[1] << 2) | (buf[0] & 0x03);

    *als = ((unsigned short)buf[3] << 8) | buf[2]; /* 读取 ALS 传感器的数据 */

    if (buf[4] & 0x40) /* IR_OF 位为 1 则数据无效 */
        *ps = 0;
    else /* 读取 PS 传感器的数据 */
        *ps = ((unsigned short)(buf[5] & 0x3F) << 4) | (buf[4] & 0x0F);
}
