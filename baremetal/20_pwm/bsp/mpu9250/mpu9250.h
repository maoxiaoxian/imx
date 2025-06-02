#ifndef _MPU9250_H
#define _MPU9250_H

#include "nxp.h"

#define MPU9250_ADDR              0x68    /* MPU6500 的器件 I2C 地址 */
#define MPU6500_ID                0x71    /* MPU6500 的器件 ID */

/* MPU9250 内部封装了一个 AK8963 磁力计，地址和 ID 如下: */
#define AK8963_ADDR               0x0C    /* AK8963 的 I2C 地址 */
#define AK8963_ID                 0x48    /* AK8963 的器件 ID */

/* MPU6500 的内部寄存器 */
#define MPU_SELF_TESTX_REG        0x0D    // 自检寄存器 X
#define MPU_SELF_TESTY_REG        0x0E    // 自检寄存器 Y
#define MPU_SELF_TESTZ_REG        0x0F    // 自检寄存器 Z
#define MPU_SELF_TESTA_REG        0x10    // 自检寄存器 A
#define MPU_SAMPLE_RATE_REG       0x19    // 采样频率分频器
#define MPU_CFG_REG               0x1A    // 配置寄存器
#define MPU_GYRO_CFG_REG          0x1B    // 陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG         0x1C    // 加速度计配置寄存器
#define MPU_MOTION_DET_REG        0x1F    // 运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG           0x23    // FIFO 使能寄存器
#define MPU_I2CMST_CTRL_REG       0x24    // IIC 主机控制寄存器
#define MPU_I2CSLV0_ADDR_REG      0x25    // IIC 从机 0 器件地址寄存器
#define MPU_I2CSLV0_REG           0x26    // IIC 从机 0 数据地址寄存器
#define MPU_I2CSLV0_CTRL_REG      0x27    // IIC 从机 0 控制寄存器
#define MPU_I2CSLV1_ADDR_REG      0x28    // IIC 从机 1 器件地址寄存器
#define MPU_I2CSLV1_REG           0x29    // IIC 从机 1 数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG      0x2A    // IIC 从机 1 控制寄存器
#define MPU_I2CSLV2_ADDR_REG      0x2B    // IIC 从机 2 器件地址寄存器
#define MPU_I2CSLV2_REG           0x2C    // IIC 从机 2 数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG      0x2D    // IIC 从机 2 控制寄存器
#define MPU_I2CSLV3_ADDR_REG      0x2E    // IIC 从机 3 器件地址寄存器
#define MPU_I2CSLV3_REG           0x2F    // IIC 从机 3 数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG      0x30    // IIC 从机 3 控制寄存器
#define MPU_I2CSLV4_ADDR_REG      0x31    // IIC 从机 4 器件地址寄存器
#define MPU_I2CSLV4_REG           0x32    // IIC 从机 4 数据地址寄存器
#define MPU_I2CSLV4_DO_REG        0x33    // IIC 从机 4 写数据寄存器
#define MPU_I2CSLV4_CTRL_REG      0x34    // IIC 从机 4 控制寄存器
#define MPU_I2CSLV4_DI_REG        0x35    // IIC 从机 4 读数据寄存器

#define MPU_I2CMST_STA_REG        0x36    // IIC 主机状态寄存器
#define MPU_INTBP_CFG_REG         0x37    // 中断/旁路设置寄存器
#define MPU_INT_EN_REG            0x38    // 中断使能寄存器
#define MPU_INT_STA_REG           0x3A    // 中断状态寄存器

#define MPU_ACCEL_XOUTH_REG       0x3B    // 加速度值，X 轴高 8 位寄存器
#define MPU_ACCEL_XOUTL_REG       0x3C    // 加速度值，X 轴低 8 位寄存器
#define MPU_ACCEL_YOUTH_REG       0x3D    // 加速度值，Y 轴高 8 位寄存器
#define MPU_ACCEL_YOUTL_REG       0x3E    // 加速度值，Y 轴低 8 位寄存器
#define MPU_ACCEL_ZOUTH_REG       0x3F    // 加速度值，Z 轴高 8 位寄存器
#define MPU_ACCEL_ZOUTL_REG       0x40    // 加速度值，Z 轴低 8 位寄存器

#define MPU_TEMP_OUTH_REG         0x41    // 温度值高 8 位寄存器
#define MPU_TEMP_OUTL_REG         0x42    // 温度值低 8 位寄存器

#define MPU_GYRO_XOUTH_REG        0x43    // 陀螺仪值，X 轴高 8 位寄存器
#define MPU_GYRO_XOUTL_REG        0x44    // 陀螺仪值，X 轴低 8 位寄存器
#define MPU_GYRO_YOUTH_REG        0x45    // 陀螺仪值，Y 轴高 8 位寄存器
#define MPU_GYRO_YOUTL_REG        0x46    // 陀螺仪值，Y 轴低 8 位寄存器
#define MPU_GYRO_ZOUTH_REG        0x47    // 陀螺仪值，Z 轴高 8 位寄存器
#define MPU_GYRO_ZOUTL_REG        0x48    // 陀螺仪值，Z 轴低 8 位寄存器

#define MPU_I2CSLV0_DO_REG        0x63    // IIC 从机 0 数据寄存器
#define MPU_I2CSLV1_DO_REG        0x64    // IIC 从机 1 数据寄存器
#define MPU_I2CSLV2_DO_REG        0x65    // IIC 从机 2 数据寄存器
#define MPU_I2CSLV3_DO_REG        0x66    // IIC 从机 3 数据寄存器

#define MPU_I2CMST_DELAY_REG      0x67    // IIC 主机延时管理寄存器
#define MPU_SIGPATH_RST_REG       0x68    // 信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG      0x69    // 运动检测控制寄存器
#define MPU_USER_CTRL_REG         0x6A    // 用户控制寄存器
#define MPU_PWR_MGMT1_REG         0x6B    // 电源管理寄存器 1
#define MPU_PWR_MGMT2_REG         0x6C    // 电源管理寄存器 2
#define MPU_FIFO_CNTH_REG         0x72    // FIFO 计数寄存器高 8 位
#define MPU_FIFO_CNTL_REG         0x73    // FIFO 计数寄存器低 8 位
#define MPU_FIFO_RW_REG           0x74    // FIFO 读写寄存器
#define MPU_DEVICE_ID_REG         0x75    // 器件 ID 寄存器

/* AK8963 的内部寄存器 */
#define MAG_WIA                   0x00    // AK8963 的器件 ID 寄存器地址
#define MAG_CNTL1                 0x0A
#define MAG_CNTL2                 0x0B

#define MAG_XOUT_L                0x03
#define MAG_XOUT_H                0x04
#define MAG_YOUT_L                0x05
#define MAG_YOUT_H                0x06
#define MAG_ZOUT_L                0x07
#define MAG_ZOUT_H                0x08


/* 函数声明 */
unsigned char mpu9250_init(void);
unsigned char mpu_write_byte(unsigned char addr,unsigned char reg, unsigned char data);
unsigned char mpu_read_byte(unsigned char addr,unsigned char reg);
void mpu_read_Len(unsigned char addr,unsigned char reg,unsigned char len,unsigned char *buf);
unsigned char mpu_set_gyro_range(unsigned char range);
unsigned char mpu_set_accel_range(unsigned char range);
unsigned char mpu_set_lpf(unsigned short fre);
unsigned char mpu_set_rate(unsigned short rate);
void mpu_get_gyroscope(short *gx,short *gy,short *gz);
void mpu_get_accelerometer(short *ax,short *ay,short *az);
void mpu_get_magnetometer(short *mx, short *my, short *mz);

#endif /* _MPU9250_H */
