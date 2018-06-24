
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __FBPIXEL_H__
#define __FBPIXEL_H__

#include <fb/fb.h>

inline u8 * bitmap_get_pointer(struct bitmap * bitmap, u32 x, u32 y);
inline u32 bitmap_get_pixel(struct bitmap * bitmap, u32 x, u32 y);
inline void bitmap_set_pixel(struct bitmap * bitmap, u32 x, u32 y, u32 c);

#endif /* __FBPIXEL_H__ */
