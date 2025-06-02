#ifndef _AP3216C_H
#define _AP3216C_H

#include "nxp.h"

#define AP3216C_ADDR           0x1E    /* AP3216C 器件地址 */

/* AP3316C 寄存器 */
#define AP3216C_SYSTEMCONG     0x00    /* 配置寄存器 */
#define AP3216C_INTSTATUS      0x01    /* 中断状态寄存器 */
#define AP3216C_INTCLEAR       0x02    /* 中断清除寄存器 */
#define AP3216C_IRDATALOW      0x0A    /* IR 数据低字节 */
#define AP3216C_IRDATAHIGH     0x0B    /* IR 数据高字节 */
#define AP3216C_ALSDATALOW     0x0C    /* ALS 数据低字节 */
#define AP3216C_ALSDATAHIGH    0x0D    /* ALS 数据高字节 */
#define AP3216C_PSDATALOW      0x0E    /* PS 数据低字节 */
#define AP3216C_PSDATAHIGH     0x0F    /* PS 数据高字节 */

/* 函数声明 */
unsigned char ap3216c_init(void);
unsigned char ap3216c_read_byte(unsigned char addr, unsigned char reg);
unsigned char ap3216c_write_byte(unsigned char addr, unsigned char reg, unsigned char data);
void ap3216c_read_data(unsigned short *ir, unsigned short *ps, unsigned short *als);

#endif /* _AP3216C_H */
