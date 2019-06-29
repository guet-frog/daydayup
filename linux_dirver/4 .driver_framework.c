
5.4.4.初步分析led驱动框架源码2
	本节接上节继续分析led-class.c文件，主要分析了led_classdev_register函数。
5.4.5.在内核中添加或去除某个驱动
	本节讲述如何在内核源码目录中添加或去除某个驱动，并且做实验进行去除实验和
	添加实验，观察sysfs中的变化。
5.4.6.基于驱动框架写led驱动1
	本节首先分析当前基于led驱动框架如何去编写程序，把思路理顺，然后动手编写好
	驱动源代码并编译通过。
5.4.7.基于驱动框架写led驱动2
	本节对上节的代码进行实践操作，并且在实践中结合源码进行分析，通过这样的对
	比分析让大家更深入的理解驱动代码。
5.4.8.基于驱动框架写led驱动3
	本节在上节基础上，讲述如何将4个LED分开实现，这样的实现更加灵活，也是真实
	驱动中采用的方式。
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
	(1) 内核中驱动部分维护者针对每个种类的驱动设计一套成熟的、标准的、典型的驱动实现，
		然后把不同厂家的同类硬件驱动中相同的部分抽出来自己实现好，再把不同部分留出接口给
		具体的驱动开发工程师来实现，这就叫驱动框架。
(2)内核维护者在内核中设计了一些统一管控系统资源的体系，这些体系让内核能够对资源
在各个驱动之间的使用统一协调和分配，保证整个内核的稳定健康运行。譬如系统中所有的
GPIO就属于系统资源，每个驱动模块如果要使用某个GPIO就要先调用特殊的接口先申请，
申请到后使用，使用完后要释放。又譬如中断号也是一种资源，驱动在使用前也必须去申请
。这也是驱动框架的组成部分。
(3)一些特定的接口函数、一些特定的数据结构，这些是驱动框架的直接表现。


5.4.2.内核驱动框架中LED的基本情况
5.4.2.1、相关文件
(1)drivers/leds目录，这个目录就是驱动框架规定的LED这种硬件的驱动应该待的地方。
(2)led-class.c和led-core.c，这两个文件加起来属于LED驱动框架的第一部分，这两个文件是内核开发者提供的，他们描述的是内核中所有厂家的不同LED硬件的相同部分的逻辑。
(3)leds-xxxx.c，这个文件是LED驱动框架的第2部分，是由不同厂商的驱动工程师编写添加的，厂商驱动工程师结合自己公司的硬件的不同情况来对LED进行操作，使用第一部分提供的接口来和驱动框架进行交互，最终实现驱动的功能。
5.4.2.2、九鼎移植的内核中led驱动
(1)九鼎实际未使用内核推荐的led驱动框架
(2)drivers/char/led/x210-led.c
5.4.2.3、案例分析驱动框架的使用
(1)以leds-s3c24xx.c为例。leds-s3c24xx.c中通过调用led_classdev_register来完成我们的LED驱动的注册，而led_classdev_register是在drivers/leds/led-class.c中定义的。所以其实SoC厂商的驱动工程师是调用内核开发者在驱动框架中提供的接口来实现自己的驱动的。
(2)驱动框架的关键点就是：分清楚内核开发者提供了什么，驱动开发者自己要提供什么
5.4.2.4、典型的驱动开发行业现状
(1)内核开发者对驱动框架进行开发和维护、升级，对应led-class.c和led-core.c
(2)SoC厂商的驱动工程师对设备驱动源码进行编写、调试，提供参考版本，对应leds-s3c24xx.c
(3)做产品的厂商的驱动工程师以SoC厂商提供的驱动源码为基础，来做移植和调试


5.4.3_4.初步分析led驱动框架源码1_2
	(1)led-core.c
	(2)led-class.c

5.4.3.2、subsys_initcall()		// linux/init.h

	// kernel启动时自动执行，不需要insmod。该模块只能设置为Y或N。不能设置成M

	(4)subsys_initcall是一个宏，定义在linux/init.h中。经过对这个宏进行展开，
	发现这个宏的功能是：将其声明的函数放到一个特定的段：.initcall4.init。
subsys_initcall
	__define_initcall("4",fn,4)
(5)分析module_init宏，可以看出它将函数放到了.initcall6.init段中。
module_init
	__initcall
		device_initcall
			__define_initcall("6",fn,6)

(6)内核在启动过程中需要顺序的做很多事，内核如何实现按照先后顺序去做很多初
	始化操作。内核的解决方案就是给内核启动时要调用的所有函数归类，然后每
	个类按照一定的次序去调用执行。这些分类名就叫.initcalln.init。n的值
	从1到8。内核开发者在编写内核代码时只要将函数设置合适的级别，这些函数就
	会被链接的时候放入特定的段，内核启动时再按照段顺序去依次执行各个段即可。
(7)经过分析，可以看出，subsys_initcall和module_init的作用是一样的，
	只不过前者所声明的函数要比后者在内核启动时的执行顺序更早。

5.4.3.3、led_class_attrs		// __ATTR()
	(1) 什么是attribute，对应将来/sys/class/leds/目录里的内容，一般是文件和
		文件夹。这些文件其实就是sysfs开放给应用层的一些操作接口（非常类似于
		/dev/目录下的那些设备文件）
	(2) attribute有什么用，作用就是让应用程序可以通过/sys/class/leds/目录下
		面的属性文件来操作驱动进而操作硬件设备
	(3) attribute是另一条驱动实现，有区别于之前讲的file_operations那条线

	// sysfs是驱动和app交互的一个接口
	// 通过leds_class->dev_attrs = led_class_attrs实现

5.4.3.4、led_classdev_register()		// 内部主要调用device_create()
	(1) 分析可知，led_classdev_register()创建一个属于leds这个类的一个设备
		其实就是去注册一个设备。所以这个函数其实就是led驱动
		框架中内核开发者提供给SoC厂家驱动开发者的一个注册驱动的接口
	(2) 当我们使用led驱动框架去编写驱动的时候，这个led_classdev_register函数
		的作用类似于我们之前使用file_operations方式去注册字符设备驱动时的
		register_chrdev函数

5.4.5.2、添加led驱动框架支持
	(1) make menuconfig -> devices driver -> led support ->
		-> CONFIG_LEDS_CLASS -> drivers/leds/Makefile ->
		-> obj-$(CONFIG_LEDS_CLASS)		+= led-class.o ->
		-> sys/class/led

	// 此时只有空的class，内部没有device			-- 实验测试2019-6-28 15:39:40
	// class_create()在sys/class创建一个空的类
	// device_create()创建属于这个类的设备

	// make menuconfig目录层次与源码目录层次相近

5.4.6.基于驱动框架写led驱动1
5.4.6.1、分析
(1)参考哪里？  drivers/leds/leds-s3c24xx.c

(2)关键点：led_classdev_register()
	// 首先检测驱动是否注册成功。老版/proc/devices 新版/sys/class/leds/

	// 驱动框架中leds_init()已经调用class_create()生成了class类
	// 自己调用led_classdev_register() -> device_create()生成相对应的设备

5.4.7.基于驱动框架写led驱动2
5.4.7.1、代码实践
(1)调试
(2)分析
通过分析看出：
第1：我们写的驱动确实工作了，被加载了，/sys/class/leds/目录下确实多出来了一个表示设备的文件夹。文件夹里面有相应的操控led硬件的2个属性brightness和max_brightness
第2：led-class.c中brightness方法有一个show方法和store方法，这两个方法对应用户在/sys/class/leds/myled/brightness目录下直接去读写这个文件时实际执行的代码。
当我们show brightness时，实际就会执行led_brightness_show函数
当我们echo 1 > brightness时，实际就会执行led_brightness_store函数
(3)show方法实际要做的就是读取LED硬件信息，然后把硬件信息返回给我们即可。所以show方法和store方法必要要会去操控硬件。但是led-class.c文件又属于驱动框架中的文件，它本身无法直接读取具体硬件，因此在show和store方法中使用函数指针的方式调用了struct led_classdev结构体中的相应的读取/写入硬件信息的方法。
(4)struct led_classdev结构体中的实际用来读写硬件信息的函数，就是我们自己写的驱动文件leds-s5pv210.c中要提供的。

5.4.7.2、添加硬件操作


5.4.8.基于驱动框架写led驱动3
5.4.8.1、在驱动中将4个LED分开
(1)好处。驱动层实现对各个LED设备的独立访问，并向应用层展示出4个操作接口led1、led2、led3、led4，这样应用层可以完全按照自己的需要对LED进行控制。
驱动的设计理念：不要对最终需求功能进行假定，而应该只是直接的对硬件的操作。有一个概念就是：机制和策略的问题。在硬件操作上驱动只应该提供机制而不是策略。策略由应用程序来做。
(2)如何实现
5.4.8.2、和leds-s3c24xx.c的不同
5.4.8.3、gpiolib引入
(1)一个事实：很多硬件都要用到GPIO、GPIO会复用
(2)如果同一个GPIO被2个驱动同时控制了，就会出现bug
(3)内核提供gpiolib来统一管理系统中所有GPIO
(4)gpiolib本身属于驱动框架的一部分


5.4.9.linux内核的gpiolib学习1
5.4.9.1、gpiolib学习重点
(1)gpiolib的建立过程
(2)gpiolib的使用方法：申请、使用、释放
(3)gpiolib的架构：涉及哪些目录的哪些文件
5.4.9.2、gpiolib的学习方法
(1)以一条主线进去，坚持主线
(2)中途遇到杂碎知识，彻底搞定之，然后继续主线
(3)随时做笔记以加深理解和记忆
(4)学习途中注意架构思想，提升自己大脑的空间复杂度
5.4.9.3、主线1：gpiolib的建立
(1)找到目标函数
smdkc110_map_io
	s5pv210_gpiolib_init		这个函数就是我们gpiolib初始化的函数
	

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
使用方法：mount -t debugfs debugfs /tmp，然后cat /tmp/gpio即可得到gpio的所有信息，使用完后umount /tmp卸载掉debugfs


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
mach-s5pv210/gpiolib.c 		s5pv210_gpiolib_init
mach-s5pv210/include/mach/gpio.h			#define S5PV210_GPA0(_nr)	(S5PV210_GPIO_A0_START + (_nr))
arch/arm/plat-samsung/gpiolib.c		里面是210/6410这种4bit CON寄存器类型的操作方法
arch/arm/plat-samsung/gpio.c		里面是24XX这种2bit CON寄存器类型的操作方法
drivers/gpio/gpiolib.c				里面是内核开发者提供的gpiolib的驱动框架部分



