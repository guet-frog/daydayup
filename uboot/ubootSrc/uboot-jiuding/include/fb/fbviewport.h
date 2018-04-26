
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __FBVIEWPORT_H__
#define __FBVIEWPORT_H__

#include <fb/rect.h>
#include <fb/fb.h>

//int bitmap_set_viewport(struct bitmap * bitmap, struct rect * rect);
int bitmap_get_viewport(struct bitmap * bitmap, struct rect * rect);

#endif /* __FBVIEWPORT_H__ */
