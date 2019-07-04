
5.4.9.linux内核的gpiolib学习1
	本节开始学习gpiolib，主要讲了学习重点的三条主线，和本部分的学习方法，并且
	结合前面的课程找到了gpiolib建立的入口。
5.4.10.linux内核的gpiolib学习2
	本节重点讲了gpiolib中对gpio信息的封装结构体，以及内核中记录各个GPIO的编号
	方法宏定义等。
5.4.11.linux内核的gpiolib学习3
	本节接上节继续分析gpiolib，主要分析了记录芯片中gpio bank的主结构体变量
5.4.12.linux内核的gpiolib学习4
	本节接上节继续分析gpiolib，本节到了三星驱动工程师写的硬件操作函数部分，
	主要是设置GPIO为输入模式、输出模式的2个函数。
5.4.13.linux内核的gpiolib学习5
	本节开始分析gpiolib的驱动框架部分，这部分是内核开发者编写的。本节主要
	讲述其中开放给goiolib使用者的接口
5.4.14.linux内核的gpiolib学习6	
	本节接上节继续分析gpiolib的驱动框架部分，本节继续分析其中开放给gpiolib使用
	者的接口。
5.4.15.linux内核的gpiolib学习7	
	本节接上节继续分析gpiolib的驱动框架部分，主要分析gpiolib自己工作的部分，
	其中重点是各个attribute的实现代码。
5.4.16.使用gpiolib完成led驱动	
	本节使用gpiolib来操作硬件，继续改造我们之前写的led驱动，这个改造后得到的
	驱动就是一个真实的LED设备驱动了。
5.4.17.将驱动添加到内核中
	本节讲述如何将一个写好的驱动程序源码添加到内核中，并且可以在内核中去配置
	该驱动的编译，这在实际工作中很有用。



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
	(1) 参考drivers/leds/leds-s3c24xx.c

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

5.4.10.linux内核的gpiolib学习2
5.4.10.1、struct s3c_gpio_chip
	(1)这个结构体是一个GPIO端口的抽象,这个结构体的一个变量就可以完全的描述一个IO端口。
	(2)端口和IO口是两个概念。S5PV210有很多个IO口（160个左右），这些IO口首先被分成N个端口（port group），然后每个端口中又包含了M个IO口。譬如GPA0是一个端口，里面包含了8个IO口，我们一般记作：GPA0_0（或GPA0.0）、GPA0_1、
	(3)内核中为每个GPIO分配了一个编号，编号是一个数字（譬如一共有160个IO时编号就可以从1到160连续分布），编号可以让程序很方便的去识别每一个GPIO。
5.4.10.2、s5pv210_gpio_4bit
	(1)这个东西是一个结构体数组，数组中包含了很多个struct s3c_gpio_chip类型的变量。


5.4.11.linux内核的gpiolib学习3
5.4.11.1、S5PV210_GPA0宏
(1)这个宏的返回值就是GPA0端口的某一个IO口的编号值，传参就是我们这个IO口在GPA0端口中的局部编号。
(2)samsung_gpiolib_add_4bit_chips这个函数才是具体进行gpiolib的注册的。这个函数接收的参数是我们当前文件中定义好的结构体数组s5pv210_gpio_4bit（其实2个参数分别是数组名和数组元素个数），这个数组中其实就包含了当前系统中所有的IO端口的信息（这些信息包含：端口的名字、端口中所有GPIO的编号、端口操作寄存器组的虚拟地址基地址、端口中IO口的数量、端口上下拉等模式的配置函数、端口中的IO口换算其对应的中断号的函数）。


5.4.12.linux内核的gpiolib学习4
5.4.12.1、几个问题
(1)哪个目录的哪个文件
(2)函数名中为什么有个4bit：三星的CPU中2440的CON寄存器是2bit对应一个IO口，而6410和210以及之后的系列中CON寄存器是4bit对应1个IO口。所以gpiolib在操作2440和210的CON寄存器时是不同的。
5.4.12.2、函数调用关系
samsung_gpiolib_add_4bit_chips
	samsung_gpiolib_add_4bit
	s3c_gpiolib_add
经过分析，发现samsung_gpiolib_add_4bit内部其实并没有做gpiolib的注册工作，而是还在做填充，填充的是每一个GPIO被设置成输入模式/输出模式的操作方法。


5.4.13_14.linux内核的gpiolib学习5_6
5.4.13.1、s3c_gpiolib_add
(1)首先检测并完善chip的direction_input/direction_ouput/set/get这4个方法
(2)然后调用gpiochip_add方法进行真正的注册操作。其实这个注册就是将我们的封装了一个GPIO端口的所有信息的chip结构体变量挂接到内核gpiolib模块定义的一个gpio_desc数组中的某一个格子中。
5.4.13.2、从驱动框架角度再来分析一下gpiolib
(1)之前的分析已经告一段落，截至目前我们已经搞清楚了gpiolib的建立工程。但是这只是整个gpiolib建立的一部分，是厂商驱动工程师负责的那一部分；还有另一部分是内核开发者提供的驱动框架的那一部分，就是我们后面要去分析的第2条主线。
(2)drivers/gpio/gpiolib.c这个文件中所有的函数构成了我们第2部分，也就是内核开发者写的gpiolib框架部分。这个文件中提供的函数主要有以下部分：
gpiochip_add:	是框架开出来的接口，给厂商驱动工程师用，用于向内核注册我们的gpiolib
gpio_request: 	是框架开出来的接口，给使用gpiolib来编写自己的驱动的驱动工程师用的，
				驱动中要想使用某一个gpio，就必须先调用gpio_request接口来向内核的
				gpiolib部分申请，得到允许后才可以去使用这个gpio。
gpio_free:		对应gpio_request，用来释放申请后用完了的gpio
gpio_request_one/gpio_request_array: 这两个是gpio_request的变种	
gpiochip_is_requested:	接口用来判断某一个gpio是否已经被申请了
gpio_direction_input/gpio_direction_output: 接口用来设置GPIO为输入/输出模式，
				注意该函数内部实际并没有对硬件进行操作，只是通过chip结构体变量的
				函数指针调用了将来SoC厂商的驱动工程师写的真正的操作硬件实现gpio
				设置成输出模式的那个函数。
				
以上的接口属于一类，这些都是给写其他驱动并且用到了gpiolib的人使用的
剩下的还有另外一类函数，这类函数是gpiolib内部自己的一些功能实现的代码
				

5.4.15.linux内核的gpiolib学习7
5.4.15.1、gpiolib的attribute部分
(1)CONFIG_GPIO_SYSFS
(2)GPIO的attribute演示
5.4.15.2、相关代码分析
(1)

gpiolib_sysfs_init
	gpiochip_export
		sysfs_create_group


5.4.16.使用gpiolib完成led驱动
5.4.16.1、流程分析
(1)第1步：使用gpio_request申请要使用的一个GPIO
(2)第2步：gpio_direction_input/gpio_direction_output 设置输入/输出模式
(3)第3步：设置输出值gpio_set_value  获取IO口值gpio_get_value
5.4.16.2、代码实践
(1)在led1上编写代码测试通过
(2)扩展支持led2和led3、led4.可以分开注册也可以使用gpio_request_array去一次注册
(3)学习linux中查看gpio使用情况的方法
内核中提供了虚拟文件系统debugfs，里面有一个gpio文件，提供了gpio的使用信息。
使用方法：mount -t debugfs debugfs /tmp，然后cat /tmp/gpio即可得到gpio的所有信息，
使用完后umount /tmp卸载掉debugfs


5.4.17.将驱动添加到内核中
5.4.17.1、驱动的存在形式
(1)野生，优势是方便调试开发，所以在开发阶段都是这种
(2)家养，优势可以在内核配置时make menuconfig决定内核怎么编译，方便集成
5.4.17.2、驱动开发的一般步骤
(1)以模块的形式在外部编写、调试
(2)将调试好的驱动代码集成到kernel中
5.4.17.3、实践
(1)关键点：Kconfig、Makefile、make menuconfig
(2)操作步骤：
第1步：将写好的驱动源文件放入内核源码中正确的目录下
第2步：在Makefile中添加相应的依赖
第3步：在Kconfig中添加相应的配置项
第4步：make menuconfig


	
目录和文件结构：
mach-s5pv210/gpiolib.c 		s5pv210_gpiolib_init()
mach-s5pv210/include/mach/gpio.h			#define S5PV210_GPA0(_nr)	(S5PV210_GPIO_A0_START + (_nr))
arch/arm/plat-samsung/gpiolib.c		里面是210/6410这种4bit CON寄存器类型的操作方法
arch/arm/plat-samsung/gpio.c		里面是24XX这种2bit CON寄存器类型的操作方法
drivers/gpio/gpiolib.c				里面是内核开发者提供的gpiolib的驱动框架部分



