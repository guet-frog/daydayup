
5.3.1.1、新接口与老接口
	(1) 老接口：register_chrdev		// 设备号(只有主设备号，无次设备号)	fops
	(2) 新接口：register_chrdev_region/alloc_chrdev_region + cdev	// struct cdev

		// 注册设备号和驱动是分开的

5.3.1.2、cdev介绍
	(2) 相关函数：cdev_alloc()、cdev_init()、cdev_add()、cdev_del()

5.3.1.4、编程实践
	(1) register_chrdev_region(), cdev_init(), cdev_add(), unregister_chrdev_region()

// 200 0 1 2 3

// from MKDEV(200, 0)
// count 4

5.3.2.1、alloc_chrdev_region()自动分配设备号

5.3.2.2、得到分配的主设备号和次设备号	// MKDEV(), MAJOR(), MINOR()
	*/note: dev_t device_dec; unsigned int device_major;

5.3.2.3、中途出错的倒影式错误处理方法	// note: goto语句 资源释放

5.3.4.1、使用cdev_alloc

5.3.4.2、cdev_init的替代
	// cdev->owner = THIS_MODULE;
	// cdev->ops = &fops;

5.3.5.字符设备驱动注册代码分析1
5.3.5.1、老接口分析
 -- register_chrdev()
  |
  | -- __register_chrdev()
  |
  | -- __register_chrdev_region()
  |
  | -- cdev_alloc()
  |
  | -- cdev_add()

5.3.5.2、新接口分析
 -- register_chrdev_region()
  |
  | -- __register_chrdev_region()

 -- alloc_chrdev_region()
  |
  | -- __register_chrdev_region()

  // 视频6 17′ kzalloc()讲解
  
  kernel/drivers/base/map.c -- kobj_map()		// base目录下为基础架构

5.3.7.2、udev（嵌入式中用的是mdev）
	(2) 内核驱动和/*应用层udev*/之间有一套信息传输机制（netlink协议）
	(3) 应用层启用udev，内核驱动中使用相应接口，驱动注册和注销时信息会被
		传给udev，由udev在应用层进行设备文件的创建和删除
		// uevent -> udev

5.3.7.3、内核驱动设备类相关函数
	(1) class_create()
	(2) device_create()
	(3) device_destory()
	(4) class_destroy()

	// lrwxrwxrwx 1 0 0 0 Jan  1 13:43 led_device_test -> ../../devices/virtual/led_class_test/led_device_test

5.3.8.1、sys文件系统简介
	(1) sys文件系统的设计思想
	(2) 设备类的概念
	(3) /sys/class/xxx/中的文件的作用

5.3.10.静态映射表建立过程分析
	(1) 映射表PA和VA相关的宏定义
	(2) 映射表建立函数mach-smdkc110.c/smdkc110_map_io()

 -- smdkc110_map_io()
  |
  | -- s5p_init_io()
     |
	 | -- iotable_init(s5p_iodesc, ARRAY_SIZE(s5p_iodesc))

//	如何添加其他外设模块地址空间，虚拟地址的静态映射。 -- 手动扩充s5p_iodesc

(3) 开机时调用映射表建立函数
 -- setup_arch()
  |
  | -- paging_init()
     |
	 | -- devicemaps_init()
	    |
		| -- if (mdesc->map_io)
		   |
		   | -- mdesc->map_io();

5.3.12.2、内核提供的寄存器读写接口
	(1) writel()、readl()
	(2) iowrite32()、ioread32()
