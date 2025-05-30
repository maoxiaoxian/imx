#ifndef _LCD_H
#define _LCD_H

#include "nxp.h"

/* LCD 屏幕 ID 对应的值 */
#define ATK4342    0x4342 // 4.3 英寸，480*272 分辨率
#define ATK4384    0x4384 // 4.3 英寸，800*480 分辨率
#define ATK7084    0x7084 // 7 英寸，800*480 分辨率
#define ATK7016    0x7016 // 7 英寸，1024*600 分辨率
#define ATK1018    0x1018 // 10.1 英寸，1280*800 分辨率
#define ATKVGA     0xFF00 // VGA 接口
#define ATKUNKNOWN 0x0000

/* 像素点大小 (单位为字节) */
#define PIXEL_SIZE_ARGB8888 4

/* LCD 显存首地址 */
#define LCD_FRAMEBUF_ADDR (0x89000000)

/* 屏幕颜色定义 */
#define LCD_BLUE          0x000000FF
#define LCD_GREEN         0x0000FF00
#define LCD_RED           0x00FF0000
#define LCD_CYAN          0x0000FFFF
#define LCD_MAGENTA       0x00FF00FF
#define LCD_YELLOW        0x00FFFF00
#define LCD_LIGHTBLUE     0x008080FF
#define LCD_LIGHTGREEN    0x0080FF80
#define LCD_LIGHTRED      0x00FF8080
#define LCD_LIGHTCYAN     0x0080FFFF
#define LCD_LIGHTMAGENTA  0x00FF80FF
#define LCD_LIGHTYELLOW   0x00FFFF80
#define LCD_DARKBLUE      0x00000080
#define LCD_DARKGREEN     0x00008000
#define LCD_DARKRED       0x00800000
#define LCD_DARKCYAN      0x00008080
#define LCD_DARKMAGENTA   0x00800080
#define LCD_DARKYELLOW    0x00808000
#define LCD_WHITE         0x00FFFFFF
#define LCD_LIGHTGRAY     0x00D3D3D3
#define LCD_GRAY          0x00808080
#define LCD_DARKGRAY      0x00404040
#define LCD_BLACK         0x00000000
#define LCD_BROWN         0x00A52A2A
#define LCD_ORANGE        0x00FFA500
#define LCD_TRANSPARENT   0x00000000

/*
 * LCD 屏幕 ID
 * 三个模拟开关的值 Mx 与 Panel ID 的对应关系如下：
 *      M0: M1: M2 --> Panel ID
 *      0 : 0 : 0  --> 4.3 Inch,  480*272,  ID = 0x4342
 *      0 : 0 : 1  --> 7 Inch,    800*480,  ID = 0x7084
 *      0 : 1 : 0  --> 7 Inch,    1024*600, ID = 0x7016
 *      1 : 0 : 1  --> 10.1 Inch, 1280*800, ID = 0x1018
 *      1 : 0 : 0  --> 4.3 Inch,  800*480,  ID = 0x4384
 */
typedef enum panel_id_t {
    ID_ATK4342 = 0,
    ID_ATK7084 = 1,
    ID_ATK7016 = 2,
    ID_ATK4384 = 4,
    ID_ATK1018 = 5,
    ID_ATKVGA  = 7,
    ID_MAX,
} panel_id_t;

/* LCD 屏幕参数结构体 */
typedef struct tft_lcd_t{
    uint16_t height; // 屏幕高度
    uint16_t width;  // 屏幕宽度

    uint8_t pixel_size; // 每个像素占用大小

    // VSPW, VBP, VFP
    uint16_t vspw;
    uint16_t vbp;
    uint16_t vfp;

    // HSPW, HBF, HFP
    uint16_t hspw;
    uint16_t hbp;
    uint16_t hfp;

    uint32_t buf; // 显存起始地址

    uint32_t forecolor; // 屏幕前景色
    uint32_t backcolor; // 屏幕背景色

    uint16_t id; // 屏幕 ID
} tft_lcd_t;

/* 屏幕参数 */
extern tft_lcd_t tft_lcd;

/* 函数定义 */
void lcd_init(void);

void lcd_reset(void);
void lcd_noreset(void);
void lcd_enable(void);
void lcd_disable(void);

void lcd_panel_id_gpio_init(void);
uint16_t lcd_read_panel_id(void);

void lcd_gpio_init(void);

void lcd_clk_init(uint8_t loop_div, uint8_t pre_div, uint8_t post_div);

/* 画图 API */
inline void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);
inline uint32_t lcd_read_point(uint16_t x, uint16_t y);
void lcd_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
void lcd_clear(uint32_t color);

#endif /* _LCD_H */
