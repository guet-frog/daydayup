
5.7.1.1、什么是framebuffer			// fb
	(5) fb是一个典型的字符设备，而且创建了一个类/sys/class/graphics

5.7.1.2、framebuffer的使用
	(1) 设备文件 /dev/fb0
	(2) 获取设备信息 #include <linux/fb.h>	// ioctl(fd, FBIOGET_VSCREENINFO, &var_info);
	(3) mmap做映射
	(4) 填充framebuffer

5.7.5.1、驱动框架部分
	(1) drivers/video/fbmem.c		// 驱动框架
	(2) drivers/video/fbsys.c		// fb在/sys目录下的属性文件
	(3) drivers/video/modedb.c		// 显示模式管理
	(4) drivers/video/fb_notify.c

5.7.5.2、驱动部分
	(1) drivers/video/samsung/s3cfb.c		// 驱动主体
	(2) drivers/video/samsung/s3cfb_fimd6x.c	// lcd硬件操作函数
	(2) arch/arm/mach-s5pv210/mach-x210.c	// 提供platform_device
	(3) arch/arm/plat-s5p/devs.c	// 提供device相关信息

5.7.8.2、注册登记该fb设备
	(1) registered_fb[i] = fb_info;		// 数据的填充、传递、使用

5.7.9.1、s3cfb.c
	(2) static struct platform_driver s3cfb_driver = {};

5.7.9.2、s3c_device_fb
	(1) struct platform_device s3c_device_fb = {};

5.7.10.1、probe函数分析
	(1) struct s3c_platform_fb
	(2) struct s3cfb_global		// 用于s3cfb.c和s3cfb_fimd6x.c两个文件传递数据

5.7.11.1、struct s3cfb_lcd			//  lcd param

5.7.11.4、resource的处理
	(1) platform_device中提供resource结构体数组
	(2) probe中platform_get_resource取出resource并且按FLAG分头处理

// free fbdev
// line921 fbdev

5.7.12.1、lcd寄存器相关硬件操作
	(1) s3cfb_set_vsync_interrupt()
	(2) s3cfb_set_global_interrupt()
	(3) s3cfb_init_global()

5.7.12.3、向框架注册该fb设备
	(1) s3cfb_alloc_framebuffer()
	(2) s3cfb_register_framebuffer()

5.7.13.framebuffer驱动分析4
5.7.13.1、一些硬件操作
	(1) s3cfb_set_clock
	(2) s3cfb_set_window
	(3) s3cfb_display_on

5.7.13.2、驱动中处理中断
	(1) platform_get_irq()
	(2) request_irq()		// 内核管理记录所有的中断

5.7.13.3、logo显示
	(1) drivers/video/logo/xxx.ppm
	// 内核将ppm文件转化为struct logo{}变量值

	s3cfb_probe
		fb_prepare_logo
			fb_find_logo
		fb_show_logo
			fb_show_logo_line
				fb_do_show_logo
					info->fbops->fb_imageblit	// file_operation

5.7.14.应用层为何不能设置分辨率
5.7.14.1、问题描述
	(1) 第4节时试图在应用层设置分辨率失败了，原因何在？
	(2) 定位问题：肯定是驱动的事儿
	(3) 进一步驱动中定位：ioctl部分的事儿

5.7.14.2、fb的ioctl部分
	(1) fb是典型的字符设备驱动
	(2) ioctl分为2部分，在驱动框架部分和驱动部分各有一半

5.7.15.折腾内核的启动logo
5.7.15.1、让logo显示在屏幕中央
5.7.15.2、自定义内核启动logo

	// logo_x210_clut224.ppm
	
	// logo/Makefile
	
	// logo结构体 与 ppm的第二行

