
	/*
		./Makefile
		./arch/arm/configs/x210ii_qt_defconfig
		./.config
		
		arch/arm/kernel/vmlinux.lds.S
		
		arch/arm/kernel/head.S
		arch/arm/kernel/head_common.S
		./init/main.c
		
		include/kernel/		-- 内核基本头文件
		include/net/		-- 各种驱动或功能部件头文件
		include/asm/		-- include/asm-xxx/的链接
		
		kernel				-- arch/arm/kernel		// lib, mm, include
	*/

2.16.1.3、Makefile分析
	(4)	Makefile中刚开始定义了kernel版本号		// 在模块化驱动安装时会需要用到

	(5)	make编译内核时传参		// make O=xxx指定编译目录

	(6)	kernel的顶层Makefile中两个重要变量: ARCH, CROSS_COMPILE

	/* Makefile传参 */
	// make O=/tmp/mykernel 	\
			ARCH=arm 			\
			CROSS_COMPILE=/usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

2.16.1.4、链接脚本分析
	(1) 程序的入口		// kernel -- head.S 	uboot -- start.S

	(2)	kernel的链接脚本: 编译vmlinux.lds.S生成vmlinux.lds	// 可能是为了使用条件编译的原因

		// link addr = 0x30008000 (0xC0008000)

2.16.2.3、内核运行的硬件条件	/* NOTE: comment */
	(1)	解压代码运行时先将zImage后段的内核解压开，然后再去调用运行真正的内核入口

	(2)	内核启动有一定的先决条件	/* uboot构建kernel启动环境 */

		// theKernel(0, machid, bd->bi_boot_params)
		// r0 = 0, r1 = macid, r2 = bd->bi_boot_params

2.16.3.内核启动的汇编阶段
	// 校验启动合法性、建立段式映射的页表并开启MMU
	// 构建C语言运行环境(复制data段, 清bss段)
	// b start_kernel

2.16.4.内核启动的C语言阶段1
	(2)	对照内核启动的打印信息进行分析

	(1)	分析uboot给kernel传参的影响和实现
	(2)	硬件初始化与驱动加载
	(3)	内核启动后的结局与归宿

2.16.5.2、打印内核版本信息
	(1) printk()		// 打印级别设置
	(6)	linux_banner	// 宏定义在include/generated/

2.16.6.2、Machine查找
	(1)	setup_arch()		// 确定当前内核对应机器的arch, machine

	(2)	setup_machine(machine_arch_type)	// #define machine_arch_type	2456
		// include/generated/mach-types.h

	(3) -- head-common.S
	     |
		 | -- lookup_machine_type
		 |
		 | -- __lookup_machine_type

		// 如何查找取决于如何存放
		// 链接时将所有具有相同段属性的段链接到一块
		|											// arch/arm/include/asm/procinfo.h
		| -- 类型: struct proc_info_list{ ... }		// include/asm-arm/procinfo.h
		|
		| -- 实例: __arm920_proc_info:				// arch/arm/mm/proc-arm920.S
		|
		| -- 链接: .proc.info.init段				// arch/arm/kernel/vmlinux.lds
		|--------------------------------------------------------------------------
		| -- 类型: struct proc_info_list{ ... }		// arch/arm/include/asm/mach/arch.h
		|
		| -- 实例: MACHINE_START(S3C2440, "SDMK2440")	// arch/arm/mach-s3c2440/mach-smdk2440.c
		|
		| -- 链接: .arch.info.init段				// arch/arm/kernel/vmlinux.lds

		// 通过machine_id遍历指定段, 查找并返回machine_desc描述符(结构体指针)

		/* adr r3, 4b */		// 4是符号
		/* NOTE: CONFIG_XXX 注意CONFIG开头的宏 .config */

2.16.7.内核启动的C语言阶段4
	-- setup_arch()中cmdline相关		// cmdline: uboot给kernel传参时的命令行启动参数 -- bootargs
	 |
	 | -- parse_tags();
	 |
	 | -- parse_cmdline();

	// static char default_command_line[COMMAND_LINE_SIZE] __initdata = CONFIG_CMDLINE;
	// __tagtable(ATAG_CMDLINE, parse_tag_cmdline);
	
	-- setup_arch(char **cmdline_p)
	 |
	 | -- struct tag *tags = (struct tag *)&init_tags
	 | -- struct machine_desc *mdesc

	 
	 // bootargs=console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3
	 // $$$$$$$$$cmdline:console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3
	 
2.16.8.4、rest_init

	// 总结: -- start_kernel()	\
			  |					\
			  | -- setup_arch()		// 机器码相关, uboot内核传参相关	\
			  |															\
			  | -- rest_init()

2.16.9.内核启动的C语言阶段6		// os相关介绍
	(1)	rest_init()中调用kernel_thread()启动了2个内核线程: kernel_init和kthreadd
	(2)	schedule()开启了内核的调度系统
	(3)	kernel启动完成调用cpu_idle()

2.16.9.2、什么是内核线程
	(2)	在linux系统中，线程和进程非常相似
	(3)	应用层运行一个程序就构成一个用户进程
		内核中运行一个函数(函数其实就是一个程序)就构成了一个内核进程/线程

2.16.9.3、进程0、进程1、进程2
	(2)	进程号从0开始, 进程0不是用户进程, 属于内核进程
	(5)	进程0：idle进程，空闲进程
		进程1：kernel_init函数就是进程1，init进程
		进程2：kthreadd函数就是进程2，linux内核的守护进程。用来保证linux内核正常工作

	// ps -aux 		-- in ubuntu
	// man 1 ps

2.16.10.init进程详解1
2.16.10.1、init进程完成了从内核态向用户态的转变
(1)一个进程2种状态。init进程刚开始运行的时候是内核态，它属于一个内核线程，
	然后他自己运行了一个用户态下面的程序后把自己强行转成了用户态。
	因为init进程自身完成了从内核态到用户态的过度，因此后续的其他进程都可以工作在用户态下面了。
(2)内核态下做了什么？重点就做了一件事情，就是挂载根文件系统并试图找到用户态下的那个init程序。init进程要把自己转成用户态就必须运行一个用户态的应用程序（这个应用程序名字一般也叫init），要运行这个应用程序就必须得找到这个应用程序，要找到它就必须得挂载根文件系统，因为所有的应用程序都在文件系统中。
内核源代码中的所有函数都是内核态下面的，执行任何一个都不能脱离内核态。应用程序必须不属于内核源代码，这样才能保证自己是用户态。也就是说我们这里执行的这个init程序和内核不在一起，他是另外提供的。提供这个init程序的那个人就是根文件系统。

	// init进程是在用户态下编译得到

(3)用户态下做了什么？init进程大部分有意义的工作都是在用户态下进行的。init进程对我们操作系统的意义在于：其他所有的用户进程都直接或者间接派生自init进程。

(4)如何从内核态跳跃到用户态？还能回来不？
init进程在内核态下面时，通过一个函数kernel_execve来执行一个用户空间编译连接的应用程序就跳跃到用户态了。注意这个跳跃过程中进程号是没有改变的，所以一直是进程1.这个跳跃过程是单向的，也就是说一旦执行了init程序转到了用户态下整个操作系统就算真正的运转起来了，以后只能在用户态下工作了，用户态下想要进入内核态只有走API这一条路了。

2.16.10.2、init进程构建了用户交互界面
(1)init进程是其他用户进程的老祖宗。linux系统中一个进程的创建是通过其父进程创建出来的。根据这个理论只要有一个父进程就能生出一堆子孙进程了。
(2)init启动了login进程、命令行进程、shell进程
(3)shell进程启动了其他用户进程。命令行和shell一旦工作了，用户就可以在命令行下通过./xx的方式来执行其他应用程序，每一个应用程序的运行就是一个进程。

总结：本节的主要目的是让大家认识到init进程如何一步步发展成为我们平时看到的那种操作系统的样子。


2.16.11.init进程详解2
2.16.11.1、打开控制台
(1)linux系统中每个进程都有自己的一个文件描述符表，表中存储的是本进程打开的文件。
(2)linux系统中有一个设计理念：一切届是文件。所以设备也是以文件的方式来访问的。我们要访问一个设备，就要去打开这个设备对应的文件描述符。譬如/dev/fb0这个设备文件就代表LCD显示器设备，/dev/buzzer代表蜂鸣器设备，/dev/console代表控制台设备。
(3)这里我们打开了/dev/console文件，并且复制了2次文件描述符，一共得到了3个文件描述符。这三个文件描述符分别是0、1、2.这三个文件描述符就是所谓的：标准输入、标准输出、标准错误。
(4)进程1打开了三个标准输出输出错误文件，因此后续的进程1衍生出来的所有的进程默认都具有这3个三件描述符。

2.16.11.2、挂载根文件系统
(1)prepare_namespace函数中挂载根文件系统
(2)根文件系统在哪里？根文件系统的文件系统类型是什么？ uboot通过传参来告诉内核这些信息。
uboot传参中的root=/dev/mmcblk0p2 rw 这一句就是告诉内核根文件系统在哪里
uboot传参中的rootfstype=ext3这一句就是告诉内核rootfs的类型。
(3)如果内核挂载根文件系统成功，则会打印出：VFS: Mounted root (ext3 filesystem) on device 179:2.
如果挂载根文件系统失败，则会打印：No filesystem could mount root, tried:  yaffs2
(4)如果内核启动时挂载rootfs失败，则后面肯定没法执行了，肯定会死。内核中设置了启动失败休息5s自动重启的机制，因此这里会自动重启，所以有时候大家会看到反复重启的情况。
(5)如果挂载rootfs失败，可能的原因有：
	最常见的错误就是uboot的bootargs设置不对。
	rootfs烧录失败（fastboot烧录不容易出错，以前是手工烧录很容易出错）
	rootfs本身制作失败的。（尤其是自己做的rootfs，或者别人给的第一次用）

2.16.11.3、执行用户态下的进程1程序
(1)上面一旦挂载rootfs成功，则进入rootfs中寻找应用程序的init程序，这个程序就是用户空间的进程1.找到后用run_init_process去执行他
(2)我们如果确定init程序是谁？方法是：
先从uboot传参cmdline中看有没有指定，如果有指定先执行cmdline中指定的程序。cmdline中的init=/linuxrc这个就是指定rootfs中哪个程序是init程序。这里的指定方式就表示我们rootfs的根目录下面有个名字叫linuxrc的程序，这个程序就是init程序。
如果uboot传参cmdline中没有init=xx或者cmdline中指定的这个xx执行失败，还有备用方案。第一备用：/sbin/init，第二备用：/etc/init，第三备用：/bin/init，第四备用：/bin/sh。
如果以上都不成功，则认命了，死了。


2.16.12.cmdline常用参数
2.16.12.1、格式简介
(1)格式就是由很多个项目用空格隔开依次排列，每个项目中都是项目名=项目值
(2)整个cmdline会被内核启动时解析，解析成一个一个的项目名=项目值的字符串。这些字符串又会被再次解析从而影响启动过程。
2.16.12.2、root=
(1)这个是用来指定根文件系统在哪里的
(2)一般格式是root=/dev/xxx（一般如果是nandflash上则/dev/mtdblock2，如果是inand/sd的话则/dev/mmcblk0p2）
(3)如果是nfs的rootfs，则root=/dev/nfs。

2.16.12.3、rootfstype=
(1)根文件系统的文件系统类型，一般是jffs2、yaffs2、ext3、ubi
2.16.12.4、console=
(1)控制台信息声明，譬如console=/dev/ttySAC0,115200表示控制台使用串口0，波特率是115200.
(2)正常情况下，内核启动的时候会根据console=这个项目来初始化硬件，并且重定位console到具体的一个串口上，所以这里的传参会影响后续是否能从串口终端上接收到内核的信息。

2.16.12.5、mem=
(1)mem=用来告诉内核当前系统的内存有多少

2.16.12.6、init=
(1)init=用来指定进程1的程序pathname，一般都是init=/linuxrc

2.16.12.7、常见cmdline介绍
(1)console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3
第一种这种方式对应rootfs在SD/iNand/Nand/Nor等物理存储器上。这种对应产品正式出货工作时的情况。

(2)root=/dev/nfs nfsroot=192.168.1.141:/root/s3c2440/build_rootfs/aston_rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC0,115200 
第二种这种方式对应rootfs在nfs上，这种对应我们实验室开发产品做调试的时候。


2.16.13.内核中架构相关代码简介
2.16.13.1、内核代码基本分为3块
(1)arch。		本目录下全是cpu架构有关的代码
(2)drivers		本目录下全是硬件的驱动
(3)其他			相同点是这些代码都和硬件无关，因此系统移植和驱动开发的时候这些代码几乎都是不用关注的。

2.16.13.2、架构相关的常用目录名及含义
(1)mach。（mach就是machine architecture）。arch/arm目录下的一个mach-xx目录就表示一类machine的定义，这类machine的共同点是都用xx这个cpu来做主芯片。（譬如mach-s5pv210这个文件夹里面都是s5pv210这个主芯片的开发板machine）；mach-xx目录里面的一个mach-yy.c文件中定义了一个开发板（一个开发板对应一个机器码），这个是可以被扩展的。
(2)plat（plat是platform的缩写，含义是平台）plat在这里可以理解为SoC，也就是说这个plat目录下都是SoC里面的一些硬件（内部外设）相关的一些代码。
在内核中把SoC内部外设相关的硬件操作代码就叫做平台设备驱动。
(3)include。这个include目录中的所有代码都是架构相关的头文件。（linux内核通用的头文件在内核源码树根目录下的include目录里）

2.16.13.3、补充
(1)内核中的文件结构很庞大、很凌乱（不同版本的内核可能一个文件存放的位置是不同的），会给我们初学者带来一定的困扰。
(2)头文件目录include有好几个，譬如：
	kernel/include		内核通用头文件
	kernel/arch/arm/include		架构相关的头文件
		kernel/arch/arm/include/asm
			kernel\arch\arm\include\asm\mach
	kernel\arch\arm\mach-s5pv210\include\mach
	kernel\arch\arm\plat-s5p\include\plat
(3)内核中包含头文件时有一些格式

#include <linux/kernel.h>		kernel/include/linux/kernel.h
#include <asm/mach/arch.h>		kernel/arch/arm/include/asm/mach/arch.h
#include <asm/setup.h>			kernel\arch\arm\include\asm/setup.h
#include <plat/s5pv210.h>		kernel\arch\arm\plat-s5p\include\plat/s5pv210.h

(4)有些同名的头文件是有包含关系的，有时候我们需要包含某个头文件时可能并不是直接包含他，而是包含一个包含了他的头文件。
