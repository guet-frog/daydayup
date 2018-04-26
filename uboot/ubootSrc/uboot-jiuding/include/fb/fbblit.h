
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __FBBLIT_H__
#define __FBBLIT_H__

#include <fb/fb.h>


void common_bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy);

#endif /* __FBBLIT_H__ */
