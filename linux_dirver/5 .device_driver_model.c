***********《朱有鹏老师嵌入式linux核心课程》 ***********
《5.linux驱动开发-第5部分-5.5.linux设备驱动模型》

--------------------------------------------------------
本课程由朱老师物联网大讲堂推出并提供技术支持，课件可打包下载
网盘地址：http://yunpan.cn/cjVy3RAgfDufK 访问密码 4ad7
技术交流QQ群：朱老师物联网讲堂1群 397164505
--------------------------------------------------------

第一部分、章节目录
5.5.1.linux设备驱动模型简介
5.5.2.设备驱动模型的底层架构
5.5.3.总线式设备驱动组织方式
5.5.4.platform平台总线简介1
5.5.5.platform平台总线工作原理2
5.5.6.platform平台总线工作原理3
5.5.7.平台总线实践环节1
5.5.8.平台总线实践环节2
5.5.9.平台总线实践环节3
5.5.10.平台总线实践环节4



第二部分、章节介绍
5.5.1.linux设备驱动模型简介
	本节介绍linux设备驱动模型诞生的原因、技术特征、以及其对驱动开发编写调试的影响，主要目的是让大家全方位感受一下到底什么是linux的设备驱动模型。
5.5.2.设备驱动模型的底层架构
	本节讲解设备驱动模型的最底层实现模块，主要有三个：kobject、kobj_type、kset。
5.5.3.总线式设备驱动组织方式
	本节详解linux内核中以总线为连接的驱动组织形式，详细分析了类、总线、设备、驱动等几个概念及其关联。
5.5.4.platform平台总线简介1	
	本节讲述platform平台总线的目的、意义、工作方式等，注意本节的关键在于platform自己本身的构建和工作原理，下节课才会讲如何使用平台总线来组织设备和驱动。
5.5.5.platform平台总线工作原理2
	本节首先讲解平台总线的工作流程，然后重点讲了平台总线本身自己构建的过程。
5.5.6.platform平台总线工作原理3
	本节以内核中自带的三星为2440写的基于平台总线的LED驱动源码为例，分析了如何在平台总线下去编写驱动程序。
5.5.7.平台总线实践环节1
	本节开始实战编写X210下基于平台总线的LED驱动，本节把上个课程的LED驱动源码拿来改写成平台总线制式，先实现platform_driver。
5.5.8.平台总线实践环节2
	本节分析系统移植时的mach文件，然后找到并添加LED相关的platform_device的注册。
5.5.9.平台总线实践环节3
	本节同时调试platform_device和platform_driver，进行问题排除和代码测试、讲解。
5.5.10.平台总线实践环节4	
	本节重点是硬件配置信息中的数据如何从device端传递到driver端，并且被driver接收用于硬件的操作方法中。
	


第三部分、随堂记录	
5.5.1.linux设备驱动模型简介
5.5.1.1、什么是设备驱动模型
(1)类class、总线bus、设备device、驱动driver
(2)kobject和对象生命周期
(3)sysfs
(4)udev
5.5.1.2、为什么需要设备驱动模型
(1)早期内核（2.4之前）没有统一的设备驱动模型，但照样可以用
(2)2.6版本中正式引入设备驱动模型，目的是在设备越来越多，功耗要求等新特性要求的情况下让驱动体系更易用、更优秀。
(3)设备驱动模型负责统一实现和维护一些特性，诸如：电源管理、热插拔、对象生命周期、用户空间和驱动空间的交互等基础设施
(4)设备驱动模型目的是简化驱动程序编写，但是客观上设备驱动模型本身设计和实现很复杂。
5.5.1.3、驱动开发的2个点
(1)驱动源码本身编写、调试。重点在于对硬件的了解。
(2)驱动什么时候被安装、驱动中的函数什么时候被调用。跟硬件无关，完全和设备驱动模型有关。


5.5.2.设备驱动模型的底层架构
5.5.2.1、kobject
(1)定义在linux/kobject.h中
(2)各种对象最基本单元，提供一些公用型服务如：对象引用计数、维护对象链表、对象上锁、对用户空间的表示
(3)设备驱动模型中的各种对象其内部都会包含一个kobject
(4)地位相当于面向对象体系架构中的总基类
5.5.2.2、kobj_type
(1)很多书中简称为ktype，每一个kobject都需要绑定一个ktype来提供相应功能
(2)关键点1：sysfs_ops，提供该对象在sysfs中的操作方法（show和store）
(2)关键点2：attribute，提供在sysfs中以文件形式存在的属性，其实就是应用接口
5.5.2.3、kset
(1)kset的主要作用是做顶层kobject的容器类
(2)kset的主要目的是将各个kobject（代表着各个对象）组织出目录层次架构
(3)可以认为kset就是为了在sysfs中弄出目录，从而让设备驱动模型中的多个对象能够有层次有逻辑性的组织在一起


5.5.3.总线式设备驱动组织方式
5.5.3.1、总线
(1)物理上的真实总线及其作用（英文bus）
(2)驱动框架中的总线式设计
(3)bus_type结构体，关键是match函数和uevent函数
5.5.3.2、设备
(1)struct device是硬件设备在内核驱动框架中的抽象
(2)device_register用于向内核驱动框架注册一个设备
(3)通常device不会单独使用，而是被包含在一个具体设备结构体中，如struct usb_device
5.5.3.3、驱动
(1)struct device_driver是驱动程序在内核驱动框架中的抽象
(2)关键元素1：name，驱动程序的名字，很重要，经常被用来作为驱动和设备的匹配依据
(3)关键元素2：probe，驱动程序的探测函数，用来检测一个设备是否可以被该驱动所管理
5.5.3.4、类
(1)相关结构体：struct class 和 struct class_device
(2)udev的使用离不开class
(3)class的真正意义在于作为同属于一个class的多个设备的容器。也就是说，class是一种人造概念，目的就是为了对各种设备进行分类管理。当然，class在分类的同时还对每个类贴上了一些“标签”，这也是设备驱动模型为我们写驱动提供的基础设施。
5.5.3.5、总结
(1)模型思想很重要，其实就是面向对象的思想
(2)全是结构体套结构体，对基本功（语言功底和大脑复杂度）要求很高


5.5.4.platform平台总线工作原理1
5.5.4.1、何为平台总线
(1)相对于usb、pci、i2c等物理总线来说，platform总线是虚拟的、抽象出来的。
(2)回顾裸机中讲的，CPU与外部通信的2种方式：地址总线式连接和专用接口式连接。平台总线对应地址总线式连接设备，也就是SoC内部集成的各种内部外设。
(3)思考：为什么要有平台总线？进一步思考：为什么要有总线的概念？
5.5.4.2、平台总线下管理的2员大将
(1)platform工作体系都定义在drivers/base/platform.c中
(2)两个结构体：platform_device和platform_driver
(3)两个接口函数：platform_device_register和platform_driver_register

struct platform_device {
	const char	* name;			// 平台总线下设备的名字
	int		id;
	struct device	dev;		// 所有设备通用的属性部分
	u32		num_resources;		// 设备使用到的resource的个数
	struct resource	* resource;	// 设备使用到的资源数组的首地址

	const struct platform_device_id	*id_entry;	// 设备ID表

	/* arch specific additions */
	struct pdev_archdata	archdata;			// 自留地，用来提供扩展性的
};

struct platform_driver {
	int (*probe)(struct platform_device *);		// 驱动探测函数
	int (*remove)(struct platform_device *);	// 去掉一个设备
	void (*shutdown)(struct platform_device *);	// 关闭一个设备
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;				// 所有设备共用的一些属性
	const struct platform_device_id *id_table;	// 设备ID表
};


5.5.5.platform平台总线工作原理2
5.5.5.1、平台总线体系的工作流程
(1)第一步：系统启动时在bus系统中注册platform
(2)第二步：内核移植的人负责提供platform_device
(3)第三步：写驱动的人负责提供platform_driver
(4)第四步：platform的match函数发现driver和device匹配后，调用driver的probe函数来完成驱动的初始化和安装，然后设备就工作起来了
5.5.5.2、代码分析：platform本身注册
(1)每种总线（不光是platform，usb、i2c那些也是）都会带一个match方法，match方法用来对总线下的device和driver进行匹配。理论上每种总线的匹配算法是不同的，但是实际上一般都是看name的。
(2)platform_match函数就是平台总线的匹配方法。该函数的工作方法是：如果有id_table就说明驱动可能支持多个设备，所以这时候要去对比id_table中所有的name，只要找到一个相同的就匹配上了不再找了，如果找完id_table都还没找到就说明每匹配上；如果没有id_table或者每匹配上，那就直接对比device和driver的name，如果匹配上就匹配上了，如果还没匹配上那就匹配失败。


5.5.6.platform平台总线工作原理3
5.5.6.1、以leds-s3c24xx.c为例来分析platform设备和驱动的注册过程
(1)platform_driver_register
(2)platform_device_register
5.5.6.2、platdata怎么玩
(1)platdata其实就是设备注册时提供的设备有关的一些数据（譬如设备对应的gpio、使用到的中断号、设备名称····）
(2)这些数据在设备和驱动match之后，会由设备方转给驱动方。驱动拿到这些数据后，通过这些数据得知设备的具体信息，然后来操作设备。
(3)这样做的好处是：驱动源码中不携带数据，只负责算法（对硬件的操作方法）。现代驱动设计理念就是算法和数据分离，这样最大程度保持驱动的独立性和适应性。
5.5.6.3、match函数的调用轨迹
5.5.6.4、probe函数的功能和意义


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
(1)probe函数应该做什么
(2)probe函数的数据从哪里来
(3)编程实践

5.5.10.平台总线实践环节4


