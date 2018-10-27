
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include <fb/rect.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
//#include <fb/font.h>


//int fb_set_viewport(struct fb * fb, struct rect * rect);
int fb_get_viewport(struct fb * fb, struct rect * rect);
u32 fb_map_color(struct fb * fb, u8 r, u8 g, u8 b, u8 a);
void fb_unmap_color(struct fb * fb, u32 c, u8 * r, u8 * g, u8 * b, u8 * a);
int fb_fill_rect(struct fb * fb, u32 c, u32 x, u32 y, u32 w, u32 h);
/*int fb_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy);

int bitmap_fill_rect(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h);
int bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy);
*/
#endif /* __GRAPHIC_H__ */
