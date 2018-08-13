#ifndef LCD_H_
#define LCD_H_

#include "types.h"

#define RED		0xFF0000
#define GREEN	0x00FF00
#define BLUE	0x0000FF

void lcd_init();
void backlight_ctl(u8 ctl);
void lcd_enable(u8 enable);
void draw_pixel(int x, int y, u32 color);
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, u32 color);
void draw_circle(int x, int y, int r, u32 color);

#endif