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
    uint16_t vbpd;
    uint16_t vfpd;

    // HSPW, HBF, HFP
    uint16_t hspw;
    uint16_t hbpd;
    uint16_t hfpd;

    uint32_t framebuffer; // 显存起始地址

    uint32_t forecolor; // 屏幕前景色
    uint32_t backcolor; // 屏幕背景色

    uint32_t id; // 屏幕 ID
} tft_lcd_t;

/* 函数定义 */
void lcd_init(void);

void lcd_reset(void);
void lcd_noreset(void);
void lcd_enable(void);
void lcd_disable(void);

void lcd_panel_id_gpio_init(void);
uint16_t lcd_read_panel_id(void);

void lcd_gpio_init(void);

#endif /* _LCD_H */
