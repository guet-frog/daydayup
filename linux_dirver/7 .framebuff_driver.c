
5.7.1.1、什么是framebuffer
	(3) framebuffer帧缓冲（简称fb）是linux内核中虚拟出的一个设备
	(4) framebuffer向应用层提供一个统一标准接口的显示设备
	(5) 从驱动来看，fb是一个典型的字符设备，而且创建了一个类/sys/class/graphics

5.7.1.2、framebuffer的使用
	(1) 设备文件 /dev/fb0
	(2) 获取设备信息 #include <linux/fb.h>
	(3) mmap做映射
	(4) 填充framebuffer

5.7.2.framebuffer应用编程实践1
5.7.2.1、打开设备
5.7.2.2、获取设备信息
	(1) 不可变信息FSCREENINFO，使用ioctl的FBIOGET_FSCREENINFO名
	(2) 可变信息VSCREENINFO，使用ioctl的FBIOGET_VSCREENINFO名

5.7.3.framebuffer应用编程实践2
5.7.3.1、mmap做映射
	做完了mmap后fb在当前进程中就已经就绪了，随时可以去读写LCD显示器了

5.7.4.framebuffer应用编程实践3
5.7.4.1、设置分辨率
	(1) 实验失败，实验结果是只能修改虚拟分辨率，不能修改可视分辨率。原因要去驱动里找。
	(2) 正确的做法是在驱动中去修改参数，然后重新编译运行，才能解决

5.7.4.2、写字、画线、图片显示等

5.7.5.framebuffer驱动框架总览
5.7.5.1、驱动框架部分
	(1) drivers/video/fbmem.c。主要任务：1、创建graphics类、注册FB的字符设备
	驱动、提供register_framebuffer接口给具体framebuffer驱动编写着来注册fb设
	备的。本文件相对于fb来说，地位和作用和misc.c文件相对于杂散类设备来说一样的，
	结构和分析方法也是类似的。
	(2) drivers/video/fbsys.c。这个文件是处理fb在/sys目录下的一些属性文件的。
	(3) drivers/video/modedb.c。这个文件是管理显示模式（譬如VGA、720P等就是显示模式）的
	(4) drivers/video/fb_notify.c

5.7.5.2、驱动部分
	(1) drivers/video/samsung/s3cfb.c，驱动主体
	(2) drivers/video/samsung/s3cfb_fimd6x.c，里面有很多LCD硬件操作的函数
	(2) arch/arm/mach-s5pv210/mach-x210.c，负责提供platform_device的
	(3) arch/arm/plat-s5p/devs.c，为platform_device提供一些硬件描述信息的

5.7.5.3、如何分析
	(1) 经验
	(2) 分析menuconfig、Makefile、Kconfig等
	(3) 内核编译后检查编译结果中的.o文件

5.7.6.framebuffer驱动框架分析1
5.7.6.1、fbmem_init函数
	(1)#ifdef MODULE
	(2)fb_proc_fops和fb在proc文件系统中的表现
	(3)register_chrdev注册fb设备
	(4)class_create创建graphics类
	(5)fbmem_exit的对应

5.7.6.2、fb_fops
	(1)read/write/mmap/ioctl
	(2)registered_fb和num_registered_fb
	(3)struct fb_info

5.7.7.framebuffer驱动框架分析2
5.7.7.1、register_framebuffer
	(1)fb驱动框架开放给驱动编写着的注册接口
	(2)fb_check_foreignness
	(3)remove_conflicting_framebuffers
	(4)device_create
	(5)fb_init_device

	// dev_set_drvdata(); 	-- show() store()

5.7.7.2、fb在sysfs中的接口
	(1)device_attrs
	(2)dev_set_drvdata和dev_get_drvdata

5.7.8.framebuffer驱动框架分析3
5.7.8.1、fb的mode
	(1)什么是mode
	(2)fb_var_to_videomode
	(3)fb_add_videomode

5.7.8.2、注册登记该fb设备
	(1)registered_fb[i] = fb_info;
	(2)结合fb_read等函数中对fb_info的使用
	(3)关键点：数据如何封装、数据由谁准备由谁消费、数据如何传递

5.7.8.3、fb_notifier_call_chain

5.7.9.framebuffer驱动分析1
5.7.9.1、s3cfb.c
	(1)注意目录结构的组织
	(2)s3cfb_driver

5.7.9.2、s3c_device_fb
	(1)mach-x210.c中，被使用
	(2)devs.c中
	(3)resource的定义和作用

5.7.10.framebuffer驱动分析1
5.7.10.1、probe函数分析
	(1)struct s3c_platform_fb	这个结构体是fb的platform_data结构体，这个结构体变量就是platform设备的私有数据，这个数据在platform_device.device.platform_data中存储。在mach文件中去准备并填充这些数据，在probe函数中通过传参的platform_device指针取出来。
	(2)struct s3cfb_global		这个结构体主要作用是在驱动部分的2个文件（s3cfb.c和s3cfb_fimd6x.c）的函数中做数据传递用的。
	(3)struct resource
	(4)regulator

5.7.10.2、platform_data的传递过程
	(1)to_fb_plat
	(2)s3cfb_set_platdata
	(3)smdkc110_machine_init

5.7.11.framebuffer驱动分析2
5.7.11.1、struct s3cfb_lcd			//  lcd param
5.7.11.2、pdata->cfg_gpio
5.7.11.3、pdata->clk_on
5.7.11.4、resource的处理
	(1) platform_device中提供resource结构体数组
	(2) probe中platform_get_resource取出resource并且按FLAG分头处理

// free fbdev
// line921 fbdev

5.7.12.framebuffer驱动分析3		// s3cfb_fimd6x.c lcd寄存器相关操作
5.7.12.1、一些硬件操作
	(1) s3cfb_set_vsync_interrupt
	(2) s3cfb_set_global_interrupt

5.7.12.2、s3cfb_init_global

5.7.12.3、向框架注册该fb设备
	(1) s3cfb_alloc_framebuffer
	(2) s3cfb_register_framebuffer

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

5.7.13.4、backlight点亮

5.7.14.应用层为何不能设置分辨率
5.7.14.1、问题描述
	(1) 第4节时试图在应用层设置分辨率失败了，原因何在？
	(2) 定位问题：肯定是驱动的事儿
	(3) 进一步驱动中定位：ioctl部分的事儿

5.7.14.2、fb的ioctl部分
	(1) fb是典型的字符设备驱动
	(2) ioctl分为2部分，在驱动框架部分和驱动部分各有一半
	(3) 一路追踪找问题

5.7.15.折腾内核的启动logo
5.7.15.1、让logo显示在屏幕中央
5.7.15.2、自定义内核启动logo

