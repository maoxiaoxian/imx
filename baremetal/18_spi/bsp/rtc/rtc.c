#include "rtc.h"
#include "stdio.h"

/* 初始化 RTC */
void rtc_init(void)
{
    /*
     * 配置 SNVS_HPCOMR 寄存器，相关位域的设置如下：
     * NPSWA_EN[31]： 1，允许访问所有寄存器；
     */
    SNVS->HPCOMR |= (1 << 31);

    rtc_enable(); // 使能 RTC
}

/* 使能 RTC */
void rtc_enable(void)
{
    SNVS->LPCR |= (1 << 0); // 使能 RTC
    while (!(SNVS->LPCR & 0x1)) {} // 等待使能完成
}

/* 禁用 RTC */
void rtc_disable(void)
{
    SNVS->LPCR &= ~(1 << 0); // 禁用 RTC
    while (SNVS->LPCR & 0x1) {} // 等待禁用完成
}

/* 判断指定年份是否为闰年 */
uint8_t rtc_is_leap_year(uint16_t year)
{
    uint8_t is_leap_year = 0;

    if (year % 400 == 0) {
        is_leap_year = 1;
    } else {
        if ((year % 4 == 0) && (year % 100 != 0)) {
            is_leap_year = 1;
        } else {
            is_leap_year = 0;
        }
    }

    return is_leap_year;
}

/* 将时间转换为秒 */
uint32_t rtc_data_to_second(rtc_date_time *time)
{
    uint32_t i = 0;
    uint32_t seconds = 0;
    uint32_t days = 0;
    uint16_t monthdays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};

    for (i = 1970; i < time->year; i++) {
        days += DAYS_IN_A_YEAR; /* 平年，每年 365 天 */
        if (rtc_is_leap_year(i)) {
            days += 1; /* 闰年多加一天 */
        }
    }

    days += monthdays[time->month];
    if (rtc_is_leap_year(i) && (time->month >= 3)) {
        days += 1; /* 闰年，并且当前月份大于等于 3 月时加一天 */
    }

    days += time->day - 1;

    seconds = days * SECONDS_IN_A_DAY +
                time->hour * SECONDS_IN_A_HOUR +
                time->minute * SECONDS_IN_A_MINUTE +
                time->second;

    return seconds;
}

/* 设置时间和日期 */
void rtc_set_date_time(rtc_date_time *datetime)
{
    uint32_t seconds = 0;
    uint32_t tmp = SNVS->LPCR;

    rtc_disable(); /* 设置寄存器 HPRTCMR 和 HPRTCLR 时一定要先关闭 RTC */

    /* 将时间转换为秒 */
    seconds = rtc_data_to_second(datetime);

    SNVS->LPSRTCMR = (uint32_t)(seconds >> 17); /* 设置高 15 位 */
    SNVS->LPSRTCLR = (uint32_t)(seconds << 15); /* 设置低 17 位 */

    /* 如果此前 RTC 是打开的，在设置完 RTC 时间后需要重新打开 RTC */
    if (tmp & 0x1) {
        rtc_enable();
    }
}

/* 将秒数转换为时间 */
void rtc_second_to_date(uint64_t seconds, rtc_date_time *datetime)
{
    uint64_t x;
    uint64_t secondsRemaining, days;
    unsigned short daysInYear;

    /* 每个月的天数 */
    unsigned char daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    secondsRemaining = seconds; /* 剩余秒数初始化 */
    days = secondsRemaining / SECONDS_IN_A_DAY + 1; /* 根据秒数计算天数,加 1 是当前天数 */
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY; /* 计算天数以后剩余的秒数 */

    /* 计算时、分、秒 */
    datetime->hour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->minute = secondsRemaining / 60;
    datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* 计算年 */
    daysInYear = DAYS_IN_A_YEAR;
    datetime->year = YEAR_RANGE_START;
    while (days > daysInYear) {
        /* 根据天数计算年 */
        days -= daysInYear;
        datetime->year++;

        /* 处理闰年 */
        if (!rtc_is_leap_year(datetime->year))
            daysInYear = DAYS_IN_A_YEAR;
        else /* 闰年，天数加一 */
            daysInYear = DAYS_IN_A_YEAR + 1;
    }

    /* 根据剩余的天数计算月份 */
    if (rtc_is_leap_year(datetime->year)) /* 如果是闰年的话2月加一天 */
        daysPerMonth[2] = 29;

    for (x = 1; x <= 12; x++) {
        if (days <= daysPerMonth[x]) {
            datetime->month = x;
            break;
        } else {
            days -= daysPerMonth[x];
        }
    }

    datetime->day = days;
}

/* 获取 RTC 当前秒数 */
uint32_t rtc_get_seconds(void)
{
    unsigned int seconds = 0;

    seconds = (SNVS->LPSRTCMR << 17) | (SNVS->LPSRTCLR >> 15);

    return seconds;
}

/* 获取当前时间 */
void rtc_get_datetime(rtc_date_time *datetime)
{
    uint64_t seconds;

    seconds = rtc_get_seconds();

    rtc_second_to_date(seconds, datetime);
}
