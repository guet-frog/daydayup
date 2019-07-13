#if	/* 12.1 字符设备驱动程序概念 */

	// 6′
	应用程序在调用open()，read()等时，实际通过执行swi指令(swi xxx)，这条汇编
	指令会引发一次异常，进入系统异常处理函数中(进入内核中)，在异常处理函数中
	通过判断xxx调用不同的处理函数。

	根据打开的文件类型(文件属性、主设备号)来调用相应的open、read函数。
#endif

#if /* 12.2.1 字符设备驱动led */

	定义file_operation如何通知内核 -- register_chrdev()
	register_chrdev()被谁调用(驱动的入口函数是哪一个) -- module_init()
	
	字符设备驱动通过数组来管理，major为下标
	
	自动创建通过 应用程序的udev机制(mdev)
	利用系统信息(注册驱动时会在/sys目录下生成设备信息)，mdev会根据系统信息自
	动创建设备节点

	dev_class = class_create();		// 创建类
	dev_device = class_device_create();		// 类下创建设备
	
	/etc/init.d/rcS
	echo /sbin/mdev > /proc/sys/kernel/hotplug
#endif

#if /* 12.2.2 字符设备驱动led */

	<on|off> linux中<>表示参数不可省略
	
	int var = 1;
	write(fd, &var, 4);
	
	if (1 == var)
	{
		// ...
	}
	
	1，首先完成框架 -- file_operation相关注册
	2，对硬件的操作 -- 与逻辑不同使用虚拟地址
	
	static int s3c24xx_leds_open()
	{
		int minor = MINOR(inode->i_rdev);		// MINOR(inode->i_cdev)
	}
	
	static int s3c24xx_leds_read()
	{
		int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
	}
#endif

#if /* 字符设备概念纠正之另一种写法 */

	2.4内核 字符设备驱动由MAJOR唯一标识，最多只能有255个
	register_chrdev() // (主, 0) ~ (主, 255)都对应一个xxx_fops;
	
	register_chrdev_region() // 主设备号 + 次设备号 共同标识一个设备
#endif

#if /* 字符设备概念纠正之RTC驱动分析 */

	date命令		// not data
	
	date无法查看时间：
	首先查看内核中是否有rtc相关驱动源码，为何没有执行？
	看驱动源码首先从入口开始

#endif


