
5.4.1.3、到底什么是驱动框架
	(1) 驱动框架：内核驱动维护者针对各个种类驱动编写典型的驱动实现，把不同厂
		家的同类硬件驱动中相同的部分抽出来实现，再把不同部分留出接口给具体的
		驱动开发工程师实现
	(2) 内核对系统资源进行统一管控(GPIO，中断号)，驱动需要先申请后释放

	// 分清楚内核开发者提供了什么，驱动开发者自己要提供什么

5.4.2.1、内核驱动框架中LED相关文件
	(1) drivers/leds/led-class.c、led-core.c

5.4.2.2、九鼎移植的内核中led驱动		// drivers/char/led/x210-led.c

5.4.3.2、subsys_initcall()		// linux/init.h
	(1) #define subsys_initcall(fn)		__define_initcall("4", fn, 4)
	(2) #define module_init(fn)			__define_initcall("6", fn, 6)

	// kernel启动时自动执行，不需要insmod。该模块只能设置为Y或N。不能设置成M
	// module_init需要手动insmod

5.4.3.3、led_class_attrs		// __ATTR()
	(1) 对应/sys/class/leds/目录里的内容，一般是文件和文件夹
	(2) 应用程序通过/sys/class/leds/目录下的属性文件来操作驱动进而操作硬件设
		备，类似于/dev/目录下的设备文件
	(3) attribute是另一种驱动实现，区别于file_operations

	// sysfs是驱动和app交互的一个接口

5.4.3.4、led_classdev_register()
	(1) 创建一个属于leds这个类的一个设备/*注册一个设备*/，led驱动框架提供注册
		驱动的接口。作用类似于register_chardev()通过file_operation方式注册

5.4.5.2、添加led驱动框架支持
	(1) make menuconfig -> devices driver -> led support ->
		-> CONFIG_LEDS_CLASS -> drivers/leds/Makefile ->
		-> obj-$(CONFIG_LEDS_CLASS)		+= led-class.o ->
		-> sys/class/led

	// 此时只有空的class，内部没有device
	// class_create()在sys/class创建一个空的类
	// device_create()创建属于这个类的设备

	// make menuconfig目录层次与源码目录层次相近

5.4.6.基于驱动框架写led驱动1
	(1) 参考 drivers/leds/leds-s3c24xx.c

	(2) 关键点：led_classdev_register()
		// 首先检测驱动是否注册成功。老版/proc/devices 新版/sys/class/leds/

		// 驱动框架中leds_init()已经调用class_create()生成了class类
		// 自己调用led_classdev_register() -> device_create()生成相对应的设备

5.4.8.3、gpiolib引入
	(3) gpiolib属于驱动框架的一部分，内核用来统一管理系统中所有GPIO

5.4.9.1、gpiolib学习重点
	(1) gpiolib的建立过程
	(2) gpiolib的使用方法：申请、使用、释放
	(3) gpiolib的架构：涉及哪些目录的哪些文件

	// mach-s5pv210/gpiolib.c
	// plat-samsung/gpiolib.c
	// plat-samsung/gpio.c

	// drivers/gpio/gpiolib.c

5.4.10.1、struct s3c_gpio_chip
	(3) 内核中为每个GPIO分配了一个编号，内核方便处理	// chip.base

5.4.16.使用gpiolib完成led驱动
	(1) gpio_request_one()、__gpio_set_value()、gpio_free()

	(2) linux中查看gpio使用情况：
		mount -t debugfs debugfs /tmp
		cat /tmp/gpio
		umount /tmp

5.4.17.2、驱动开发的一般步骤
	(1) 以模块的形式在外部编写、调试
	(2) 将调试好的驱动代码集成到kernel中

5.4.17.3、实践
	(1) 关键点：Kconfig、Makefile、make menuconfig
	(2)	操作步骤：
		第1步：将写好的驱动源文件放入内核源码中正确的目录下
		第2步：在Makefile中添加相应的依赖
		第3步：在Kconfig中添加相应的配置项
		第4步：make menuconfig


