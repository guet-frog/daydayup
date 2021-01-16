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
	
    sudo find / -name code

/** season1 - lesson8 - linux网络配置 */
    如果安装虚拟机的主机没有连接任何外部网络，则使用主机模式
	
    windows端tftp软件
    搭建服务器均有一个主目录
	
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
	
	-w
	-Wall
	
	-D定义宏
#endif

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
	30008008:	.word 0x000001fff			// 立即数定义到内存中
	
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
		ldr pc, _start_armboot		// 绝对跳转 -- board.c
		
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
	
#endif

#ifdef latter_part
/** season2-2-1-1-MMU */
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
#endif
	
	
	
	
	
