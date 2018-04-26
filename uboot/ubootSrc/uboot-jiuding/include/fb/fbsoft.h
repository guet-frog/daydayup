
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __FBSOFT_H__
#define __FBSOFT_H__

#include <fb/fb.h>


u32 fb_soft_map_color(struct fb * fb, u8 r, u8 g, u8 b, u8 a);
void fb_soft_unmap_color(struct fb * fb, u32 c, u8 * r, u8 * g, u8 * b, u8 * a);
int fb_soft_fill_rect(struct fb * fb, u32 c, u32 x, u32 y, u32 w, u32 h);
int fb_soft_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy);

#endif /* __FBSOFT_H__ */
