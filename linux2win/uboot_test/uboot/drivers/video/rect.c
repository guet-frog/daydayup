
/***********************************************
编制：罗雄贵
日期：2012-2-13
说明：开机logo
***********************************************/

#include <common.h>
#include <stddef.h>
#include <fb/rect.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

void rect_set(struct rect * rect, s32 left, s32 top, s32 right, s32 bottom)
{
	rect->left = left;
	rect->top = top;
	rect->right = right;
	rect->bottom = bottom;
}
/*
void rect_set_empty(struct rect * rect)
{
	rect->left = 0;
	rect->top = 0;
	rect->right = 0;
	rect->bottom = 0;
}

void rect_copy(struct rect * dst, struct rect * src)
{
	dst->left = src->left;
	dst->top = src->top;
	dst->right = src->right;
	dst->bottom = src->bottom;
}

int rect_is_empty(struct rect * rect)
{
	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		return 1;
	}

	return 0;
}

int rect_is_equal(struct rect * rect1, struct rect * rect2)
{
	if( (rect1->left == rect2->left)
		&& (rect1->top == rect2->top)
		&& (rect1->right == rect2->right)
		&& (rect1->bottom == rect2->bottom) )
	{
		return 1;
	}

	return 0;
}
*/
int rect_intersect(struct rect * rect, struct rect * src1, struct rect * src2)
{
	rect->left = MAX(src1->left, src2->left);
	rect->top = MAX(src1->top, src2->top);
	rect->right = MIN(src1->right, src2->right);
	rect->bottom = MIN(src1->bottom, src2->bottom);

	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;

		return 0;
	}

	return 1;
}
/*
int rect_union(struct rect * rect, struct rect * src1, struct rect * src2)
{
	rect->left = MIN(src1->left, src2->left);
	rect->top = MIN(src1->top, src2->top);
	rect->right = MAX(src1->right, src2->right);
	rect->bottom = MAX(src1->bottom, src2->bottom);

	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;

		return 0;
	}

	return 1;
}

int rect_subtract(struct rect * rect, struct rect * src1, struct rect * src2)
{
	struct rect r;

	rect->left = 0;
	rect->top = 0;
	rect->right = 0;
	rect->bottom = 0;

	if(rect_intersect(&r, src1, src2))
	{
		if( (r.left == src1->left) && (r.top == src1->top) )
		{
			if(r.bottom == src1->bottom)
			{
				rect->left = r.right;
				rect->top = r.top;
				rect->right = src1->right;
				rect->bottom = r.bottom;
			}
			else if(r.right == src1->right)
			{
				rect->left = r.left;
				rect->top = r.bottom;
				rect->right = r.right;
				rect->bottom = src1->bottom;
			}
		}
		else if( (r.right == src1->right) && (r.bottom == src1->bottom) )
		{
			if(r.left == src1->left)
			{
				rect->left = r.left;
				rect->top =  src1->top;
				rect->right = r.right;
				rect->bottom = r.top;
			}
			else if(r.top == src1->top)
			{
				rect->left = src1->left;
				rect->top =  r.top;
				rect->right = r.left;
				rect->bottom = r.top;
			}
		}
	}

	if( (rect->right <= rect->left) || (rect->bottom <= rect->top) )
	{
		rect->left = 0;
		rect->top = 0;
		rect->right = 0;
		rect->bottom = 0;

		return 0;
	}

	return 1;
}

void rect_offset(struct rect * rect, s32 dx, s32 dy)
{
	rect->left += dx;
	rect->right += dx;
	rect->top += dy;
	rect->bottom += dy;
}

void rect_inflate(struct rect * rect, s32 dx, s32 dy)
{
	rect->left -= dx;
	rect->right += dx;
	rect->top -= dy;
	rect->bottom += dy;
}

int rect_have_point(struct rect * rect, s32 x, s32 y)
{
	if( (x >= rect->left) && (x < rect->right) && (y >= rect->top) && (y < rect->bottom) )
	{
		return 1;
	}

	return 0;
}
*/
int rect_align(struct rect * rect, struct rect * to, enum align flag)
{
	s32 ox1, oy1;
	s32 ox2, oy2;
	s32 dw, dh;

	ox1 = rect->left - to->left;
	oy1 = rect->top - to->top;
	ox2 = rect->right - to->right;
	oy2 = rect->bottom - to->bottom;
	dw = (rect->right - rect->left) - (to->right - to->left);
	dh = (rect->bottom - rect->top) - (to->bottom - to->top);

	switch(flag)
	{
	case ALIGN_LEFT:
		to->left += ox1;
		to->right += ox1;
		break;

	case ALIGN_TOP:
		to->top += oy1;
		to->bottom += oy1;
		break;

	case ALIGN_RIGHT:
		to->left += ox2;
		to->right += ox2;
		break;

	case ALIGN_BOTTOM:
		to->top += oy2;
		to->bottom += oy2;
		break;

	case ALIGN_LEFT_TOP:
		to->left += ox1;
		to->right += ox1;
		to->top += oy1;
		to->bottom += oy1;
		break;

	case ALIGN_RIGHT_TOP:
		to->left += ox2;
		to->right += ox2;
		to->top += oy1;
		to->bottom += oy1;
		break;

	case ALIGN_LEFT_BOTTOM:
		to->left += ox1;
		to->right += ox1;
		to->top += oy2;
		to->bottom += oy2;
		break;

	case ALIGN_RIGHT_BOTTOM:
		to->left += ox2;
		to->right += ox2;
		to->top += oy2;
		to->bottom += oy2;
		break;

	case ALIGN_LEFT_CENTER:
		to->left += ox1;
		to->right += ox1;
		to->top += oy1 + (dh >> 1);
		to->bottom += oy1 + (dh >> 1);
		break;

	case ALIGN_TOP_CENTER:
		to->left += ox1 + (dw >> 1);
		to->right += ox1 + (dw >> 1);
		to->top += oy1;
		to->bottom += oy1;
		break;

	case ALIGN_RIGHT_CENTER:
		to->left += ox2;
		to->right += ox2;
		to->top += oy1 + (dh >> 1);
		to->bottom += oy1 + (dh >> 1);
		break;

	case ALIGN_BOTTOM_CENTER:
		to->left += ox1 + (dw >> 1);
		to->right += ox1 + (dw >> 1);
		to->top += oy2;
		to->bottom += oy2;
		break;

	case ALIGN_CENTER_HORIZONTAL:
		to->left += ox1 + (dw >> 1);
		to->right += ox1 + (dw >> 1);
		break;

	case ALIGN_CENTER_VERTICAL:
		to->top += oy1 + (dh >> 1);
		to->bottom += oy1 + (dh >> 1);
		break;

	case ALIGN_CENTER:
		to->left += ox1 + (dw >> 1);
		to->right += ox1 + (dw >> 1);
		to->top += oy1 + (dh >> 1);
		to->bottom += oy1 + (dh >> 1);
		break;

	default:
		return 0;
	}

	return 1;
}
