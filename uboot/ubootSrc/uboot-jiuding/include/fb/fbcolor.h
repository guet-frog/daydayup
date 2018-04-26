
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __FBCOLOR_H__
#define __FBCOLOR_H__

#include <fb/fb.h>


u32 bitmap_map_color(struct bitmap * bitmap, u8 r, u8 g, u8 b, u8 a);
void bitmap_unmap_color(struct bitmap * bitmap, u32 c, u8 * r, u8 * g, u8 * b, u8 * a);

#endif /* __FBCOLOR_H__ */
