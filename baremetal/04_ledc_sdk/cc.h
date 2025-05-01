#ifndef _CC_H
#define _CC_H

// 定义库文件所需的数据类型
#define __I  volatile
#define __O  volatile
#define __IO volatile

typedef signed char          int8_t;
typedef signed short int     int16_t;
typedef signed int           int32_t;
typedef signed long long int int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char          s8;
typedef signed short int     s16;
typedef signed int           s32;
typedef signed long long int s64;

typedef unsigned char          u8;
typedef unsigned short int     u16;
typedef unsigned int           u32;
typedef unsigned long long int u64;

#endif /* _CC_H */
