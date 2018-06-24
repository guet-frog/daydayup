
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <malloc.h>
#include <fb/fb.h>
#include <fb/bitmap.h>
#include <fb/graphic.h>
#include <fb/fbcolor.h>

/*
 * map bitmap color
 */
u32 bitmap_map_color(struct bitmap * bitmap, u8 r, u8 g, u8 b, u8 a)
{
	struct bitmap_info * info = &(bitmap->info);
	u32 value;

	if(info->fmt == BITMAP_FORMAT_MONOCHROME)
	{
		if( (r == info->fg_r) && (g == info->fg_g) && (b == info->fg_b) && (a == info->fg_a) )
			return 1;
		else
			return 0;
	}
	else
	{
		r >>= 8 - info->red_mask_size;
		g >>= 8 - info->green_mask_size;
		b >>= 8 - info->blue_mask_size;
		a >>= 8 - info->alpha_mask_size;

		value = r << info->red_field_pos;
		value |= g << info->green_field_pos;
		value |= b << info->blue_field_pos;
		value |= a << info->alpha_field_pos;

		return value;
	}
}

/*
 * unmap bitmap color
 */
void bitmap_unmap_color(struct bitmap * bitmap, u32 c, u8 * r, u8 * g, u8 * b, u8 * a)
{
	struct bitmap_info * info = &(bitmap->info);
	u32 tmp;

	if(info->fmt == BITMAP_FORMAT_MONOCHROME)
	{
		if(c & 0x1)
		{
			*r = info->fg_r;
			*g = info->fg_g;
			*b = info->fg_b;
			*a = info->fg_a;
		}
		else
		{
			*r = info->bg_r;
			*g = info->bg_g;
			*b = info->bg_b;
			*a = info->bg_a;
		}
	}
	else
	{
		/* get red component */
		tmp = c >> info->red_field_pos;
		tmp &= (1 << info->red_mask_size) - 1;
		tmp <<= 8 - info->red_mask_size;
		tmp |= (1 << (8 - info->red_mask_size)) - 1;
		*r = tmp & 0xff;

		/* get green component */
		tmp = c >> info->green_field_pos;
		tmp &= (1 << info->green_mask_size) - 1;
		tmp <<= 8 - info->green_mask_size;
		tmp |= (1 << (8 - info->green_mask_size)) - 1;
		*g = tmp & 0xff;

		/* get blue component */
		tmp = c >> info->blue_field_pos;
		tmp &= (1 << info->blue_mask_size) - 1;
		tmp <<= 8 - info->blue_mask_size;
		tmp |= (1 << (8 - info->blue_mask_size)) - 1;
		*b = tmp & 0xff;

		/* get alpha component */
		if(info->alpha_mask_size > 0)
		{
			tmp = c >> info->alpha_field_pos;
			tmp &= (1 << info->alpha_mask_size) - 1;
			tmp <<= 8 - info->alpha_mask_size;
			tmp |= (1 << (8 - info->alpha_mask_size)) - 1;
		}
		else
		{
			tmp = 255;
		}
		*a = tmp & 0xff;
	}
}
