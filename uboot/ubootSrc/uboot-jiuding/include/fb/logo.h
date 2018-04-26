
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __LOGO_H__
#define __LOGO_H__

#include <fb/fb.h>
#include <fb/bitmap.h>

int init_logo(void);
int display_logo(struct fb * fb);

#endif /* __LOGO_H__ */
