
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#ifndef __RECT_H__
#define __RECT_H__

/*
 * the structure of 2d point
 */
struct point
{
	s32 x;
	s32 y;
};

/*
 * the structure of 2d rect
 */
struct rect {
	s32	left;
	s32	top;
	s32 right;
	s32	bottom;
};

/*
 * defined align method
 */
enum align {
	ALIGN_LEFT,
	ALIGN_TOP,
	ALIGN_RIGHT,
	ALIGN_BOTTOM,
	ALIGN_LEFT_TOP,
	ALIGN_RIGHT_TOP,
	ALIGN_LEFT_BOTTOM,
	ALIGN_RIGHT_BOTTOM,
	ALIGN_LEFT_CENTER,
	ALIGN_TOP_CENTER,
	ALIGN_RIGHT_CENTER,
	ALIGN_BOTTOM_CENTER,
	ALIGN_CENTER_HORIZONTAL,
	ALIGN_CENTER_VERTICAL,
	ALIGN_CENTER,
};

void rect_set(struct rect * rect, s32 left, s32 top, s32 right, s32 bottom);
void rect_set_empty(struct rect * rect);
void rect_copy(struct rect * dst, struct rect * src);
int rect_is_empty(struct rect * rect);
int rect_is_equal(struct rect * rect1, struct rect * rect2);
int rect_intersect(struct rect * rect, struct rect * src1, struct rect * src2);
int rect_union(struct rect * rect, struct rect * src1, struct rect * src2);
int rect_subtract(struct rect * rect, struct rect * src1, struct rect * src2);
void rect_offset(struct rect * rect, s32 dx, s32 dy);
void rect_inflate(struct rect * rect, s32 dx, s32 dy);
int rect_have_point(struct rect * rect, s32 x, s32 y);
int rect_align(struct rect * rect, struct rect * to, enum align flag);

#endif /* __RECT_H__ */
