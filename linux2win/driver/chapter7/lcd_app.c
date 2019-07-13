#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>

#define DEV_PATHNAME		"/dev/fb0"

struct fb_fix_screeninfo fix_info;
struct fb_var_screeninfo var_info;

#define RED		0x00ff0000
#define GREEN	0x0000ff00
#define BLUE	0x000000ff


void draw_back(struct fb_var_screeninfo *varInfo, unsigned int *vFb, unsigned int color)
{
	unsigned int i, j;
	
	for (i = 0; i < varInfo->yres; i++)
	{
		for (j = 0; j < varInfo->xres; j++)
		{
			*(vFb + i * (varInfo->xres) + j) = color;
		}
	}
}

int main(int argc, char **argv)
{
	int fd = -1, ret = -1;
	unsigned int *vFb = NULL;
	
	fd = open(DEV_PATHNAME, O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}
	
	ret = ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
	if (ret < 0)
	{
		perror("ioctl");
		exit(-1);
	}
	
	ret = ioctl(fd, FBIOGET_VSCREENINFO, &var_info);
	if (ret < 0)
	{
		perror("ioctl");
		exit(-1);
	}
	
	/* mmap frame buff */
	vFb = mmap(NULL, ((var_info.xres_virtual) * (var_info.yres_virtual) * (var_info.bits_per_pixel / 8)), 
				PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	printf("smem_start = %#x, smem_len = %#x\n", fix_info.smem_start, fix_info.smem_len);
	
	printf("xres = %d, yres = %d\n", var_info.xres, var_info.yres);
	printf("xres_virtual = %d, yres_virtual = %d\n", var_info.xres_virtual, var_info.yres_virtual);
	printf("height = %d, width = %d\n", var_info.height, var_info.width);
	
	draw_back(&var_info, vFb, RED);
	
	close(fd);
	
	return 0;
}