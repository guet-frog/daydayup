
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __FB_H__
#define __FB_H__

#include <fb/bitmap.h>

/*
 * define blit operator mode
 */
enum blit_mode
{
	BLIT_MODE_REPLACE,
	BLIT_MODE_BLEND
};

/*
 * defined the structure of framebuffer information.
 */
struct fb_info
{
	/* the framebuffer name. */
	const char * name;

	/* bitmap information description for framebuffer */
	struct bitmap bitmap;
};

/*
 * defined the structure of framebuffer.
 */
struct fb
{
	/* the framebuffer information */
	struct fb_info * info;

	/* initialize the framebuffer */
	void (*init)(struct fb * fb);

	/* clean up the framebuffer */
	void (*exit)(struct fb * fb);

	/* map color */
	u32 (*map_color)(struct fb * fb, u8 r, u8 g, u8 b, u8 a);

	/* unmap color */
	void (*unmap_color)(struct fb * fb, u32 c, u8 * r, u8 * g, u8 * b, u8 * a);

	/* fill rect */
	int (*fill_rect)(struct fb * fb, u32 c, u32 x, u32 y, u32 w, u32 h);

	/* blit bitmap */
	int (*blit_bitmap)(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy);

	/* ioctl framebuffer */
	int (*ioctl)(struct fb * fb, int cmd, void * arg);

	/* private data */
	void * priv;
};

#endif /* __FB_H__ */
