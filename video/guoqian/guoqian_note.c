#if season1
/** season1 - lesson3 - linux系统概述 */
	vi /etc/passwd      linux保存用户信息文件
		root:x:0:0:root:/root:/bin/bash
		aston:x:1000:1000:AstonUbuntu:/home/aston:/bin/bash
		每一行代表一个用户信息
		1 用户名
		2 密码
		3 uid 用户唯一编号 -- 三大类用户，root用户 系统用户1-999 一般用户
		4 group id
		5 用户全名 -- 一般用户名与用户全名相同
		6 home目录
		7 命令解析器

	vi /etc/group       linux用户组信息
		root:x:0:
		1 组名
		2 密码
		3 组编号
		4 组成员
		
	-rw-rw-r-- 1 aston aston 52 Nov 16 a.c
		1 文件访问权限
		2 文件个数 -- 如果是目录，则表示目录里面有多少个文件
		3 文件拥有者
		4 文件所属的组

/** season1 - lesson6 - linux命令详解 */
    *.gz -> gzip
    *.bz2 -> bzip2
    *.tar -> tar命令打包的文件
    *.tar.gz -> tar打包后，再经过gzip压缩
    *.tar.bz2

    gzip file           // 压缩比更高
    gzip -d file.gz

    bzip2 file
    bzip -d file.bz2

    mount /dev/cdrom /mnt       // - I盘 -> 目录  -- vmware模拟光驱
    umount /mnt

    df -hl

    ln oldfile hlink
    ln -s oldfile slink
	
	ln -s ../a.c tmp/tmp/xxx		// ok -- 可以创建不存在文件的符号链接
	
    sudo find / -name code

/** season1 - lesson8 - linux网络配置 */
    如果安装虚拟机的主机没有连接任何外部网络，则使用主机模式
	
	客户端：
		windows端tftp软件
    服务器端：
		搭建服务器均有一个主目录
		/etc/init.d/xinetd			// 配置
		/etc/init.d/xinetd restart	// 重启
		netstat -a | grep tftp		// 查看所有网络服务 有没有tftp服务
	
	
    配置NFS服务，共享哪一个目录，用户的权限（哪些用户可以访问）
	
/** season1 - lesson16 - gcc编译程序 */
	gcc -E hello.c -o hello.i
	gcc -S hello.i -o hello.s
	gcc -c hello.s -o hello.o		// 二进制文件
	gcc hello.o -o hello
	
	time ./a.out
	
	-c不链接
	-g符号信息	-- 可执行文件大小
	-I指定头文件	-- /usr/include/stdio.h -- 编译器gcc默认头文件查找路径
	printf(
	-w
	-Wall
	
	-D定义宏
	
	source ~/.bashrc
	source /etc/profile
	
	arm-linux-gcc -specs=nosys.specs a.c
#endif /* season1 */

#if season2
#if front part
	dd命令相对于cp命令，在复制的过程中可以进行格式转换
	
	dd iflag=dsync oflag=dsync if=xx.bin of=/dev/sdb seek=1
	命令名
	读取文件时，采用同步方式
	写入文件时，采用同步方式
	要复制的文件名
	文件复制到的设备
	sd卡的第一个扇区
	
/** season2-1-1-2 交叉编译工具链 */
	交叉工具
	交叉编译器	arm-linux-gcc	-- 
				arm-linux-gcc -print-search-dir		// error
	交叉链接器	arm-linux-ld
				arm-linux-ld -Tled.lds -o led.elf led.o main.o func.o		// -o 指定链接后的文件名
	elf文件工具	arm-linux-readelf -a led.elf
				Data: little endian
				Machine: ARM
				arm-linux-readelf
	反汇编器	arm-linux-objdump
				arm-linux-objdump -D -S >dump		// 编译时 arm-linux-gcc -g hello.c -o hello
				
	文件格式转换器 	arm-linux-objcopy
					arm不能直接运行elf格式文件，pc linux环境下，通过elf解析器转化为二进制文件
					arm-linux-objcopy -O binary led.elf led.bin
	
	file - 查平台
	arm-linux-readelf -a - 查大小端
	arm-linux-readelf -d - 查依赖库		// NEEDED	Share library:[libc.so.6]
	
	/* - bare device - chapter 1 */
	arm-linux-gcc -g -c led.S
	arm-linux-ld -Tled.lds -o led.elf led.o			// arm-linux-ld Ttext 0x20000000
	arm-linux-objcopy -O binary led.elf led.bin

/** season2-1-1-3 Makefile工程管理 */
	Makefile有多个规则组成
	规则：目标、依赖、命令
	目标只有一个
	伪目标：只有命令没有依赖
	最终目标：Makefile中第一条目标
	make + 目标名，得到指定目标；没有加目标名，默认得到第一条目标
	
	Makefile变量赋值左右两边不能有空格
	系统定义的默认变量
	$^ 所有的依赖文件
	$@ 代表目标
	$< 代表第一个依赖文件
	
	%.o : %.S
	去除回显：命令前加@
	Makefile -f file
	
	obj=$(src_file:%.c=%.o)
	
/** season2-1-1-5 eclipse集成开发环境 */
	eclipse通过GDB server与jlink硬件通信，从而控制开发板
	编译安装GDB server ./build_all
	
	多个环境变量添加时的顺序，注意工具链中默认带有arm-linux-gdb
	
	linux环境下安装jlink软件
	cp -d libjlinkarm.so* /usr/lib -f 			// 首先复制一些jlink库， -d 参数
	
	which eclipse		// 注销自带eclipse。 mv eclipse eclipse-bak
	
/** season2-1-2 ARM相关 */
	arm 7种工作模式，不同模式下可以使用的处理器指令、可访问的寄存器不同
	
	不同程序在不同级别的模式运行。比如操作系统比较稳定 运行在权限比较高的模式
	应用程序 运行在usr模式
	操作系统 运行在svc模式
	
	寻址方式：处理器根据指令中给出的信息来找到指令所需操作数的方式
	
/** season2-1-3 汇编相关 */
	gnu风格汇编，arm风格汇编	
	目的寄存器在前，源寄存器在后
	
	arm机器码		// 汇编器生成
	伪指令不会产生机器码						-- 伪 - 不能产生机器码
	(1) 类似于C语言宏，只在编译的时候起作用
	(2) 转化为其他的指令启动操作作用			-- Nop 通过转化为 mov
	gnu风格伪指令，前面加一个.
	
	定义类伪指令
		.data
		hello:					@ 标号记录数据位置
		.ascii "helloworld"
		bh:
		.byte 0x1
		ADD:
		.word 0xff
		
		arm-linux-readelf -a start.elf		// 查看.data段
		
		.equ DA, 0x66			// 类似C语言宏定义
		
		.align 4
	操作类伪指令
		mov r0, #0x1ff	@ error, 12bit(4bit-shift 8bit-value)
		mov r0, #0xff
		ldr r0, =0x1ff
	
	arm-linux-objdump -D -S start.elf > xxx			// 查看ldr转化为什么指令
	
	30008004:	ldr r0, [pc, #-4]			// 通过访存指令实现
	30008008:	.word 0x000001fff			// 立即数定义到内存中 -- 指令中不用 带 那么大的一个数
	
	nop		// mov r0, r0
	
	arm最多支持16个协处理器，cp15常用于系统控制 -- coprocessor
	
	windows文件名不区分大小写，解压时会覆盖掉部分文件 -- linux解压后，通过网络驱动器映射
	
	ldr pc, _data_abort
	ldr pc, _not_used		// 占位
	ldr pc, _irq
	
	Document options
	Document Type:		C Source File
	File:	*.c;*.h;*.s;*.S
	再次add Tree

/** season2-1-4 uboot相关 */
	uboot工作流程分析
		step1 程序入口
		step2 第一阶段 bl1 		// bl0 -- bootrom
		step3 第二阶段 bl2
		
	step1：
		首先查看顶层目录的Makefile
		查看配置(目标) -- 确定板级相关信息								-- 找到lds
		
		查看板级链接脚本信息 -- /board/samsung/smdk2440/u-boot.lds
		-- 找到代码段最开始为 /cpu/s3c2440/start.o 		-- 找到对应start.c 或者 start.s
		-- 找到入口 ENTRY(_start)						-- 找到_start标号，找到整个uboot的入口
		
		cpu/s3c2440/start.s
	
	step2：		// 只关心做了什么，不关心怎么做
		设置中断向量表
		设置cpu为svc模式
		刷新I/D cache
		...
		ldr pc, _start_armboot		// 绝对跳转 -- board.c		-- 是否在同一份代码段 -- sram 与 ddr在同一份代码段
		
		LDFLAGS += -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS)
		// 链接器脚本地址0x00000000被覆盖
		board/samsung/smdk2440/config.mk
		
		bl lowlevel_init		// lowlevel_init 0x30008394 -- 没有跳转到内存中
								// b指令相对跳转
								// 链接地址 并不一定是 PC指针的值
								
	step3：只关心硬件初始化，不关心纯软件初始化（环境变量等）
		串口
		LCD
		网卡
		main_loop()		// 解析命令、执行命令
		
	高版本uboot分为两个bin文件
	spl/	-- bootloader1
	
	arch/arm/cpu/armv7/u-boot.lds
	arch/arm/cpu/armv7/start.o
	
	reset:
		b save_boot_param		// cpu.c -- 只有一个函数声明(无实现体)也能调用的原因，弱函数，关联一个别名函数
		
		void save_boot_param(u32 r0, u32 r1, u32 r2, u32 r3) __attribute__((weak, alias("save_boot_param_default")));
	
/** season2-1-5 uboot arm核相关初始化 */
	start.S
		.text 指明为代码段
		.global _start
		_start:
			b reset
			
			b software_interrupt	// 
			ldr pc, =software_interrupt	// ok，不常用 -- 该句指令的最终目的是将地址传给PC
			
	
	software_interrupt:
		nop
		
	// 一般定义一个标号，新开一个内存空间，存放一个指针
	_software_interrupt: .word software_interrupt
	
	// 不再使用伪指令，使用装载指令。将内存单元中的内容转载到PC
	ldr pc, _software_interrupt
	
	// 链接脚本的编写
	
	// Makefile编写 -- 由许多条规则构成
	// -- 首先编写两条通用规则
	%o : %s
		arm-linux-gcc -g -c $^		// 只编译不链接
		
	%.o : %c
		arm-linux-gcc -g -c $^
		
	// -- 再编写最终目标
	all : start.o
		arm-linux-ld -Tgboot.lds -o gboot.elf $^	// ld输入文件，所有的依赖文件
		
	(1) 地址冲突		// App不清楚哪些物理地址在被占用
	(2) 范围较小
	
	arm11之前cache在MMU之前
	
	首先让 I/D cache无效
	然后关闭 I/D cache
	在关闭MMU					// D cache需要关闭，防止部署内核到内存中，部分数据留在cache中，没有写到内存中
#endif /* front part */

/** season2-1-6 led相关 */

/** season2-1-7 时钟相关 */
	晶振频率
	时钟体系，多少个PLL
	PLL分别产生哪些时钟
	PLL产生的时钟作用哪些模块
	
	PLL locktime期间 VCO 不能输出时钟		// VCO is adapted to new clock frequency
	
	2440一般将FCLK(核时钟)与HCLK(总线时钟)设置为不相等，需要将cpu为异步模式
	
/** season2-1-8 内存相关 */
	同步动态随机存储器 -- SDRAM
	同步：内存工作需要同步时钟 -- 传输命令、数据
	动态：需要刷新
	随机：任意指定地址读写数据
	
	DDR -- Double Data Rate SDRAM
	
	内存内部类似于 表结构（行号、列号）		// L-bank（行地址、列地址）
	4*4M*16bit
	4L-bank
	每个L-bank有4M格存储单元
	每个存储单元存储16bit数据		// 地址位宽为16bit
	
	27根地址线	// 2exp27 = 128MByte
	32根数据线
	
	存储控制器 -> 内存、NorFlash、网卡芯片		// 内存控制器
	
	2440地址空间
	内存芯片硬件连接
	存储控制器寄存器
	
	6410不仅需要初始化 存储控制器（只有一个），还需要初始化 ddr
	
	ARM从0地址开始运行，6410、210地址可以映射到iRom、IRAM
	
/** C环境相关 */
	栈帧的概念 fp(r11) 与 sp(r13)		// -- main函数的栈帧、func1函数的栈帧
	
	// 栈可以存放局部变量、可以传递参数、保存寄存器值
	c语言传参，<=4个使用r0-r3传递，多出的参数使用栈传递（写入栈内存）
	
	str fp, [sp, #-4]!		// 写内存后，同时sp = sp - 4
	
	arm-linux-readelf -a a.out		// 查看 __bass_start_ __bss_end_
	
	C与汇编混合编程
		1. 汇编调用C函数		// 直接调用 ldr pc, =led_init
		2. C调用汇编函数		// .global led_init
		3. C内嵌汇编

	__asm__(
		汇编语句部分
		:输出部分		// 修改C语言变量值
		:输入部分		// 参数
		:破坏描述部分	// 寄存器值被修改，通知编译器
	)

export TOP_DIR SRCTREE OBJTREE		// ./
export obj src						// ""

unconfig:
	@rm -f $(obj)include/config.h $(obj)include/config.mk \
		$(obj)board/*/config.tmp $(obj)board/*/*/config.tmp \
		$(obj)include/autoconf.mk $(obj)include/autoconf.mk.dep \
		$(obj)board/$(VENDOR)/$(BOARD)/config.mk
		
		
		
		
smdkv210single_config :	unconfig
	@$(MKCONFIG) $(@:_config=) arm s5pc11x smdkc110 samsung s5pc110
	@echo "TEXT_BASE = 0xcc800000" > $(obj)board/samsung/smdkc110/config.mk
	
	
	
mkconfig
	// cpu相关 -- arm
	asm -> asm-arm
	
	// soc相关
	arch -> arch-s5pc11x
	regs.h -> s5pc110.h
	
	创建文件
	config.mk	// ARCH CPU BOARD VENDOR SOC -- ./arm_config.mk -- -D__ARM__
	config.h	// #include <configs/smdkv210single_config.h>
	
	

#include <config.h>			// ./include/config.h


arm-linux-gcc -print-file-name=include
arm-linux-gcc -print-libgcc-file-name

#ifdef latter_part
/** season2-2-1-MMU */
	相同虚拟地址映射到不同的物理地址
	MMU对不同进程(task)有不同映射关系
	
	地址转换
	访问权限控制
	
	虚拟地址20-31bit，用于索引一级转换表(level one fetch)，通过虚拟地址的最后两位确认后级转化（段转化、粗页、细页、无效）
	一级转换表，内存中，初始化建立，通过TTB(translation table基地址)传给MMU
	初始化将TTB传入cp15-c2寄存器，MMU从该寄存器读取TTB
	
	linux系统常用细页转化方式
	bit20 - bit31 一级页表索引
	物理页基地址 + 页内偏移
	
	段式转化10(bit20 bit21) -- 高12bit保存物理基地址
	段基地址 + 偏移地址
	
	页表一般放在内存开头的地方，通常页表的起始地址与内存起始地址相同
	
	MMU与内存中间 -- writebuff
	section descriptor
	section base address(31:20) - SBZ(19-12) - AP(11:10) - SBZ(9) - domain(8:5) - 1(4) - C(3) - B(2) - 10(10)
	段物理基地址
	AP access permission 与 domain配合使用
	C 是否使用cache
	B 是否使用writebuff
	
	段的访问权限是由AP指定，但是段又属于domain
	内存空间分为16个域，每个域的访问权限由 cp15_c3寄存器决定
	
	cp15_c1寄存器bit0控制MMU使能 -- C语言环境下直接操作cp15寄存器 __asm__{ ... };
	
	内存映射，将物理地址与虚拟地址相同 -- 一般打开cache writebuff
	
	void main(void)
	{
		/* - 1 建立页表 */
		create_page_table()
		
		/* - 2 写入TTB */
		
		/* - 3 enable MMU */
		
		/* - */
		
	}
	
	void create_page_table()
	{
		unsigned long *ttb = (unsigned long *)0x30000000
		unsigned long vaddr = 0xA0000000;
		unsigned long paddr = 0x56000000;
		
		*(ttb + (vaddr >> 20)) = (paddr&0xFFFF00000) | SECDESC;		// ttb大小 4G / 1M ???
		
		/* 映射内存地址 */
		vaddr = 0x30000000;
		paddr = 0x30000000;
		
		while ()
		{
			
		}
	}
#endif

/** season2-2-2 中断相关 */
	2440中断处理
	中断产生 -> 中断控制器 -> cpu -> 中断程序总入口 -> 保存环境 -> 查找中断源，执行对应中断服务 -> 恢复环境
	2440非向量中断方式，统一中断入口	ldr pc, _irq
	irq:
		sub lr, lr, #4
		stmfd sp!, {r0-r12, lr}		// 保护现场
		bl handle_int
		ldmrd sp!, {r0-r12, pc}^	// 恢复现场，^表示把spsr恢复到cpsr
		
	同时要保存lr指针
	cpu正在执行A指令，PC=A+8，此时产生中断，cpu执行完A指令后执行中断服务程序
	中断返回时，从lr-4处开始执行
	
	// 初始化svc模式下sp，初始化irq模式下sp
		
	6410/210向量中断方式	// 硬件完成 -- 读取中断源寄存器，判断中断类型，跳转到相应中断服务函数

#endif /* season2 */
/** season4-1-1 前言 */
	开发板嵌入式环境搭建 -- uboot、linux内核制作、文件系统制作
	linux内核开发基础 -- 内核模块开发、内核子系统（内存管理子系统、进程管理子系统）、内核链表、内核系统调用
	linux驱动开发
	
/** season4-1-2 uboot相关 */
	帮助
		help
	环境变量相关
		printenv	// print
		setenv name value			// 添加、修改
		setenv name					// 删除
		saveenv
	程序下载
		tftp配置
		关闭防火墙
		/
		tftp 0xC0008000 uImage.bin
	内存操作
		md addr
		md.b addr
		
		mm addr value
	flash操作
		nand erase addr len
		nand write mem_addr flash_addr len
		nand read mem_addr flash_addr len
	程序执行
		bootm {addr} {arg}		// 执行 固定格式(head) 的二级制程序 -- boot检查head
	设置自动启动
		setenv bootcmd nand read C0008000 400000 500000 \; bootm C0008000
		setenv bootcmd tftp C0008000 uImage.bin \; bootm C0008000			// \;
	
/** 嵌入式linux内核相关 */
	linux体系结构
		user space		app、c库、系统配置文件
		kernel space	一般存在于内存当中不可见，系统调用接口、体系结构相关代码
	
	linux内核组成
		由7个模块(子系统)构成
		system call interface(SCI)
		process management(PM)
		virtual file system(VFS)
		Memory Management(MM)
		NetWork Stack
		Arch
		Device Drivers(DD)
	
	不同模式，cpu权限不同
	不同模式间的切换 -- 硬件中断、系统调用
	
	www.kernel.org
	
	
	
	
