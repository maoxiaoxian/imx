#ifndef _RTC_H
#define _RTC_H

#include "nxp.h"

/* 宏定义 */
#define SECONDS_IN_A_DAY    (86400) /* 一天 86400 秒 */
#define SECONDS_IN_A_HOUR   (3600)	/* 一个小时 3600 秒 */
#define SECONDS_IN_A_MINUTE (60)	/* 一分钟 60 秒 */
#define DAYS_IN_A_YEAR      (365)	/* 一年 365 天 */
#define YEAR_RANGE_START    (1970)	/* 开始年份 1970 年 */
#define YEAR_RANGE_END      (2099)	/* 结束年份 2099 年 */

/* 时间日期结构体 */
typedef struct rtc_date_time
{
    unsigned short year;  /* 范围为:1970 ~ 2099 */
    unsigned char month;  /* 范围为:1 ~ 12 */
    unsigned char day;    /* 范围为:1 ~ 31 (不同的月，天数不同) */
    unsigned char hour;   /* 范围为:0 ~ 23 */
    unsigned char minute; /* 范围为:0 ~ 59 */
    unsigned char second; /* 范围为:0 ~ 59 */
} rtc_date_time;

/* 函数声明 */

void rtc_init(void);

void rtc_enable(void);
void rtc_disable(void);

void rtc_set_date_time(rtc_date_time *datetime);

uint8_t rtc_is_leap_year(uint16_t year);
uint32_t rtc_data_to_second(rtc_date_time *time);
void rtc_second_to_date(uint64_t seconds, rtc_date_time *datetime);

uint32_t rtc_get_seconds(void);
void rtc_get_datetime(rtc_date_time *datetime);

#endif /* _RTC_H */
