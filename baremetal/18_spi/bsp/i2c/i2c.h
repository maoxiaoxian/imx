#ifndef _I2C_H
#define _I2C_H

#include "nxp.h"

/* I2C 状态标志 */
#define I2C_STATUS_OK               (0)
#define I2C_STATUS_BUSY             (1)
#define I2C_STATUS_IDLE             (2)
#define I2C_STATUS_NAK              (3)
#define I2C_STATUS_ARBITRATION_LOST (4)
#define I2C_STATUS_TIMEOUT          (5)
#define I2C_STATUS_ADDR_NAK         (6)

/* 数据传输方向 */
typedef enum i2c_dir
{
    I2C_WRITE, /* 主机向从机写入数据 */
    I2C_READ,  /* 主机从从机读取数据 */
} i2c_dir;

/* 数据传输所使用的结构体 */
typedef struct i2c_transfer
{
    unsigned char slave_addr;       /* 7-bit 长度的从机地址 */
    i2c_dir dir;                    /* 传输方向 */
    unsigned int reg_addr;          /* 寄存器地址 */
    unsigned char reg_addr_len;     /* 寄存器地址长度 */
    unsigned char *volatile data;   /* 数据缓冲区 */
    volatile unsigned int data_len; /* 数据缓冲区长度 */
} i2c_transfer;

/* 函数声明 */
void i2c_init(I2C_Type *base);

unsigned char i2c_master_start(I2C_Type *base, unsigned char addr, i2c_dir dir);
unsigned char i2c_master_repeated_start(I2C_Type *base, unsigned char addr,  i2c_dir dir);

unsigned char i2c_check_and_clear_error(I2C_Type *base, unsigned int status);

unsigned char i2c_master_stop(I2C_Type *base);

void i2c_master_write(I2C_Type *base, const unsigned char *buf, unsigned int size);
void i2c_master_read(I2C_Type *base, unsigned char *buf, unsigned int size);

unsigned char i2c_master_transfer(I2C_Type *base, struct i2c_transfer *xfer);

#endif /* _I2C_H */
