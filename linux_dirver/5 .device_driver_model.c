
5.5.1.linux设备驱动模型简介
	(1) 类class、总线bus、设备device、驱动driver
	(2) kobject和对象生命周期
	(3) sysfs
	(4) udev

5.5.1.2、为什么需要设备驱动模型
	(3) 2.6 版本中正式引入设备驱动模型，设备驱动模型负责统一实现和维护一些
		特性，诸如：电源管理、热插拔、对象生命周期、用户空间和驱动空间的交
		互等	// 统一管理驱动，简化驱动编写

5.5.1.3、驱动开发的2个点
	(1) 驱动功能编写 -- 硬件相关操作
	(2) 驱动如何加载 -- 框架相关操作

5.5.2.设备驱动模型的底层架构
5.5.2.1、kobject
	(1) 定义在linux/kobject.h中
	(2) 各种对象最基本单元，提供一些公用型服务如：对象引用计数、维护对象链
		表、对象上锁、对用户空间的表示
	(3) 设备驱动模型中的各种对象其内部都会包含一个kobject
	(4) 地位相当于面向对象体系架构中的总基类
5.5.2.2、kobj_type
	(1) 很多书中简称为ktype，每一个kobject都需要绑定一个ktype来提供相应功能
	(2) 关键点1：sysfs_ops，提供该对象在sysfs中的操作方法（show和store）
	(2) 关键点2：attribute，提供在sysfs中以文件形式存在的属性，其实就是应用接口
5.5.2.3、kset
	(1) kset的主要作用是做顶层kobject的容器类
	(2) kset的主要目的是将各个kobject（代表着各个对象）组织出目录层次架构
	(3) 可以认为kset就是为了在sysfs中弄出目录，从而让设备驱动模型中的多个
		对象能够有层次有逻辑性的组织在一起

5.5.3.总线式设备驱动组织方式
5.5.3.1、总线
	(2) 驱动框架中的总线式设计
	(3) bus_type结构体，关键是match函数和uevent函数

5.5.3.2、设备
	(1) struct device是硬件设备在内核驱动框架中的抽象，struct usb_device是
		struct device的子类
	(2) device_register()用于向内核驱动框架注册一个设备

5.5.3.3、驱动
	(1) struct device_driver是驱动程序在内核驱动框架中的抽象
	(2) 关键元素1：name，驱动和设备匹配依据
	(3) 关键元素2：probe，驱动程序的探测函数，用来检测一个设备是否可以被该驱动所管理
		// 总线式驱动框架使用probe函数
		// 作用相当于module_init()自动执行的函数
		// 驱动的probe()函数如果可以识别这个设备，则设备可以安装该驱动

5.5.3.4、类
	(1) 相关结构体：struct class 和 struct class_device
	(2) udev的使用离不开class
	(3) class的真正意义在于作为同属于一个class的多个设备的容器。
		也就是说，class是一种人造概念，目的就是为了对各种设备进行分类管理
		当然，class在分类的同时还对每个类贴上了一些“标签”，这也是设备驱动
		模型为我们写驱动提供的基础设施

5.5.4.platform平台总线工作原理1
5.5.4.1、何为平台总线
	(1) 相对于usb、pci、i2c等物理总线来说，platform总线是虚拟的、抽象出来的
	(2) CPU与外设通信的2种方式：地址总线式连接和专用接口式连接。平台总线对应
		地址总线式连接设备，也就是SoC内部集成的各种内部外设

	// 为什么需要总线 -- 便于管理驱动和设备???
	// 外设与cpu连接的方式：地址总线(串口)、专用接口(如：nand)

5.5.4.2、平台总线下管理的2员大将		// drivers/base/platform.c
	(2) 两个结构体：platform_device、platform_driver
	(3) 两个接口函数：platform_device_register()、platform_driver_register()

	// platform_device属于device

struct platform_device
{
	const char	* name;
	int		id;
	struct device	dev;
	u32		num_resources;
	struct resource	* resource;		// 资源数组首地址
	const struct platform_device_id	*id_entry;
	struct pdev_archdata	archdata;	// specific data
};

5.5.5.platform平台总线工作原理2
5.5.5.1、平台总线体系的工作流程
	(1)第一步：系统启动时在bus系统中注册platform			// sys/bus -- 内核bus子系统
	(2)第二步：内核移植的人负责提供和注册platform_device	// mach-x210.c
	(3)第三步：写驱动的人负责提供和注册platform_driver
	(4)第四步：platform的match函数发现driver和device匹配后，调用driver的probe
			   函数来完成驱动的初始化和安装，然后设备就工作起来了

	// 第二步：mach-x210.c中添加相应的 struct platform_device xxx_device

	// 1、4步自动完成。需要完成2、3步，提供platform_device和platform_driver

5.5.5.2、代码分析：platform本身注册
	(1) 每种总线（不光是platform，usb、i2c那些也是）都会带一个match方法
		match方法用来对总线下的device和driver进行匹配。理论上每种总线的
		匹配算法是不同的，但是实际上一般都是看name的。
	(2) platform_match函数就是平台总线的匹配方法。该函数的工作方法是：
		如果有id_table就说明驱动可能支持多个设备，所以这时候要去对比
		id_table中所有的name，只要找到一个相同的就匹配上了不再找了，
		如果找完id_table都还没找到就说明每匹配上；如果没有id_table或
		者每匹配上，那就直接对比device和driver的name，如果匹配上就匹
		配上了，如果还没匹配上那就匹配失败。

struct bus_type
{
	const char *name;
	
	int (*match)(struct device *dev, struct device_driver *drv);	// not usb_device or platform_device
}

static int platform_match(struct device *dev, struct device_driver *drv)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct platform_driver *pdrv = to_platform_driver(drv);
}


// 定义device结构体，调用接口注册
static struct platform device smdk_backlight_device =
{
	.name = "pwm-backlight",
	.id  = -1,
};

ret = platform_device_register(&smdk_backlight_device);

// 通过name搜索相应driver
drivers/video/backlight
static int __init pwm_backlight_init(void)
{
	return platform_driver_register(&pwm_backlight_driver );
} 

//device.h
//mach-mini2440.c
5.5.6.platform平台总线工作原理3
5.5.6.1、以leds-s3c24xx.c为例来分析platform设备和驱动的注册过程
	(1) platform_driver_register
	(2) platform_device_register

5.5.6.2、platdata怎么玩			// 类似于resource结构体
	(1) platdata其实就是设备注册时/*提供的设备有关的一些数据*/
		（譬如设备对应的gpio、使用到的中断号、设备名称····）
	(2) 这些数据在设备和驱动match之后，会由设备方转给驱动方。驱动拿到这些数据后，
		通过这些数据得知设备的具体信息，然后来操作设备。
	(3) 这样做的好处是：驱动源码中不携带数据，只负责算法（对硬件的操作方法）。
		现代驱动设计理念就是算法和数据分离，这样最大程度保持驱动的独立性和适应性

5.5.6.3、match函数的调用轨迹
5.5.6.4、probe函数的功能和意义

// 分离思想：数据和驱动是分离的。leds-s3c24xx.c是驱动，数据是设备所定义的结构体
// devices和driver的name匹配上后，会自动执行probe函数，则执行led_classdev_register()生成相应的设备文件节点

// 以前的驱动如果由操控led1转为操作led2，需要修改代码没有？

5.5.7.平台总线实践环节1
5.5.7.1、回顾
5.5.7.2、先初步改造添加platform_driver
(1)第1步：先修改原来的代码到只有led1
(2)第2步：
5.5.7.3、实验现象分析


5.5.8.平台总线实践环节2
5.5.8.1、检查mach-x210.c中是否有led相关的platform_device
5.5.8.2、参考mach-mini2440.c中添加led的platform_device定义
5.5.8.3、测试只有platform_device没有platform_driver时是怎样的


5.5.9.平台总线实践环节3
5.5.9.1、测试platform_device和platform_driver相遇时会怎样
5.5.9.2、probe函数
	(1) probe函数应该做什么
	(2) probe函数的数据从哪里来
	(3) 编程实践

5.5.10.平台总线实践环节4

x210-led.c	

// linux/device.h

struct device_attribute
{
	struct attribute	attr;
	ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
	ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
};

#define DEVICE_ATTR(_name, _mode, _show, _store) \
struct device_attribute dev_attr_##_name = __ATTR(_name, _mode, _show, _store)

#define __ATTR(_name,_mode,_show,_store) { \
	.attr = {.name = __stringify(_name), .mode = _mode },	\
	.show	= _show,					\
	.store	= _store,					\
}

// x210-led.c
static DEVICE_ATTR(led1, 0666, x210_led_read, x210_led_write);

struct device_attribute dev_attr_led1 =
{
	.attr =
	{
		.name = led1,
		.mode = 0666,
	},
	.show = x210_led_read,
	.store = x210_led_write,
}


