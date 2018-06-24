
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __BITMAP_H__
#define __BITMAP_H__

/*
 * defined blit formats
 */
enum bitmap_format
{
	BITMAP_FORMAT_RGBA_GENERIC,
	BITMAP_FORMAT_RGB_GENERIC,

	BITMAP_FORMAT_RGBA_8888,
	BITMAP_FORMAT_BGRA_8888,

	BITMAP_FORMAT_RGB_888,
	BITMAP_FORMAT_BGR_888,

	BITMAP_FORMAT_RGB_565,
	BITMAP_FORMAT_BGR_565,

	BITMAP_FORMAT_MONOCHROME,
};

/*
 * the gimp's c source format picture
 */
struct picture
{
	/* width of the picture*/
	u32 width;

	/* height of the picture*/
	u32 height;

	/* bytes per pixel */
	u32 bytes_per_pixel;

	/* the pixel data */
	u8 * data;
};

struct bitmap_info
{
	/* width of the bitmap */
	u32 width;

	/* height of the bitmap */
	u32 height;

	/* bits per pixel */
	u32 bpp;

	/* bytes per pixel */
	u32 bytes_per_pixel;

	/* pitch of one scan line, how many bytes there are for scan line */
	u32 pitch;

	/* how many bits are reserved for red color */
	u32 red_mask_size;

	/* what is location of red color bits */
	u32 red_field_pos;

	/* how many bits are reserved for green color */
	u32 green_mask_size;

	/* what is location of green color bits */
	u32 green_field_pos;

	/* how many bits are reserved for blue color */
	u32 blue_mask_size;

	/* what is location of blue color bits */
	u32 blue_field_pos;

	/* how many bits are alpha in color */
	u32 alpha_mask_size;

	/* what is location of alpha color bits */
	u32 alpha_field_pos;

	/* optimization hint how binary data is coded */
	enum bitmap_format fmt;

	/* for monochrome bitmaps, the foreground and background color */
	u8 fg_r, fg_g, fg_b, fg_a;
	u8 bg_r, bg_g, bg_b, bg_a;
};

struct bitmap
{
	/* bitmap information description */
	struct bitmap_info info;

	/* bitmap's view port */
	struct {
		s32	left;
		s32	top;
		s32 	right;
		s32	bottom;
	} viewport;

	/* if the data in heap (malloc method), the it will be set */
	int allocated;

	/* pointer to bitmap data formatted according to info */
	void * data;
};



enum bitmap_format get_bitmap_format(struct bitmap_info * info);
int bitmap_create(struct bitmap ** bitmap, u32 width, u32 height, enum bitmap_format fmt);
int bitmap_load_from_picture(struct bitmap ** bitmap, struct picture * picture);
int bitmap_load_from_file(struct bitmap ** bitmap, const char * filename);
int bitmap_destroy(struct bitmap * bitmap);

#endif /* __BITMAP_H__ */
