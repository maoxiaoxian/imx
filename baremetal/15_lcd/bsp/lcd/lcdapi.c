#include "font.h"
#include "lcdapi.h"

/* 绘制线条 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    u16 t;
    uint32_t xerr = 0, yerr = 0, delta_x, delta_y, distance;
    uint32_t incx, incy, uRow, uCol;

    delta_x = x2 - x1; /* 计算坐标增量 */
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0) /* 设置单步方向 */
        incx = 1;
    else if (delta_x == 0) /* 垂直线 */
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0) /* 水平线 */
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y) /* 选取基本增量坐标轴 */
        distance = delta_x;
    else
        distance = delta_y;

    for (t = 0; t <= distance + 1; t++) /* 画线输出 */
    {
        lcd_draw_point(uRow, uCol, tft_lcd.forecolor); /* 画点 */

        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/* 绘制矩形框 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_draw_line(x1, y1, x2, y1);
    lcd_draw_line(x1, y1, x1, y2);
    lcd_draw_line(x1, y2, x2, y2);
    lcd_draw_line(x2, y1, x2, y2);
}

/* 绘制圆形 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int32_t mx = x0, my = y0;
    int32_t x = 0, y = r;
    int32_t d = 1 - r;

    while (y > x) /* y > x 即第一象限的第 1 区八分圆 */
    {
        lcd_draw_point(x  + mx, y  + my, tft_lcd.forecolor);
        lcd_draw_point(y  + mx, x  + my, tft_lcd.forecolor);
        lcd_draw_point(-x + mx, y  + my, tft_lcd.forecolor);
        lcd_draw_point(-y + mx, x  + my, tft_lcd.forecolor);

        lcd_draw_point(-x + mx, -y + my, tft_lcd.forecolor);
        lcd_draw_point(-y + mx, -x + my, tft_lcd.forecolor);
        lcd_draw_point(x  + mx, -y + my, tft_lcd.forecolor);
        lcd_draw_point(y  + mx, -x + my, tft_lcd.forecolor);

        if ( d < 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            d = d + 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

/* 指定位置显示 1 个字符 */
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8  temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 计算字体一个字符对应点阵集所占的字节数 */

    num = num - ' '; /* 计算偏移后的值，ASCII 字库从空格开始取模，所以 -' ' 就是对应字符的字库 */

    for (t = 0; t < csize; t++)
    {
        if (size == 12) temp = asc2_1206[num][t];        /* 调用 1206 字体 */
        else if (size == 16)temp = asc2_1608[num][t];    /* 调用 1608 字体 */
        else if (size == 24)temp = asc2_2412[num][t];    /* 调用 2412 字体 */
        else if (size == 32)temp = asc2_3216[num][t];    /* 调用 3216 字体 */
        else return;                                     /* 未找到对应字库 */

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)lcd_draw_point(x, y, tft_lcd.forecolor);
            else if (mode==0)lcd_draw_point(x, y, tft_lcd.backcolor);

            temp <<= 1;

            y++;

            if (y >= tft_lcd.height) return; /* 超出区域 */

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= tft_lcd.width) return; /* 超出区域 */

                break;
            }
        }
    }
}

/* 计算 m 的 n 次方 */
uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--) result *= m;

    return result;
}

/* 显示指定的数字，高位为 0 不显示 */
void lcd_show_num(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            } else {
                enshow = 1;
            }
        }
         lcd_show_char(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}

/* 显示指定的数字，高位为 0 也显示 */
void lcd_show_xnum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / lcd_pow(10, len - t- 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80) lcd_show_char(x + (size / 2) * t, y, '0', size, mode & 0X01);
                else  lcd_show_char(x + (size / 2) * t, y , ' ', size, mode & 0X01);
                continue;
            } else {
                enshow=1;
            }
        }
         lcd_show_char( x + (size / 2) * t, y, temp + '0' , size , mode & 0X01);
    }
}

/* 显示字符串 */
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;

    width += x;
    height += y;

    while((*p <= '~') && (*p >= ' ')) /* 判断是不是非法字符! */
    {
        if (x >= width) {x = x0; y += size;}
        if (y >= height) break; /* 退出 */

        lcd_show_char(x, y, *p , size, 0);
        x += size / 2;
        p++;
    }
}
