#ifndef _ICM20608_H
#define _ICM20608_H

#include "nxp.h"
#include "gpio.h"

/* SPI 片选信号 */
#define ICM20608_CSN(n) (n ? pin_write(GPIO1, 20, 1) : pin_write(GPIO1, 20, 0))

#define ICM20608G_ID 0xAF /* ID 值 */
#define ICM20608D_ID 0xAE /* ID 值 */

/* 状态标志 */
#define STATUS_OK 0
#define STATUS_ID_ERROR 1

/* ICM20608 寄存器
 * 复位后所有寄存器地址都为 0，除了以下两个寄存器：
 * Register 107 (0x6B) Power Management 1 = 0x40
 * Register 117 (0x75) WHO_AM_I           = 0xAF 或 0xAE
 */

/* 陀螺仪 & 加速度计的自测值 (出厂时设置，用于与用户的自检输出值进行比较) */
#define ICM20_SELF_TEST_X_GYRO  0x00
#define ICM20_SELF_TEST_Y_GYRO  0x01
#define ICM20_SELF_TEST_Z_GYRO  0x02
#define ICM20_SELF_TEST_X_ACCEL 0x0D
#define ICM20_SELF_TEST_Y_ACCEL 0x0E
#define ICM20_SELF_TEST_Z_ACCEL 0x0F

/* 陀螺仪静态偏移 */
#define ICM20_XG_OFFS_USRH 0x13
#define ICM20_XG_OFFS_USRL 0x14
#define ICM20_YG_OFFS_USRH 0x15
#define ICM20_YG_OFFS_USRL 0x16
#define ICM20_ZG_OFFS_USRH 0x17
#define ICM20_ZG_OFFS_USRL 0x18

#define ICM20_SMPLRT_DIV    0x19
#define ICM20_CONFIG        0x1A
#define ICM20_GYRO_CONFIG   0x1B
#define ICM20_ACCEL_CONFIG  0x1C
#define ICM20_ACCEL_CONFIG2 0x1D
#define ICM20_LP_MODE_CFG   0x1E
#define ICM20_ACCEL_WOM_THR 0x1F
#define ICM20_FIFO_EN       0x23
#define ICM20_FSYNC_INT     0x36
#define ICM20_INT_PIN_CFG   0x37
#define ICM20_INT_ENABLE    0x38
#define ICM20_INT_STATUS    0x3A

/* 加速度输出 */
#define ICM20_ACCEL_XOUT_H 0x3B
#define ICM20_ACCEL_XOUT_L 0x3C
#define ICM20_ACCEL_YOUT_H 0x3D
#define ICM20_ACCEL_YOUT_L 0x3E
#define ICM20_ACCEL_ZOUT_H 0x3F
#define ICM20_ACCEL_ZOUT_L 0x40

/* 温度输出 */
#define ICM20_TEMP_OUT_H 0x41
#define ICM20_TEMP_OUT_L 0x42

/* 陀螺仪输出 */
#define ICM20_GYRO_XOUT_H 0x43
#define ICM20_GYRO_XOUT_L 0x44
#define ICM20_GYRO_YOUT_H 0x45
#define ICM20_GYRO_YOUT_L 0x46
#define ICM20_GYRO_ZOUT_H 0x47
#define ICM20_GYRO_ZOUT_L 0x48

#define ICM20_SIGNAL_PATH_RESET 0x68
#define ICM20_ACCEL_INTEL_CTRL  0x69
#define ICM20_USER_CTRL         0x6A
#define ICM20_PWR_MGMT_1        0x6B
#define ICM20_PWR_MGMT_2        0x6C
#define ICM20_FIFO_COUNTH       0x72
#define ICM20_FIFO_COUNTL       0x73
#define ICM20_FIFO_R_W          0x74
#define ICM20_WHO_AM_I          0x75

/* 加速度静态偏移 */
#define ICM20_XA_OFFSET_H 0x77
#define ICM20_XA_OFFSET_L 0x78
#define ICM20_YA_OFFSET_H 0x7A
#define ICM20_YA_OFFSET_L 0x7B
#define ICM20_ZA_OFFSET_H 0x7D
#define ICM20_ZA_OFFSET_L 0x7E

/* ICM20608 数据结构体 */
struct icm20608_dev_struc
{
    signed int gyro_x_adc;  /* 陀螺仪 X 轴的原始值 */
    signed int gyro_y_adc;  /* 陀螺仪 Y 轴的原始值 */
    signed int gyro_z_adc;  /* 陀螺仪 Z 轴的原始值 */
    signed int accel_x_adc; /* 加速度计 X 轴原始值 */
    signed int accel_y_adc; /* 加速度计 Y 轴原始值 */
    signed int accel_z_adc; /* 加速度计 Z 轴原始值 */
    signed int temp_adc;    /* 温度原始值         */

    /* 计算得到的实际值，扩大 100 倍 */
    signed int gyro_x_act;  /* 陀螺仪 X 的轴实际值 */
    signed int gyro_y_act;  /* 陀螺仪 Y 的轴实际值 */
    signed int gyro_z_act;  /* 陀螺仪 Z 的轴实际值 */
    signed int accel_x_act; /* 加速度计 X 轴实际值 */
    signed int accel_y_act; /* 加速度计 Y 轴实际值 */
    signed int accel_z_act; /* 加速度计 Z 轴实际值 */
    signed int temp_act;    /* 温度实际值         */
};

struct icm20608_dev_struc icm20608_dev; /* ICM20608 设备 */

/* 函数声明 */
uint8_t icm20608_init(void);

void icm20608_write_reg(uint8_t reg, uint8_t val);
uint8_t icm20608_read_reg(uint8_t reg);

void icm20608_read_len(uint8_t reg, uint8_t *buf, uint8_t len);

void icm20608_get_data(void);
float icm20608_gyro_scaleget(void);
uint16_t icm20608_accel_scaleget(void);

#endif /* _ICM20608_H */
