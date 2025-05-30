#include "mpu9250.h"
#include "i2c.h"
#include "delay.h"
#include "stdio.h"

/* 初始化 MPU9250 */
unsigned char mpu9250_init(void)
{
    unsigned char data = 0;

    /*
     * IO 初始化 
     * I2C1_SCL -> UART4_TXD
     * I2C1_SDA -> UART4_RXD
     */
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);

    /*
     * 配置引脚电气特性 IOMUXC_SW_PAD_CTL_PAD_xxx：
     * HYS[16: 0，HYS 关闭
     * PUS[15:14]: 1，默认 47K 上拉
     * PUE[13]: 1，pull 功能
     * PKE[12]: 1，pull/keeper使能
     * ODE[11]: 0，关闭开路输出
     * SPEED[7:6]: 10，速度 100Mhz
     * DSE[5:3]: 110，驱动能力为 R0/6
     * SRE[0]: 1，高转换率
     */
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0X70B0);

    i2c_init(I2C1); /* 初始化I2C1 */

    mpu_write_byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80); // 复位 MPU9250

    delay_ms(100);  // 延时 100ms

    mpu_write_byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00); // 唤醒 MPU9250

    mpu_set_gyro_range(3);  // 陀螺仪传感器，±2000dps
    mpu_set_accel_range(0); // 加速度传感器，±2g
    mpu_set_rate(50);       // 设置采样率 50Hz

    mpu_write_byte(MPU9250_ADDR,MPU_INT_EN_REG, 0x00);    // 关闭所有中断
    mpu_write_byte(MPU9250_ADDR,MPU_USER_CTRL_REG, 0x00); // I2C 主模式关闭
    mpu_write_byte(MPU9250_ADDR,MPU_FIFO_EN_REG, 0x00);   // 关闭 FIFO
    mpu_write_byte(MPU9250_ADDR,MPU_INTBP_CFG_REG, 0x82); // INT 引脚低电平有效，开启 bypass 模式，可以直接读取磁力计

    data = mpu_read_byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);

    if(data == MPU6500_ID) {
        printf("MPU9250 ID=%#X\r\n", data);
        mpu_write_byte(MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0X01);  /* 设置 CLKSEL,PLL X 轴为参考 */
        mpu_write_byte(MPU9250_ADDR, MPU_PWR_MGMT2_REG, 0X00);  /* 加速度与陀螺仪都工作 */
        mpu_set_rate(50);                                       /* 设置采样率为 50Hz */
    } else {
        return 1;
    }

    data = mpu_read_byte(AK8963_ADDR, MAG_WIA); /* 读取 AK8963 ID */

    if(data == AK8963_ID) {
        printf("AK8963_ID=%#X\r\n", data);
        mpu_write_byte(AK8963_ADDR, MAG_CNTL1, 0X11); /* 设置 AK8963 为单次测量模式 */
    } else {
        return 2;
    }

    return 0;
}

/* 向 MPU9250 写入数据 */
unsigned char mpu_write_byte(unsigned char addr,unsigned char reg, unsigned char data)
{
    unsigned char status=0;
    unsigned char writedata=data;
    struct i2c_transfer masterXfer;

    /* 配置 I2C xfer结构体 */
    masterXfer.slave_addr = addr;  /* 设备地址 */
    masterXfer.dir = I2C_WRITE;    /* 写入数据 */
    masterXfer.reg_addr = reg;     /* 要写入的寄存器地址 */
    masterXfer.addr_len = 1;       /* 地址长度一个字节 */
    masterXfer.buf = &writedata;   /* 要写入的数据 */
    masterXfer.size = 1;           /* 写入数据长度1个字节 */

    if(i2c_master_transfer(I2C1, &masterXfer))
        status=1;

    return status;
}

/* 从 MPU9250 读取一个字节的数据 */
unsigned char mpu_read_byte(unsigned char addr,unsigned char reg)
{
    unsigned char val=0;
    struct i2c_transfer masterXfer;

    masterXfer.slave_addr = addr;  /* 设备地址 */
    masterXfer.dir = I2C_READ;     /* 读取数据 */
    masterXfer.reg_addr = reg;     /* 要读取的寄存器地址 */
    masterXfer.addr_len = 1;       /* 地址长度一个字节 */
    masterXfer.buf = &val;         /* 接收数据缓冲区 */
    masterXfer.size = 1;           /* 读取数据长度 1 个字节 */

    i2c_master_transfer(I2C1, &masterXfer);

    return val;
}

/* 从 MPU9250 读取多个字节的数据 */
void mpu_read_len(unsigned char addr,unsigned char reg,unsigned char len,unsigned char *buf)
{
    struct i2c_transfer masterXfer;

    masterXfer.slave_addr = addr; /* 设备地址 */
    masterXfer.dir = I2C_READ;    /* 读取数据 */
    masterXfer.reg_addr = reg;    /* 要读取的寄存器地址 */
    masterXfer.addr_len = 1;      /* 地址长度一个字节 */
    masterXfer.buf = buf;         /* 接收数据缓冲区 */
    masterXfer.size = len;        /* 读取数据长度 1 个字节 */

    i2c_master_transfer(I2C1, &masterXfer);
} 

/* 设置 MPU9250 陀螺仪量程 */
unsigned char mpu_set_gyro_range(unsigned char range)
{
    return mpu_write_byte(MPU9250_ADDR, MPU_GYRO_CFG_REG, range << 3); // 设置陀螺仪满量程范围
}

/* 设置 MPU9250 加速度量程 */
unsigned char mpu_set_accel_range(unsigned char range)
{
    return mpu_write_byte(MPU9250_ADDR, MPU_ACCEL_CFG_REG,range << 3); // 设置加速度传感器满量程范围
}

/* 设置 MPU9250 的数字低通滤波器 */
unsigned char mpu_set_lpf(unsigned short fre)
{
    unsigned char data = 0;

    if (fre >= 188) data = 1;
    else if (fre >= 98) data = 2;
    else if (fre >= 42) data = 3;
    else if (fre >= 20) data = 4;
    else if (fre >= 10) data = 5;
    else data = 6; 

    return mpu_write_byte(MPU9250_ADDR,MPU_CFG_REG,data);
}

/* 设置 MPU9250 的采样率 (假定 Fs = 1KHz) */
unsigned char mpu_set_rate(unsigned short rate)
{
    unsigned char data;

    if (rate > 1000) rate = 1000;
    if (rate < 4) rate = 4;

    data = 1000 / rate - 1;
    data = mpu_write_byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data); /* 设置数字低通滤波器 */

    return mpu_set_lpf(rate / 2); /* 自动设置 LPF 为采样率的一半 */
}

/* 得到陀螺仪值 (原始值) */
void mpu_get_gyroscope(short *gx,short *gy,short *gz)
{
    unsigned char buf[6];

    mpu_read_len(MPU9250_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
    *gx=((short)buf[0] << 8) | buf[1];
    *gy=((short)buf[2] << 8) | buf[3];
    *gz=((short)buf[4] << 8) | buf[5];
}

/* 读取加速度原始值 */
void mpu_get_accelerometer(short *ax,short *ay,short *az)
{
    unsigned char buf[6];

    mpu_read_len(MPU9250_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);

    *ax=((short)buf[0] << 8) | buf[1];
    *ay=((short)buf[2] << 8) | buf[3];
    *az=((short)buf[4] << 8) | buf[5];
}

/* 读取磁力计原始值 */
void mpu_get_magnetometer(short *mx, short *my, short *mz)
{
    unsigned char buf[6];

    mpu_read_len(AK8963_ADDR, MAG_XOUT_L , 6, buf);

    *mx = ((short)buf[1] << 8) | buf[0];
    *my = ((short)buf[3] << 8) | buf[2];
    *mz = ((short)buf[5] << 8) | buf[4];

    mpu_write_byte(AK8963_ADDR, MAG_CNTL1, 0X11); /* AK8963 每次读完以后都需要重新设置为单次测量模式 */
}
