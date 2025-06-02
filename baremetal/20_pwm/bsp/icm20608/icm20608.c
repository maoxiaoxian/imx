#include "icm20608.h"
#include "delay.h"
#include "spi.h"
#include "stdio.h"

struct icm20608_dev_struc icm20608_dev; /* ICM20608 设备 */

/* 初始化 ICM20608，成功返回 0 */
uint8_t icm20608_init(void)
{
    uint8_t id;
    pin_cfg cs_cfg;

    /*
     * ECSPI3 的 IO 初始化
     * 
     * ECSPI 与 SoC 的引脚连接如下：
     *      ECSPI3_SCLK --> UART2_RXD
     *      ECSPI3_MISO --> UART2_RTS
     *      ECSPI3_MOSI --> UART2_CTS
     */
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0);
    IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI, 0);
    IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO, 0);

    /*
     * 配置 SPI 相关引脚 SCLK、MISO、MOSI 的 IO 电气属性
     * 配置 IOMUXC_SW_PAD_CTL_PAD_xxx 寄存器，相关位域配置如下：
     *      HYS[16]:    0b0,   禁用迟滞比较器
     *      PUS[15:14]: 0b00,  上拉/下拉电阻值，设置为 100K 下拉
     *      PUE[13]:    0b0,   引脚状态保持，设置为不保持引脚电平值
     *      PKE[12]:    0b1,   使能状态保持器
     *      ODE[11]:    0b0,   禁用开漏输出
     *      SPEED[7:6]: 0b10,  IO 速率设置为 100MHz
     *      DSE[5:3]:   0b110, IO 驱动能力设置为 R0/6
     *      SRE[0]:     0b0,   低压摆率
     */
    IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK, 0x10B1);
    IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI,   0x10B1);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO,   0x10B1);

    /* 片选引脚 CS/SS 初始化 */
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0);
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20, 0X10B0);
    cs_cfg.dir = GPIO_OUTPUT;
    cs_cfg.def_val = 0; /* 默认输出低电平 */
    gpio_init(GPIO1, 20, &cs_cfg);

    /* 初始化 ECSPI */
    spi_init(ECSPI3);

    /* ICM20608 复位，复位后为 0x40，睡眠模式 */
    icm20608_write_reg(ICM20_PWR_MGMT_1, 0x80);
    delay_ms(50);
    /* 关闭睡眠，自动选择时钟 */
    icm20608_write_reg(ICM20_PWR_MGMT_1, 0x01);
    delay_ms(50);

    /* 读取器件 ID */
    id = icm20608_read_reg(ICM20_WHO_AM_I);
    printf("ICM20608 ID = %#X\r\n", id);
    if(id != ICM20608G_ID && id != ICM20608D_ID) { // 检查 ID 是否正确
        return STATUS_ID_ERROR;
    }

    /* 配置 ICM20608 传感器 */
    icm20608_write_reg(ICM20_SMPLRT_DIV, 0x00);    /* 输出速率设置为内部采样率      */
    icm20608_write_reg(ICM20_GYRO_CONFIG, 0x18);   /* 陀螺仪的量程设置为 ±2000dps  */
    icm20608_write_reg(ICM20_ACCEL_CONFIG, 0x18);  /* 加速度计量程设置为 ±16G      */
    icm20608_write_reg(ICM20_CONFIG, 0x04);        /* 陀螺仪低通滤波 BW = 20Hz    */
    icm20608_write_reg(ICM20_ACCEL_CONFIG2, 0x04); /* 加速度计低通滤波 BW = 21.2Hz */
    icm20608_write_reg(ICM20_PWR_MGMT_2, 0x00);    /* 打开加速度计和陀螺仪的所有轴   */
    icm20608_write_reg(ICM20_LP_MODE_CFG, 0x00);   /* 关闭低功耗                  */
    icm20608_write_reg(ICM20_FIFO_EN, 0x00);       /* 关闭 FIFO                  */

    return STATUS_OK;
}

/*
 * @description   : 向 ICM20608 的指定寄存器写值
 * @param - reg   : 要写入的寄存器地址
 * @param - value : 要写入的值
 * @return        : 无
 */
void icm20608_write_reg(uint8_t reg, uint8_t val)
{
    /*
     * ICM20608 在使用 SPI 接口时，寄存器地址只有低 7 位有效
     * 寄存器地址的最高位是读/写标志位，读取操作为 1，写入操作为 0
     */
    reg &= ~0x80;

    ICM20608_CSN(0); /* 使能 SPI 传输 */

    spi_ch0_rw_byte(ECSPI3, reg); /* 发送寄存器地址 */
    spi_ch0_rw_byte(ECSPI3, val); /* 发送要写入的值 */

    ICM20608_CSN(1); /* 禁止 SPI 传输 */
}

/*
 * @description : 读取 ICM20608 指定寄存器中的值
 * @param - reg : 要读取的寄存器地址
 * @return      : 读取到的寄存器值
 */
uint8_t icm20608_read_reg(uint8_t reg)
{
    uint8_t val;

    /*
     * ICM20608 在使用 SPI 接口时，寄存器地址只有低 7 位有效
     * 寄存器地址的最高位是读/写标志位，读取操作为 1，写入操作为 0
     */
    reg |= 0x80;

    ICM20608_CSN(0); /* 使能 SPI 传输 */

    spi_ch0_rw_byte(ECSPI3, reg);        /* 发送寄存器地址 */
    val = spi_ch0_rw_byte(ECSPI3, 0xFF); /* 读取寄存器的值 */

    ICM20608_CSN(1); /* 禁止 SPI 传输 */

    return val; /* 返回读取到的寄存器值 */
}

/*
 * @description : 连续读取 ICM20608 的多个寄存器
 * @param - reg : 要读取的寄存器地址
 * @return      : 读取到的寄存器值
 */
void icm20608_read_len(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;

    /*
     * ICM20608 在使用 SPI 接口时，寄存器地址只有低 7 位有效
     * 寄存器地址的最高位是读/写标志位，读取操作为 1，写入操作为 0
     */
    reg |= 0x80;

    ICM20608_CSN(0); /* 使能 SPI 传输 */

    spi_ch0_rw_byte(ECSPI3, reg); /* 发送寄存器地址 */
    for(i = 0; i < len; i++) {    /* 顺序读取寄存器 */
        buf[i] = spi_ch0_rw_byte(ECSPI3, 0xFF);
    }

    ICM20608_CSN(1); /* 禁止 SPI 传输 */
}

/*
 * @description : 获取陀螺仪的分辨率
 * @param       : 无
 * @return      : 获取到的分辨率
 */
float icm20608_gyro_scaleget(void)
{
    uint8_t data;
    float gyroscale;

    data = (icm20608_read_reg(ICM20_GYRO_CONFIG) >> 3) & 0x3;
    switch(data)
    {
        case 0:
            gyroscale = 131;
            break;
        case 1:
            gyroscale = 65.5;
            break;
        case 2:
            gyroscale = 32.8;
            break;
        case 3:
            gyroscale = 16.4;
            break;
    }

    return gyroscale;
}

/*
 * @description : 获取加速度计的分辨率
 * @param       : 无
 * @return      : 获取到的分辨率
 */
uint16_t icm20608_accel_scaleget(void)
{
    uint8_t data;
    uint16_t accelscale;

    data = (icm20608_read_reg(ICM20_ACCEL_CONFIG) >> 3) & 0x3;
    switch(data)
    {
        case 0:
            accelscale = 16384;
            break;
        case 1:
            accelscale = 8192;
            break;
        case 2:
            accelscale = 4096;
            break;
        case 3:
            accelscale = 2048;
            break;
    }

    return accelscale;
}

/*
 * @description : 读取 ICM20608 的加速度、陀螺仪和温度的原始值
 * @param       : 无
 * @return      : 无
 */
void icm20608_get_data(void)
{
    float gyroscale;
    unsigned short accescale;
    unsigned char data[14];

    // printf("[icm20608_get_data]: Ready to Read Data!\r\n");

    icm20608_read_len(ICM20_ACCEL_XOUT_H, data, 14);

    gyroscale = icm20608_gyro_scaleget();
    accescale = icm20608_accel_scaleget();

    icm20608_dev.accel_x_adc = (unsigned short)((data[0] << 8) | data[1]);
    icm20608_dev.accel_y_adc = (unsigned short)((data[2] << 8) | data[3]);
    icm20608_dev.accel_z_adc = (unsigned short)((data[4] << 8) | data[5]);
    icm20608_dev.temp_adc    = (unsigned short)((data[6] << 8) | data[7]);
    icm20608_dev.gyro_x_adc  = (unsigned short)((data[8] << 8) | data[9]);
    icm20608_dev.gyro_y_adc  = (unsigned short)((data[10] << 8) | data[11]);
    icm20608_dev.gyro_z_adc  = (unsigned short)((data[12] << 8) | data[13]);

    /* 计算实际值 */
    icm20608_dev.gyro_x_act = ((float)(icm20608_dev.gyro_x_adc)  / gyroscale) * 100;
    icm20608_dev.gyro_y_act = ((float)(icm20608_dev.gyro_y_adc)  / gyroscale) * 100;
    icm20608_dev.gyro_z_act = ((float)(icm20608_dev.gyro_z_adc)  / gyroscale) * 100;

    icm20608_dev.accel_x_act = ((float)(icm20608_dev.accel_x_adc) / accescale) * 100;
    icm20608_dev.accel_y_act = ((float)(icm20608_dev.accel_y_adc) / accescale) * 100;
    icm20608_dev.accel_z_act = ((float)(icm20608_dev.accel_z_adc) / accescale) * 100;

    icm20608_dev.temp_act = (((float)(icm20608_dev.temp_adc) - 25 ) / 326.8 + 25) * 100;
}
