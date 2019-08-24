
5.9.1.1、常用的2种触摸屏
	(1) 电阻触摸屏。驱动一般分2种：一种是SoC内置触摸屏控制器，一种是外置的
		专门触摸屏控制芯片，通过I2C接口和SoC通信
	(2) 电容触摸屏。驱动只有一种，外接专用的电容式触摸屏控制芯片，I2C接口和
		SoC通信

		// SOC == 触摸屏IC == 触摸屏

5.9.1.2、X210使用的触摸屏
	(1) X210V3使用的触摸屏：ft5x06
	(2) X210V3S使用的触摸屏：gslX680

5.9.1.3、学习触摸屏驱动的关键点
	(1) input子系统相关知识
	(2) 中断上下半部
	(3) I2C子系统
	(4) 触摸屏芯片本身知识

5.9.1.4、竞争状态和同步

5.9.2.3、自旋锁和信号量的使用要点
	(1) 自旋锁不能递归
	(2) 自旋锁可以用在中断上下文（信号量不可以，因为可能睡眠，中断上下文不参
		与调度），但是在中断上下文中获取自旋锁之前要/*先禁用本地中断*/
	(3) 自旋锁的核心要求是：拥有自旋锁的代码必须不能睡眠，要一直持有CPU直到
		释放自旋锁
	(4) 信号量和读写信号量适合于保持时间较长的情况，它们会导致调用者睡眠，因此
		只能在进程上下文使用，而自旋锁适合于保持时间非常短的情况，它可以在任何
		上下文使用。如果被保护的共享资源只在进程上下文访问，使用信号量保护该共
		享资源非常合适，如果对共享资源的访问时间非常短，自旋锁也可以。但是如果
		被保护的共享资源需要在中断上下文访问（包括底半部即中断处理句柄和顶半部
		即软中断），就必须使用自旋锁。自旋锁保持期间是抢占失效的，而信号量和读
		写信号量保持期间是可以被抢占的。自旋锁只有在内核可抢占或SMP（多处理器）
		的情况下才真正需要，在单CPU且不可抢占的内核下，自旋锁的所有操作都是空操作

5.9.4.中断的上下半部1
5.9.4.1、中断处理的注意点
	(1) 中断上下文，不能和用户空间数据交互 // 如：copy_to_user() copy_from_user() 在数据交互的过程中，可能会导致休眠
	(2) 不能交出CPU（不能休眠、不能schedule）
	(3) ISR运行时间尽可能短，越长则系统响应特性越差

5.9.4.2、中断下半部2种解决方案
	(2) 下半部处理策略1：tasklet（小任务）
	(3) 下半部处理策略2：workqueue（工作队列）

5.9.4.3、tasklet使用实战
	(1) tasklet相关接口介绍		// declare_tasklet()	tasklet_schedule()

5.9.5.1、workqueue实战演示
	(1) workqueue的突出特点是下半部会交给worker thead，因此下半部处于进程上下
		文，可以被调度，因此可以睡眠。
	(2) include/linux/workqueue.h

5.9.5.2、中断上下半部处理原则
	(1) 必须立即进行紧急处理的极少量任务放入在中断的顶半部中，此时屏蔽了与自
		己同类型的中断，由于任务量少，所以可以迅速不受打扰地处理完紧急任务。
	(2) 需要较少时间的中等数量的急迫任务放在tasklet中。此时不会屏蔽任何中断
		（包括与自己的顶半部同类型的中断），所以不影响顶半部对紧急事务的处理；
		同时又不会进行用户进程调度，从而保证了自己急迫任务得以迅速完成。
	(3) 需要较多时间且并不急迫（允许被操作系统剥夺运行权）的大量任务放在
		workqueue中。此时操作系统会尽量快速处理完这个任务，但如果任务量太大，
		期间操作系统也会有机会调度别的用户进程运行，从而保证不会因为这个任务
		需要运行时间将其它用户进程无法进行。
	(4) 可能引起睡眠的任务放在workqueue中。因为在workqueue中睡眠是安全的。在
		需要获得大量的内存时、在需要获取信号量时，在需要执行阻塞式的I/O操作时，
		用workqueue很合适。

5.9.6.2、linux内核的I2C驱动框架总览
	(2) 源码中I2C相关的驱动均位于：drivers/i2c目录下。linux系统提供2种I2C驱动
		实现方法：第一种叫i2c-dev，对应drivers/i2c/i2c-dev.c，这种方法只是封装
		了主机（I2C master，一般是SoC中内置的I2C控制器）的I2C基本操作，并且向
		应用层提供相应的操作接口，应用层代码需要自己去实现对slave的控制和操作，
		所以这种I2C驱动相当于只是提供给应用层可以访问slave硬件设备的接口，本身
		并未对硬件做任何操作，应用需要实现对硬件的操作，所以这种I2C
		驱动又叫做“应用层驱动”，这种方式并不主流，它的优势是把差异化都放在应用
		中，这样在设备比较难缠（尤其是slave是非标准I2C时）时不用动驱动，而只需
		要修改应用就可以实现对各种设备的驱动
	(3) 第二种I2C驱动是所有的代码都放在驱动层实现，直接向应用层提供最终结果。
		应用层甚至不需要知道这里面有I2C存在，譬如电容式触摸屏驱动，直接向应用
		层提供/dev/input/event1的操作接口，应用层编程的人根本不知道event1中涉
		及到了I2C

5.9.8.linux内核的I2C子系统详解3
5.9.8.1、I2C子系统的4个关键结构体
	(1) struct i2c_adapter			I2C适配器
	(2) struct i2c_algorithm		I2C算法
	(3) struct i2c_client			I2C（从机）设备信息
	(4) struct i2c_driver			I2C（从机）设备驱动

5.9.9.linux内核的I2C子系统详解4
5.9.9.1、i2c-core.c初步分析
	(1) smbus代码略过
	(2) 模块加载和卸载:bus_register(&i2c_bus_type);		// cd /sys/bus

5.9.9.2、I2C总线的匹配机制
	(1) match函数
	(2) probe函数
		总结：I2C总线上有2条分支：i2c_client链和i2c_driver链，当任何一个
			driver或者client去注册时，I2C总线都会调用match函数去对client.name
			和driver.id_table.name进行循环匹配。如果driver.id_table中所有的id
			都匹配不上则说明client并没有找到一个对应的driver，没了；如果匹配上
			了则标明client和driver是适用的，那么I2C总线会调用自身的probe函数，
			自身的probe函数又会调用driver中提供的probe函数，driver中的probe函
			数会对设备进行硬件初始化和后续工作。

5.9.9.3、核心层开放给其他部分的注册接口
	(1) i2c_add_adapter/i2c_add_numbered_adapter()		// register adapter
	(2) i2c_add_driver()								// register driver
	(3) i2c_new_device()								// register client

5.9.10.1、adapter模块的注册	// platform总线

5.9.10.2、probe函数分析
	(1) 填充一个i2c_adapter结构体，并且调用接口去注册之		// i2c_add_numbered_adapter()
	(2) 从platform_device接收硬件信息，做必要的处理（request_mem_region & ioremap、request_irq等）
	(3) 对硬件做初始化（直接操作210内部I2C控制器的寄存器）

5.9.10.3、i2c_algorithm
	(1) i2c->adap.algo = &s3c24xx_i2c_algorithm;
	(2) functionality
	(3) s3c24xx_i2c_doxfer()

// i2c的控制器(adapter)是soc内部外设，使用platform总线合适

	static int i2c_adap_s3c_init(void)
	{
		return platform_driver_register(&s3c34xx_i2c_driver);
	}

	subsys_initcall(i2c_adap_s3c_init);

// i2c-s3c2410.c	i2c platform driver	 -- 匹配可以通过id_table 和 name。 -- platform.c platform_match()
// mach-x210.c 	所有的平台总线设备都在 数组里

// i2c->clk = clk_get(&pdev->dev, "i2c");		// 内核提供的时钟接口

// -------------------------------------------------
struct bus_type platform_bus_type = {};
bus_register(&platform_bus);

struct bus_type i2c_bus_type = {};
bus_register(&i2c_bus_type);

5.9.11_12.linux内核的I2C子系统详解6_7
5.9.11.1、i2c_driver的注册
(1)以gslX680的驱动为例
(2)将驱动添加到内核SI项目中
(3)i2c_driver的基本分析：name和probe		// name不能用来匹配，在iic bus中
											// probe中执行电容触摸屏本身相关初始化

// i2c-core.c -- static int i2c_device_match() -- 仅通过id_table
// driver: -- gslX680.c
// device: -- mach-x210.c

5.9.11.2、i2c_client从哪里来
(1)直接来源：i2c_register_board_info
smdkc110_machine_init
	i2c_register_board_info

struct i2c_board_info {
	char		type[I2C_NAME_SIZE];			// 设备名
	unsigned short	flags;						// 属性
	unsigned short	addr;						// 设备从地址
	void		*platform_data;					// 设备私有数据
	struct dev_archdata	*archdata;
#ifdef CONFIG_OF
	struct device_node *of_node;
#endif
	int		irq;								// 设备使用的IRQ号，对应CPU的EINT
};

(2) 实现原理分析
	内核维护一个链表 __i2c_board_list，这个链表上链接的是I2C总线上挂接的所有
	硬件设备的信息结构体。也就是说这个链表维护的是一个struct i2c_board_info结
	构体链表
	真正的需要的struct i2c_client在别的地方由__i2c_board_list链表中的各个节点
	内容来另外构建生成。

函数调用层次：
i2c_add_adapter/i2c_add_numbered_adapter
	i2c_register_adapter
		i2c_scan_static_board_info
			i2c_new_device
				device_register

总结：I2C总线的i2c_client的提供是内核通过i2c_add_adapter/i2c_add_numbered_adapter接口
调用时自动生成的，生成的原料是mach-x210.c中的
i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));

5.9.13.gslX680驱动的移植实践
5.9.13.1、初步移植实验
(1)源码获取
(2)源码加入内核中				/* 一般测试驱动，编译成模块 */
(3)mach文件中添加board_info
(4)编译后内核去启动
5.9.13.2、在内核配置中添加CONFIG项
(1)定义一个宏名，譬如CONFIG_TOUCHSCREEN_GSLX680
(2)在代码中使用宏来条件编译
(3)在Makefile中使用宏来条件配置
(4)在Kconfig项目中添加宏的配置项
(5)make menuconfig并选择Y或者N


5.9.14_15.gslX680驱动源码分析1_2


5.9.16.老版本触摸屏的驱动
5.9.16.1、ft5x06驱动移植实践
5.9.16.2、ft5x06驱动源码分析

	(1) struct i2c_adapter			I2C适配器
	(2) struct i2c_algorithm		I2C算法
	(3) struct i2c_client			I2C（从机）设备信息
	(4) struct i2c_driver			I2C（从机）设备驱动

// i2c_adapter 注册在platform bus中
	(1) i2c_adapter driver		// i2c-s3c2410.c
		① static struct platform_driver s3c24xx_i2c_driver = {};

	(2) i2c_adapter device		// mach-x210.c -- platform device array
		① struct platform_device s3c_device_i2c0 = {};
		② s3c_i2c0_set_platdata(NULL);

	(3) -- s3c24xx_i2c_probe()
		 |
		 | -- s3c24xx_i2c_init()		// init i2c controller
		 |
		 | -- i2c_add_numbered_adapter()
		    |
			| -- i2c_register_adapter()		// register i2c_device

// touch screen 注册在i2c bus中
	(1) touch screen i2c_driver		// gslX680.c
		① static struct i2c_driver gsl_ts_driver = {};

	i2c_check_functionality(client->adapter, I2C_FUNC_I2C)		// client->adapter

	struct i2c_board_info i2c_devs1[] = {
		.irq = 
	}
	
	gslX680.c -- client->irq = IRQ_PORT;		// line857  数据一般不放在驱动中

	// line860

	gsl_ts_init()	// input irq相关操作

ft5x06_touch_5p.c
//line627 ts->wq = create_workqueue("ft5x06_wq")		// 动态创建workqueue

i2c-dev.c

xmbuf_transfer
