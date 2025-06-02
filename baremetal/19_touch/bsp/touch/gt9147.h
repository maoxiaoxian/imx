#ifndef _GT9147_H
#define _GT9147_H

#include "nxp.h"
#include "gpio.h"

/* GT9147 设备地址 */
#define GT9147_ADDR             0x14

/* GT9147 寄存器地址 */
#define GT_CTRL_REG             0x8040  /* GT9147 控制寄存器         */
#define GT_MODSW_REG            0x804D  /* GT9147 模式切换寄存器      */
#define GT_CFGS_REG             0x8047  /* GT9147 配置起始地址寄存器   */
#define GT1151_CFGS_REG         0x8050  /* GT1151 配置起始地址寄存器   */
#define GT_CHECK_REG            0x80FF  /* GT9147 校验和寄存器        */
#define GT_PID_REG              0x8140  /* GT9147 产品 ID 寄存器      */

#define GT_GSTID_REG            0x814E  /* GT9147 当前检测到的触摸情况 */
#define GT_TP1_REG              0x8150  /* 第一个触摸点数据地址        */
#define GT_TP2_REG              0x8158  /* 第二个触摸点数据地址        */
#define GT_TP3_REG              0x8160  /* 第三个触摸点数据地址        */
#define GT_TP4_REG              0x8168  /* 第四个触摸点数据地址        */
#define GT_TP5_REG              0x8170  /* 第五个触摸点数据地址        */

#define GT9147_XYCOORDREG_NUM    30       /* 触摸点坐标寄存器数量 */
#define GT9147_INIT_FINISHED     1        /* 触摸屏初始化完成     */
#define GT9147_INIT_NOTFINISHED  0        /* 触摸屏初始化未完成   */

/* 触摸屏结构体 */
typedef struct gt9147_dev_t
{
    unsigned char initfalg;     /* 触摸屏初始化状态   */
    unsigned char intflag;      /* 标记中断有没有发生 */
    unsigned char point_num;    /* 触摸点           */
    unsigned short x[5];        /* X轴坐标          */
    unsigned short y[5];        /* Y轴坐标          */
} gt9147_dev_t;

extern struct gt9147_dev_t gt9147_dev;
extern int gt_init_fail;

/* 函数声明 */
void gt9147_init(void);

void gt9147_irqhandler(void);

unsigned char gt9147_write_byte(unsigned char addr,unsigned int reg, unsigned char data);
unsigned char gt9147_read_byte(unsigned char addr,unsigned int reg);
void gt9147_read_len(unsigned char addr,unsigned int reg,unsigned int len,unsigned char *buf);
void gt9147_write_len(unsigned char addr,unsigned int reg,unsigned int len,unsigned char *buf);
void gt9147_read_tpnum(void);
void gt9147_read_tpcoord(void);
void gt9147_send_cfg(unsigned char mode);
void gt9147_read_tpcoord(void);

#endif /* _GT9147_H */
