
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
#include <fb/fbviewport.h>


 //set bitmap viewport
 
int bitmap_set_viewport(struct bitmap * bitmap, struct rect * rect)
{
	struct rect r, x;

	if(!bitmap || !rect)
		return 0;

	x.left = 0;
	x.top = 0;
	x.right = bitmap->info.width;
	x.bottom = bitmap->info.height;

	if(rect_intersect(&r, &x, rect) == 0)
		return 0;

	bitmap->viewport.left = r.left;
	bitmap->viewport.top = r.top;
	bitmap->viewport.right = r.right;
	bitmap->viewport.bottom = r.bottom;

	return 1;
}


 //get bitmap viewport
 
int bitmap_get_viewport(struct bitmap * bitmap, struct rect * rect)
{
	if(!bitmap || !rect)
		return 0;

	rect->left = bitmap->viewport.left;
	rect->top = bitmap->viewport.top;
	rect->right = bitmap->viewport.right;
	rect->bottom = bitmap->viewport.bottom;

	return 1;
}
