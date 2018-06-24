
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <malloc.h>
#include <fb/bitmap.h>

/*
 * determine bitmap format
 */
enum bitmap_format get_bitmap_format(struct bitmap_info * info)
{
	/* check if we have any known 32 bit modes */
	if(info->bpp == 32)
	{
		if((info->red_mask_size == 8)
			&& (info->red_field_pos == 16)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 0))
		{
			return BITMAP_FORMAT_BGRA_8888;
		}
		else if((info->red_mask_size == 8)
			&& (info->red_field_pos == 0)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 16))
		{
			return BITMAP_FORMAT_RGBA_8888;
		}
	}

	/* check if we have any known 24 bit modes */
	else if(info->bpp == 24)
	{
		if((info->red_mask_size == 8)
			&& (info->red_field_pos == 16)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 0))
		{
			return BITMAP_FORMAT_BGR_888;
		}
		else if ((info->red_mask_size == 8)
			&& (info->red_field_pos == 0)
			&& (info->green_mask_size == 8)
			&& (info->green_field_pos == 8)
			&& (info->blue_mask_size == 8)
			&& (info->blue_field_pos == 16))
		{
			return BITMAP_FORMAT_RGB_888;
		}
	}

	/* check if we have any known 16 bit modes */
	else if(info->bpp == 16)
	{
		if ((info->red_mask_size == 5)
			&& (info->red_field_pos == 11)
			&& (info->green_mask_size == 6)
			&& (info->green_field_pos == 5)
			&& (info->blue_mask_size == 5)
			&& (info->blue_field_pos == 0))
		{
			return BITMAP_FORMAT_BGR_565;
		}
		else if ((info->red_mask_size == 5)
			&& (info->red_field_pos == 0)
			&& (info->green_mask_size == 6)
			&& (info->green_field_pos == 5)
			&& (info->blue_mask_size == 5)
			&& (info->blue_field_pos == 11))
		{
			return BITMAP_FORMAT_RGB_565;
		}
	}

	/* check if we have any known 1 bit modes */
	else if(info->bpp == 1)
	{
		return BITMAP_FORMAT_MONOCHROME;
	}

	if(info->alpha_mask_size > 0)
		return BITMAP_FORMAT_RGBA_GENERIC;
	else
		return BITMAP_FORMAT_RGB_GENERIC;
}


 //create new bitmap, saves created bitmap on success to * bitmap

 //just support BITMAP_FORMAT_RGBA_8888 and BITMAP_FORMAT_RGB_888
int bitmap_create(struct bitmap ** bitmap, u32 width, u32 height, enum bitmap_format fmt)
{
	struct bitmap_info * info;
	u32 size;

	if(!bitmap)
		return 0;

	*bitmap = NULL;

	if(width <= 0 || height <= 0)
		return 0;

	*bitmap = (struct bitmap *)malloc(sizeof(struct bitmap));
	if( !(*bitmap) )
		return 0;

	info = &((*bitmap)->info);

	info->width = width;
	info->height = height;
	info->fmt = fmt;

	switch(fmt)
	{
	case BITMAP_FORMAT_RGBA_8888:
		info->bpp = 32;
        info->bytes_per_pixel = 4;
        info->pitch = width * 4;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 8;
        info->alpha_field_pos = 24;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
		break;

	case BITMAP_FORMAT_RGB_888:
		info->bpp = 24;
        info->bytes_per_pixel = 3;
        info->pitch = width * 3;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 0;
        info->alpha_field_pos = 0;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
        break;

	default:
		free(*bitmap);
        *bitmap = NULL;

        return 0;
	}

	(*bitmap)->viewport.left = 0;
	(*bitmap)->viewport.top = 0;
	(*bitmap)->viewport.right = width;
	(*bitmap)->viewport.bottom = height;

	size = (width * info->bytes_per_pixel) * height;
	(*bitmap)->data = malloc(size);

	if(!(*bitmap)->data)
	{
		free(*bitmap);
        *bitmap = NULL;
        return 0;
    }
	(*bitmap)->allocated = 1;

	return 1;
}


 //load bitmap from memory picture (the gimp's c source format)
 
int bitmap_load_from_picture(struct bitmap ** bitmap, struct picture * picture)
{
	struct bitmap_info * info;

	if(!bitmap)
		return 0;
	*bitmap = NULL;

	if(!picture)
		return 0;

	if(picture->width <= 0 || picture->height <= 0)
		return 0;

	if( (picture->bytes_per_pixel != 3) && (picture->bytes_per_pixel != 4) )
		return 0;

	if(!picture->data)
		return 0;

	*bitmap = (struct bitmap *)malloc(sizeof(struct bitmap));
	if( !(*bitmap) )
		return 0;

	info = &((*bitmap)->info);
	info->width = picture->width;
	info->height = picture->height;

	if(picture->bytes_per_pixel == 3)
	{
		info->fmt = BITMAP_FORMAT_RGB_888;
		info->bpp = 24;
        info->bytes_per_pixel = 3;
        info->pitch = picture->width * 3;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 0;
        info->alpha_field_pos = 0;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
	}
	else if(picture->bytes_per_pixel == 4)
	{
		info->fmt = BITMAP_FORMAT_RGBA_8888;
		info->bpp = 32;
        info->bytes_per_pixel = 4;
        info->pitch = picture->width * 4;
        info->red_mask_size = 8;
        info->red_field_pos = 0;
        info->green_mask_size = 8;
        info->green_field_pos = 8;
        info->blue_mask_size = 8;
        info->blue_field_pos = 16;
        info->alpha_mask_size = 8;
        info->alpha_field_pos = 24;
        info->fg_r = info->fg_g = info->fg_b = info->fg_a = 0xff;
        info->bg_r = info->bg_g = info->bg_b = info->bg_a = 0x00;
	}
	else
	{
		free(*bitmap);
        *bitmap = NULL;

        return 0;
	}

	(*bitmap)->viewport.left = 0;
	(*bitmap)->viewport.top = 0;
	(*bitmap)->viewport.right = picture->width;
	(*bitmap)->viewport.bottom = picture->height;

	(*bitmap)->data = picture->data;
	(*bitmap)->allocated = 0;

	return 1;
}

/*
 * frees all resources allocated by bitmap
 */
int bitmap_destroy(struct bitmap * bitmap)
{
	if(!bitmap)
		return 0;

	if(bitmap->allocated)
		free(bitmap->data);
	free(bitmap);

	return 1;
}
