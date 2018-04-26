
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <malloc.h>
#include <fb/rect.h>
#include <fb/bitmap.h>
//#include <fb/font.h>
#include <fb/fb.h>
//#include <fb/fbfill.h>
//#include <fb/fbblit.h>
#include <fb/fbviewport.h>
#include <fb/graphic.h>


 //framebuffer set viewport
 
int fb_set_viewport(struct fb * fb, struct rect * rect)
{
	if(!fb)
		return 0;

	return bitmap_set_viewport(&(fb->info->bitmap), rect);
}


 //framebuffer get viewport
 
int fb_get_viewport(struct fb * fb, struct rect * rect)
{
	if(!fb)
		return 0;

	return bitmap_get_viewport(&(fb->info->bitmap), rect);
}


 //framebuffer map color
 
u32 fb_map_color(struct fb * fb, u8 r, u8 g, u8 b, u8 a)
{
	if(!fb || !fb->map_color)
		return 0;

	return fb->map_color(fb, r, g, b, a);
}


 //framebuffer unmap color
 
void fb_unmap_color(struct fb * fb, u32 c, u8 * r, u8 * g, u8 * b, u8 * a)
{
	if(!fb || !fb->unmap_color)
		return;

	return fb->unmap_color(fb, c, r, g, b, a);
}


 //framebuffer fill rect
 
int fb_fill_rect(struct fb * fb, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	if(!fb || !fb->fill_rect)
		return 0;

	return fb->fill_rect(fb, c, x, y, w, h);
}


 //framebuffer blit bitmap
 
int fb_blit_bitmap(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy)
{
	if(!fb || !fb->blit_bitmap)
		return 0;

	return fb->blit_bitmap(fb, bitmap, mode, x, y, w, h, ox, oy);
}


 //fill rect to bitmap
 
int bitmap_fill_rect(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	struct rect r, a, b;

	if(!bitmap)
		return 0;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = bitmap->viewport.left;
	b.top = bitmap->viewport.top;
	b.right = bitmap->viewport.right;
	b.bottom = bitmap->viewport.bottom;

	if(rect_intersect(&r, &a, & b) == 0)
		return 0;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_fill_rect(bitmap, c, x, y, w, h);

	return 1;
}


 //bitmap blitter
 
int bitmap_blit(struct bitmap * dst, struct bitmap * src, enum blit_mode mode, u32 x, u32 y, u32 w, u32 h, u32 ox, u32 oy)
{
	struct rect r, a, b;

	if(!dst || !src)
		return 0;

	a.left = x;
	a.top = y;
	a.right = x + w;
	a.bottom = y + h;

	b.left = dst->viewport.left;
	b.top = dst->viewport.top;
	b.right = dst->viewport.right;
	b.bottom = dst->viewport.bottom;

	if(rect_intersect(&r, &a, & b) == 0)
		return 0;

	x = r.left;
	y = r.top;
	w = r.right - r.left;
	h = r.bottom - r.top;

	common_bitmap_blit(dst, src, mode, x, y, w, h, ox, oy);

	return 1;
}
