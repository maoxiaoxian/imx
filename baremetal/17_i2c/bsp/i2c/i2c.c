#include "i2c.h"
#include "delay.h"
#include "stdio.h"

/* 初始化 IIC，时钟频率设置为 100KHz */
void i2c_init(I2C_Type *base)
{
    /* 配置 I2C */
    base->I2CR &= ~(1 << 7); /* 访问 I2C 的寄存器前，需要先关闭 I2C */

    /*
     * 时钟频率设置为 100KHz
     * I2C 的时钟源为：IPG_CLK_ROOT = 66MHz
     * 时钟频率 = PERCLK_ROOT/dividison (dividison 由 IFDR 寄存器设置)
     * 设置寄存器 IFDR，根据配置表挑选一个合适的分频值
     * 设置 I2C 的时钟频率为 100KHz，分频值 = 66000000/100000 = 660
     * 表中没有 660 这个值，只能选择近似值 640，即寄存器 IFDR 的 IC 位域设置为 0x15
     */
    base->IFDR = 0x15 << 0;

    /*
     * 设置寄存器 I2CR，开启 I2C
     * IEN[7]：设置为 1 使能 I2C，修改 I2CR 寄存器的其他位域前，必须将该位置 1
     */
    base->I2CR |= (1 << 7);
}

/*
 * @description          : 发送重新开始信号
 * @param - base         : 要使用的 IIC
 * @param - addrss       : 设备地址
 * @param - dir          : 方向
 * @return               : 0 为正常，其他值表示出错
 */
unsigned char i2c_master_repeated_start(I2C_Type *base, unsigned char addr, i2c_dir dir)
{
    /* I2C 忙且工作在从模式，直接返回 */
    if(base->I2SR & (1 << 5) && (((base->I2CR) & (1 << 5)) == 0)) {
        return I2C_STATUS_BUSY;
    }

    /*
     * 设置寄存器 I2CR
     * MTX[4]: 传输方向，设置为 1，表示发送 (主机 --> 从机)
     * RSTA[2]: 置 1 产生重新开始信号
     */
    base->I2CR |=  (1 << 4) | (1 << 2);

    /*
     * 设置寄存器 I2DR
     * DATA[7:0]: 要发送的数据，1~7 位写入从设备地址，bit0 为传输方向
     */
    base->I2DR = ((unsigned int)addr << 1) | ((dir == I2C_READ) ? I2C_READ : I2C_WRITE);

    return I2C_STATUS_OK;
}

/*
 * @description          : 发送开始信号
 * @param - base         : 要使用的 IIC
 * @param - addrss       : 设备地址
 * @param - dir          : 方向
 * @return               : 0 正常，其他值表示出错
 */
unsigned char i2c_master_start(I2C_Type *base, unsigned char addr, i2c_dir dir)
{
    if(base->I2SR & (1 << 5)) /* I2C 忙 */
        return I2C_STATUS_BUSY;

    /*
     * 设置寄存器 I2CR
     * MSTA[5]: 1，设置为主模式
     * MTX[4]:  1，发送 (主机 --> 从机)
     */
    base->I2CR |=  (1 << 5) | (1 << 4);

    /*
     * 设置寄存器 I2DR
     * DATA[7:0]: 要发送的数据，1~7 位写入从设备地址，bit0 为传输方向
     */ 
    base->I2DR = ((unsigned int)addr << 1) | ((dir == I2C_READ) ? I2C_READ : I2C_WRITE);

    return I2C_STATUS_OK;
}

/*
 * @description      : 检查并清除错误
 * @param - base     : 要使用的 IIC
 * @param - status   : 状态
 * @return           : 状态结果
 */
unsigned char i2c_check_and_clear_error(I2C_Type *base, unsigned int status)
{
    /* 检查是否发生仲裁丢失错误 */
    if (status & (1 << 4)) {
        base->I2SR &= ~(1 << 4); /* 清除仲裁丢失错误位 */

        base->I2CR &= ~(1 << 7); /* 先关闭 I2C */
        base->I2CR |= (1 << 7);  /* 重新打开 I2C */

        return I2C_STATUS_ARBITRATION_LOST;
    } else if (status & (1 << 0)) { /* 没有接收到从机的应答信号 */
        return I2C_STATUS_NAK;      /* 返回 NAK(No acknowledge) */
    }

    return I2C_STATUS_OK;
}

/*
 * @description        : 停止信号
 * @param - base       : 要使用的 IIC
 * @param              : 无
 * @return             : 状态结果
 */
unsigned char i2c_master_stop(I2C_Type *base)
{
    unsigned short timeout = 0xffff;

    /* 清除 I2CR 的 TXAK[3]、MTX[4]、MSTA[5] 三位 */
    base->I2CR &= ~((1 << 5) | (1 << 4) | (1 << 3));

    /* 等待总线空闲 */
    while ((base->I2SR & (1 << 5))) {
        timeout--;

        if(timeout == 0) /* 超时跳出 */
            return I2C_STATUS_TIMEOUT;
    }

    return I2C_STATUS_OK;
}

/*
 * @description        : 发送数据
 * @param - base       : 要使用的 IIC
 * @param - buf        : 要发送的数据
 * @param - size       : 要发送的数据大小
 * @param - flags      : 标志
 * @return             : 无
 */
void i2c_master_write(I2C_Type *base, const unsigned char *buf, unsigned int size)
{
    /* 等待传输完成 */
    while (!(base->I2SR & (1 << 7)));

    base->I2SR &= ~(1 << 1); /* 清除标志位 */
    base->I2CR |= 1 << 4;    /* 发送数据 */

    while (size--) {
        base->I2DR = *buf++; /* 将 buf 中的数据写入 I2DR 寄存器 */

        while (!(base->I2SR & (1 << 1))); /* 等待传输完成 */
        base->I2SR &= ~(1 << 1);          /* 清除标志位 */

        /* 检查 ACK */
        if (i2c_check_and_clear_error(base, base->I2SR))
            break;
    }

    base->I2SR &= ~(1 << 1);
    i2c_master_stop(base); /* 发送停止信号 */
}

/*
 * @description        : 读取数据
 * @param - base       : 要使用的 IIC
 * @param - buf        : 读取到数据
 * @param - size       : 要读取的数据大小
 * @return             : 无
 */
void i2c_master_read(I2C_Type *base, unsigned char *buf, unsigned int size)
{
    volatile uint8_t dummy = 0;

    dummy++; /* 防止编译报错 */
    
    /* 等待传输完成 */
    while (!(base->I2SR & (1 << 7)));

    base->I2SR &= ~(1 << 1); /* 清除中断标志位 */
    base->I2CR &= ~((1 << 4) | (1 << 3)); /* 接收数据 */
    
    /* 如果只接收一个字节数据，发送 NACK 信号 */
    if (size == 1)
        base->I2CR |= (1 << 3);

    dummy = base->I2DR; /* 假读 */

    while (size--) {
        while (!(base->I2SR & (1 << 1))); /* 等待传输完成 */
        base->I2SR &= ~(1 << 1); /* 清除标志位 */

        if (size == 0) {
            i2c_master_stop(base); /* 发送停止信号 */
        }

        if (size == 1) {
            base->I2CR |= (1 << 3);
        }

        *buf++ = base->I2DR;
    }
}

/*
 * @description    : I2C 数据传输，包括读和写
 * @param - base   : 要使用的 I2C
 * @param - xfer   : 传输结构体
 * @return         : 传输结果，0 表示成功，其他值表示失败
 */
unsigned char i2c_master_transfer(I2C_Type *base, i2c_transfer *xfer)
{
    unsigned char ret = 0;
    i2c_dir dir = xfer->dir;

    base->I2SR &= ~((1 << 1) | (1 << 4)); /* 清除标志位 */

    /* 等待传输完成 */
    while (!((base->I2SR >> 7) & 0X1)) {};

    /* 读取时，先发送寄存器地址，所以先将方向改为写入 */
    if ((xfer->reg_addr_len > 0) && (xfer->dir == I2C_READ))
    {
        dir = I2C_WRITE;
    }

    ret = i2c_master_start(base, xfer->slave_addr, dir); /* 发送开始信号 */
    if (ret) {
        return ret;
    }

    while (!(base->I2SR & (1 << 1))) {}; /* 等待传输完成 */

    ret = i2c_check_and_clear_error(base, base->I2SR); /* 检查是否出现传输错误 */
    if (ret) {
        i2c_master_stop(base); /* 发送出错，发送停止信号 */

        return ret;
    }

    /* 发送寄存器地址 */
    if (xfer->reg_addr_len) {
        do {
            base->I2SR &= ~(1 << 1); /* 清除标志位 */
            xfer->reg_addr_len--;    /* 地址长度减一 */

            base->I2DR = ((xfer->reg_addr) >> (8 * xfer->reg_addr_len)); // 向 I2DR 寄存器写入子地址

            while (!(base->I2SR & (1 << 1))); /* 等待传输完成 */

            /* 检查是否有错误发生 */
            ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret) {
                i2c_master_stop(base); /* 发送停止信号 */

                return ret;
            }
        } while ((xfer->reg_addr_len > 0) && (ret == I2C_STATUS_OK));

        if (xfer->dir == I2C_READ) { /* 读取数据 */
            base->I2SR &= ~(1 << 1); /* 清除中断挂起位 */
            i2c_master_repeated_start(base, xfer->slave_addr, I2C_READ); /* 发送重复开始信号和从机地址 */
            while (!(base->I2SR & (1 << 1))) {}; /* 等待传输完成 */

            /* 检查是否有错误发生 */
            ret = i2c_check_and_clear_error(base, base->I2SR);
            if (ret) {
                ret = I2C_STATUS_ADDR_NAK;
                i2c_master_stop(base); /* 发送停止信号 */

                return ret;
            }
        }
    }

    /* 发送数据 */
    if ((xfer->dir == I2C_WRITE) && (xfer->data_len > 0)) {
        i2c_master_write(base, xfer->data, xfer->data_len);
    }

    /* 读取数据 */
    if ((xfer->dir == I2C_READ) && (xfer->data_len > 0)) {
        i2c_master_read(base, xfer->data, xfer->data_len);
    }

    return I2C_STATUS_OK;
}
