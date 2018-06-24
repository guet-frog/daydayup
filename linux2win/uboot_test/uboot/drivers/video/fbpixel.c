
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <malloc.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/fbcolor.h>
#include <fb/fbfill.h>
#include <fb/fbblit.h>
#include <fb/graphic.h>
#include <fb/fbpixel.h>

/*
 * get bitmap's pointer at coordinate of x, y
 */
u8 * bitmap_get_pointer(struct bitmap * bitmap, u32 x, u32 y)
{
	u8 * p;

	switch(bitmap->info.bpp)
	{
	case 32:
		p = bitmap->data + y * bitmap->info.pitch + x * 4;
		break;

	case 24:
		p = bitmap->data + y * bitmap->info.pitch + x * 3;
		break;

	case 16:
		p = bitmap->data + y * bitmap->info.pitch + x * 2;
		break;

	case 8:
		p = bitmap->data + y * bitmap->info.pitch + x;
		break;

	case 1:
		return 0;

	default:
		return 0;
	}

	return p;
}

/*
 * get bitmap's pixel at coordinate of x, y
 */
inline u32 bitmap_get_pixel(struct bitmap * bitmap, u32 x, u32 y)
{
	u32 c;
	u8 * p;
	s32 pos;

	switch(bitmap->info.bpp)
	{
	case 32:
		p = bitmap->data + y * bitmap->info.pitch + x * 4;
		c = cpu_to_le32( *((u32 *)p) );
		break;

	case 24:
		p = bitmap->data + y * bitmap->info.pitch + x * 3;
		c = p[0] | (p[1] << 8) | (p[2] << 16);
		break;

	case 16:
		p = bitmap->data + y * bitmap->info.pitch + x * 2;
		c = cpu_to_le16( *((u16 *)p) );
		break;

	case 8:
		p = bitmap->data + y * bitmap->info.pitch + x;
		c = *((u8 *)p);
		break;

	case 1:
		p = bitmap->data + y * bitmap->info.pitch + (x / 8);
		pos = 7 - x % 8;
		c = (*p >> pos) & 0x01;
		break;

	default:
		return 0;
	}

	return c;
}

/*
 * set bitmap's pixel at coordinate of x, y
 */
inline void bitmap_set_pixel(struct bitmap * bitmap, u32 x, u32 y, u32 c)
{
	u8 * p;
	s32 pos;

	switch(bitmap->info.bpp)
	{
	case 32:
		p = bitmap->data + y * bitmap->info.pitch + x * 4;
		*((u32 *)p) = cpu_to_le32(c);
		break;

	case 24:
		p = bitmap->data + y * bitmap->info.pitch + x * 3;
		p[0] = c & 0xff;
		p[1] = (c >> 8) & 0xff;
		p[2] = (c >> 16) & 0xff;
		break;

	case 16:
		p = bitmap->data + y * bitmap->info.pitch + x * 2;
		*((u16 *)p) = cpu_to_le16(c & 0xffff);
		break;

	case 8:
		p = bitmap->data + y * bitmap->info.pitch + x;
		*((u8 *)p) = c & 0xff;
		break;

	case 1:
		p = bitmap->data + y * bitmap->info.pitch + (x / 8);
		pos = 7 - x % 8;
		*p = (*p & ~(1 << pos)) | ((c & 0x01) << pos);
		break;

	default:
		break;
	}
}
