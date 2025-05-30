#ifndef _LCD_API_H
#define _LCD_API_H

#include "nxp.h"
#include "lcd.h"

/* 函数声明 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r);

void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode);
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 size, char *p);

uint32_t lcd_pow(uint8_t m, uint8_t n);
void lcd_show_num(u16 x, u16 y, u32 num, u8 len, u8 size);
void lcd_show_xnum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode);

#endif /* _LCD_API_H */
