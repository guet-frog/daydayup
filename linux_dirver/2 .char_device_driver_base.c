
5.2.2.1、常用的模块操作命令
	(1) lsmod
	(2) insmod		// insmod xxx.ko
	(3) modinfo		// modinfo xxx.ko
	(4) rmmod
	(5) modprobe、depmod

	// 驱动一般实现为模块的方式
	// 内核源码树下编译驱动，调用内核Makefile		\
							 驱动安装时版本校验不会出错 -- 保持vermagic一致

5.2.2.2、模块的安装与卸载
	(2)	module_init(chrdev_init)		// insmod xxx.ko时自动调用chrdev_init()
	(3) module_exit(chrdev_exit)		// rmmod xxx时自动调用chrdev_exit()

5.2.3.2、模块中常用宏
	(1) MODULE_LICENSE
	(2) MODULE_AUTHOR
	(3) MODULE_DESCRIPTION
	(4) MODULE_ALIAS

5.2.3.3、函数修饰符
	(1) __init		// #define __init	__section(.init.text) __cold notrace
		内核启动时统一加载.init.text段中的模块安装函数，加载完后就会把这个段
		给释放掉以节省内存

	(2) inline
		效率
		防止头文件定义的函数，链接时重复定义
		// 头文件中定义变量，结构体保护宏能防止编译error，链接可能error

5.2.4.1、printk函数详解			// for kernel not app
		cat /proc/sys/kernel/printk		// 第一个数字，数字越小级别越高
		// ubuntu中dmesg查看printk打印信息

	(5) ubuntu中这个printk的打印级别控制没法实践，ubuntu中不管你把级别怎么
		设置都不能直接打印出来，必须dmesg命令去查看

5.2.4.2、关于驱动模块中的头文件
	(1) 驱动源代码中包含的头文件和原来应用编程程序中包含的头文件不是一回事

		应用编程中包含的头文件是应用层的头文件，是应用程序的编译器(gcc)提供
		gcc的头文件路径/usr/include，和操作系统无关

		驱动源码属于内核源码的一部分，驱动源码中的头文件其实就是内核源代码目录
		下的include目录下的头文件

5.2.4.3、驱动编译的Makefile分析
	(1) KERN_DIR，变量的值就是我们用来编译这个模块的内核源码树的目录
	(2) obj-m += module_test.o					// obj-m
	(3) make -C $(KERN_DIR) M=`pwd` modules		// make modules

		make -C进入到指定的内核源码树目录下
		然后在源码目录树下/*调用*/内核源码中定义的模块编译规则去编译这个模块
		编译完成后把生成的文件还拷贝到当前目录下	// 通过M=`pwd`实现

5.2.5.用开发板来调试模块
	// uname				Linux
	// uname -r				3.13.0-32-generic
	// Ubuntu内核源码树		/lib/modules/3.13.0-32-generic/build

5.2.6.1、系统整体工作原理
	(1) 应用层->API->设备驱动->硬件
	(2) API：open、read、write、close等
	(3) 驱动源码中提供真正的open、read、write、close等函数实体

5.2.6.2、file_operations	/* kernel/include/linux/fs.h */
	(1) 元素主要是函数指针，用来挂接实体驱动函数
	(2) 每个设备驱动都需要一个该结构体类型的变量
	(3) 设备驱动向内核注册时，提供该结构体类型的变量

5.2.7.1、register_chrdev	/* fs.h */

5.2.7.2、内核如何管理字符设备驱动
	(1) 内核用结构体数组存储注册的字符设备驱动
	(3) cat /proc/devices查看内核中已经注册的char devices和block devices
	(4) 主设备号(major)既是设备编号，也是设备管理表的下标

5.2.8.字符设备驱动代码实践1		// 定义并填充file_operations、注册驱动
	// make && make cp

5.2.10.1、驱动设备文件的创建
	(2) 设备文件的关键信息：主次设备号，使用ls -l去查看设备文件
		就可以得到这个设备文件对应的主次设备号
	(3) 使用mknod创建设备文件：mknod /dev/xxx c 主设备号 次设备号

	// vi /dev/led_test			-- 设备文件一般不用vi打开
	// ls -l /dev/led_test

5.2.11.添加读写接口
	(1) copy_from_user()
		用来将数据从用户空间复制到内核空间
		成功复制则返回0，如果不成功复制则返回尚未成功复制剩下的字节数

	(2) copy_to_user()

	// 注意区分复制和mmap的映射

5.2.13.驱动中如何操控硬件区别裸机
	(1) 寄存器地址不同。物理地址转为虚拟地址
	(2) 编程方法不同。裸机中直接用函数指针操作，kernel用封装好的io读写函数，提
		高可移植性

5.2.14.2、三星版本内核中的静态映射表
	(1) 主映射表位于：arch/arm/plat-s5p/include/plat/map-s5p.h
	(2) 虚拟地址基地址定义在：arch/arm/plat-samsung/include/plat/map-base.h
	(3) GPIO相关的主映射表位于：arch/arm/mach-s5pv210/include/mach/regs-gpio.h
	(4) GPIO的具体寄存器定义位于：arch/arm/mach-s5pv210/include/mach/gpio-bank.h

5.2.17.1、动态映射建立与销毁
	(1) request_mem_region() 向内核申请（报告）需要映射的内存资源
	(2) ioremap() 传入物理地址，返回虚拟地址
	(3) iounmap()
	(4) release_mem_region()		// 注意先后顺序


