
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <malloc.h>
#include <fb/rect.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
//#include <fb/fbcolor.h>
//#include <fb/fbfill.h>
//#include <fb/fbblit.h>
//#include <fb/graphic.h>
//#include <fb/fbsoft.h>

/*
 * default soft map color function
 */
u32 fb_soft_map_color(struct fb * fb, u8 r, u8 g, u8 b, u8 a)
{
	return bitmap_map_color(&(fb->info->bitmap), r, g, b, a);
}

/*
 * default soft unmap color function
 */
void fb_soft_unmap_color(struct fb * fb, u32 c, u8 * r, u8 * g, u8 * b, u8 * a)
{
	bitmap_unmap_color(&(fb->info->bitmap), c, r, g, b, a);
}

/*
 * default soft fill rect function
 */
int fb_soft_fill_rect(struct fb * fb, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	struct bitmap * p = &(fb->info->bitmap);
	struct rect r, a, b;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = p->viewport.left;
	b.top = p->viewport.top;
	b.right = p->viewport.right;
	b.bottom = p->viewport.bottom;

	if(rect_intersect(&r, &a, & b) == 0)
		return 0;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;
	common_bitmap_fill_rect(p, c, x, y, w, h);
	return 1;
}

/*
 * default soft blit bitmap function
 */
int fb_soft_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy)
{
	struct bitmap * p = &(fb->info->bitmap);
	struct rect r, a, b;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = p->viewport.left;
	b.top = p->viewport.top;
	b.right = p->viewport.right;
	b.bottom = p->viewport.bottom;

	if(rect_intersect(&r, &a, & b) == 0)
		return 0;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_blit(p, bitmap, mode, x, y, w, h, ox, oy);

	return 1;
}
