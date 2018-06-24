
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <malloc.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbpixel.h>
#include <fb/fbfill.h>

#define cpu_to_le32(x)	((u32)(x))
 //generic filler that works for every supported mode
 
static void bitmap_fill_rect_generic(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	u32 i, j;

	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			bitmap_set_pixel(bitmap, x + i, y + j, c);
		}
	}
}


 //optimized filler for direct color 32 bit modes
 
static void bitmap_fill_rect_direct32(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	u8 * p, * q;
	u32 * t;
	u32 len, skip;
	u32 i;

	c = cpu_to_le32(c);
	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u32 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8 *)t;

	for(i = 0; i < w; i++)
		*t++ = c;

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}


 //optimized filler for direct color 24 bit modes
 
static void bitmap_fill_rect_direct24(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	u8 * p, * q;
	u8 * t;
	u32 len, skip;
	u32 i;
	u8 fill0 = (u8)((c >> 0) & 0xff);
	u8 fill1 = (u8)((c >> 8) & 0xff);
	u8 fill2 = (u8)((c >> 16) & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u8 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8 *)t;

	for(i = 0; i < w; i++)
	{
		*t++ = fill0;
		*t++ = fill1;
		*t++ = fill2;
	}

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}


 //optimized filler for direct color 16 bit modes
 
static void bitmap_fill_rect_direct16(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	u8 * p, * q;
	u8 * t;
	u32 len, skip;
	u32 i;
	u8 fill0 = (u8)((c >> 0) & 0xff);
	u8 fill1 = (u8)((c >> 8) & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u8 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8 *)t;

	for(i = 0; i < w; i++)
	{
		*t++ = fill0;
		*t++ = fill1;
	}

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}


 //optimized filler for direct color 8 bit modes
 
static void bitmap_fill_rect_direct8(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	u8 * p, * q;
	u8 * t;
	u32 len, skip;
	u32 i;
	u8 fill = (u8)(c & 0xff);

	len = bitmap->info.bytes_per_pixel * w;
	skip = bitmap->info.pitch;
	t = (u8 *)bitmap_get_pointer(bitmap, x, y);
	p = q = (u8 *)t;

	for(i = 0; i < w; i++)
	{
		*t++ = fill;
	}

	for(i = 1; i < h; i++)
	{
		q += skip;
		memcpy(q, p, len);
	}
}


 //common file rect to bitmap
 
void common_bitmap_fill_rect(struct bitmap * bitmap, u32 c, u32 x, u32 y, u32 w, u32 h)
{
	switch(bitmap->info.bpp)
	{
	case 32:
		bitmap_fill_rect_direct32(bitmap, c, x, y, w, h);
		break;
	case 24:
		bitmap_fill_rect_direct24(bitmap, c, x, y, w, h);
		break;
	case 16:
		bitmap_fill_rect_direct16(bitmap, c, x, y, w, h);
		break;
	case 8:
		bitmap_fill_rect_direct8(bitmap, c, x, y, w, h);
		break;
	default:
		bitmap_fill_rect_generic(bitmap, c, x, y, w, h);
		break;
	}
}
