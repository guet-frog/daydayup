
2.15.1.2、分析源码目录下的单个文件
	// Kbuild(kernel build)、Kconfig
	// Makefile、mk

2.15.2.linux内核源码目录结构2
	(10)ipc	inter process communication, 进程间通信, linux支持的IPC的代码实现

	(12)lib	不同于C语言的标准库函数
			内核编程中不能使用C语言标准库函数, lib下的库函数用来替代C标准库函数
			内核编程中要把字符串转成数字只能用lib目录下的atoi函数, 不能用标准C库中的atoi
			内核编程中要打印使用lib目录下的printk, 不能用标准C库中的printf

	// important: arch、drivers

2.15.3.1、先确认Makefile
	// ARCH=arm
	// CROSS_COMPILE = /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

2.15.3.1、配置编译
	step1: make x210ii_qt_defconfig		// cp arch/arm/configs/x210ii_qt_defconfig .config

	step2: make menuconfig		// apt-get install libncurses5-dev -- 安装ncurse库

	step3: make		// arch/arm/boot/zImage

2.15.4.2、配置的关键是得到.config文件	// -- 类似于x210_sd.h
	// 读取.config中配置项, 指导整个编译链接过程

2.15.5.menuconfig的使用和演示
	模块化: 单独链接成一个内核模块.ko文件(编进内核或者编进模块), linux系统内核启动后可以动态的加载或卸载这个模块

2.15.6.1、menuconfig本身由一套软件支持
	// scripts/kconfig/lxdialog -- menuconfig源码

2.15.7.1、Kconfig的格式
(2)	驱动移植等工作时, 需要修改Kconfig中的配置项, 将某个设备驱动添加到内核的配置项目中
(7)	内核源码目录树中每一个Kconfig都会/*source引入其所有子目录下的Kconfig*/, 从而所有的Kconfig项目都被包含进menuconfig

2.15.7.2、tristate和bool的含义
(1)	tristate三态(Y、N、M)
	bool(Y、N)

	// 注意Kconfig文件中的depends
	// menuconfig中的菜单项、Kconfig中的配置项、.config中的一行、Makefile中的一行


