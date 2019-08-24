
// devs.c -- line454
/* - for test video memory -- 2019-7-16 14:02:00 */
printk(KERN_INFO "npd->pmem_start = %#x, npd->pmem->pmem_size = %#x\n");


// mach-x210.c -- line255
//#define S5PV210_LCD_WIDTH 800
//#define S5PV210_LCD_HEIGHT 480
#define S5PV210_LCD_WIDTH 1024      /* - modify for new lcd 2019-7-16 14:49:15 */
#define S5PV210_LCD_HEIGHT 600


// Makefile -- line59
# add own button driver -- 2019-7-19 20:42:13
#obj-$(CONFIG_MACH_SMDKV210)	+= button-x210.o


// add touch screen src driver code
// gslX680.c gslX680.h gsl_point_id
