
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

2.16.2.3、内核运行的硬件条件	/* note: comment */
	(1)	解压代码运行时先将zImage后段的内核解压开，然后再去调用运行真正的内核入口

	(2)	内核启动有一定的先决条件	/* uboot构建kernel启动环境 */

		// theKernel(0, machid, bd->bi_boot_params)
		// r0 = 0, r1 = macid, r2 = bd->bi_boot_params

2.16.3.内核启动的汇编阶段
	// 校验启动合法性、建立段式映射的页表并开启MMU
	// 构建C语言运行环境(复制data段, 清bss段)
	// b start_kernel

2.16.4.内核启动的C语言阶段1
	(1)	对照内核启动的打印信息进行分析
	(2)	分析uboot给kernel传参的影响和实现
	(3)	硬件初始化与驱动加载
	(4)	内核启动后的结局与归宿

2.16.6.1、start_kernel
	-- start_kernel()
	 |
	 | -- printk(linux_banner)	// include/generated
	 |
	 | -- setup_arch()
	 |
	 | -- console_init()
	 |
	 | -- rest_init()

	(1)-- setup_arch()			// cmdline相关: uboot给kernel传参时的命令行启动参数 -- bootargs
	    |
	    | -- parse_tags();		// ATAG_MEM, ATAG_CMDLINE
	    |
	    | -- parse_cmdline();

	// static char default_command_line[COMMAND_LINE_SIZE] __initdata = CONFIG_CMDLINE;
	// __tagtable(ATAG_CMDLINE, parse_tag_cmdline);

	-- 验证uboot给kernel传参的cmdline(bootargs)
	// bootargs=console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3
	// $$$$$$$$$cmdline:console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3

	-- linux传参, Android的启动配置文件, linux etc目录(运行时配置)		// 不用修改源码，不需要重新编译

	(1)	-- setup_arch()		// 确定当前内核对应机器的arch, machine
		 |
		 | -- struct tag *tags = (struct tag *)&init_tags
		 | -- struct machine_desc *mdesc
		 | -- char *from = default_command_line
		 |
		 | -- setup_processor()
		 |
		 | -- mdesc = setup_machine(machine_arch_type)	// include/generated/mach-types.h
			|											// #define machine_arch_type	2456
			| -- mdesc = lookup_machine_type(nr)

	(1) -- head-common.S
	     |
		 | -- lookup_machine_type
		 |
		 | -- __lookup_machine_type

		// 如何查找取决于如何存放
		// 链接时将所有具有相同段属性的段链接到一块
		// 通过machine_id遍历指定段, 查找并返回machine_desc描述符(结构体指针)
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


		/* adr r3, 4b */		// 4是符号
		/* note: CONFIG_XXX注意CONFIG开头的宏 .config */

	(2) -- console_init()
		 |
		 | -- // console driver init

	(3) -- rest_init()
		 |
		 | -- kernel_thread(kernel_init)		// 进程1 -- kernel_init()需要在schedule中注册
		 | -- kernel_thread(kthreadd)
		 |
		 | -- cpu_idle()

	(3) -- kernel_init()
		 |
		 | -- sys_open("/dev/console")
		 | -- (void)sys_dup(0)
		 | -- (void)sys_dup(0)
		 |
		 | -- prepare_namespace()
		    |
			| -- mount_block_root()
			   |
			   | -- do_mount_root()
			      |
				  | -- printk("VFS: Mounted root (%s filesystem) on device %u:%u.\n")
		 |
		 | -- init_post()
		    |
			| -- run_init_process()
			   |
			   | -- kernel_execve("/linuxrc")

2.16.9.3、进程0、进程1、进程2
	(2)	进程号从0开始, 进程0属于内核进程, 在用户空间看不到
	(5)	进程0：idle进程
		进程1：kernel_init		// init进程
		进程2：kthreadd			// linux内核的守护进程, 用来保证linux内核正常工作

	// ps -aux 		-- in ubuntu
	// man 1 ps

2.16.10.1、init进程完成了从内核态向用户态的转变
	// 一个进程2种状态: \
		init进程刚开始是内核态(内核线程)	-- 挂载根文件系统并找到用户态下的init进程	\
		运行用户态下init进程转成用户态		-- kernel_execve("/sbin/init")

	// 由内核态转到用户态时, init进程号没有变 -- 进程1

	// init进程是在用户态下编译得到, 由根文件系统来提供

	// 此后其他进程都工作再用户态下, 其他所有的用户进程都直接或者间接派生自init进程

	// init启动了login进程、命令行进程(提取命令)、shell进程(解析命令, 启动其他进程)

	// 子进程默认继承父进程所有的文件描述符-- 其他进程默认都有标准输入、输出、错误(文件描述符>3)

	// 用户态进入内核态需要调用API

2.16.11.2、挂载根文件系统，执行进程1
	(1)	挂载根文件系统: prepare_namespace()

	(2)	根文件系统在哪里？根文件系统的文件系统类型是什么？
		root=/dev/mmcblk0p2 rw			// SD/MMC Channel0(inand) partition2 -- 0-uboot 1-kernel 2-rootfs
		rootfstype=ext3

	(3)	挂载rootfs成功，则进入rootfs中寻找应用程序的init程序(进程1)		// run_init_process()
		init=/linuxrc

2.16.12.cmdline常用参数
2.16.12.1、格式简介
	(1)	格式就是由很多个项目用空格隔开依次排列，每个项目中都是项目名=项目值
	(2)	整个cmdline会被内核启动时解析，解析成一个一个的项目名=项目值的字符串。
		这些字符串又会被再次解析从而影响启动过程。
2.16.12.2、root=
	(1)这个是用来指定根文件系统在哪里的
	(2)一般格式是root=/dev/xxx（一般如果是nandflash上则/dev/mtdblock2，如果是inand/sd的话则/dev/mmcblk0p2）
	(3)如果是nfs的rootfs，则root=/dev/nfs。

2.16.12.3、rootfstype=
	(1)根文件系统的文件系统类型，一般是jffs2、yaffs2、ext3、ubi
2.16.12.4、console=
	(1)控制台信息声明，譬如console=/dev/ttySAC0,115200表示控制台使用串口0，波特率是115200.
	(2)正常情况下，内核启动的时候会根据console=这个项目来初始化硬件，
		并且重定位console到具体的一个串口上，所以这里的传参会影响后续是否能从串口终端上接收到内核的信息。

2.16.12.5、mem=
(1)mem=用来告诉内核当前系统的内存有多少

2.16.12.6、init=
(1)init=用来指定进程1的程序pathname，一般都是init=/linuxrc

2.16.12.7、常见cmdline介绍
(1)console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3
第一种这种方式对应rootfs在SD/iNand/Nand/Nor等物理存储器上。这种对应产品正式出货工作时的情况。

(2)root=/dev/nfs nfsroot=192.168.1.141:/root/s3c2440/build_rootfs/aston_rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC0,115200 
第二种这种方式对应rootfs在nfs上，这种对应我们实验室开发产品做调试的时候。

