
2.3.2.2、配置
	(1)uboot和linux kernel等复杂项目, 都不能直接编译, 需要先配置才能编译
	(2)uboot配置方法: make x210_sd_config	// -- 执行./configmk脚本

2.3.2.3、编译得到uboot.bin
	(1)检查当前编译环境或者主Makefile中编译工具链设置	// make "CROSS_COMPILE=xxx"有更高优先级
		// make or make -j4

2.3.3.2、各文件介绍
	(1 ) .gitignore
	(2 ) arm_config.mk 	// 各个层级special config.mk
	(4 ) config.mk		// 主Makefile包含, 编译工具链的配置
	(5 ) COPYINGuboot	// uboot是GPL许可证
	(7 ) image_split	// 脚本. //for split boot.bin to BL1
	(9 ) MAKEALL		// 脚本, 帮助编译uboot
	(10) Makefile		// for all uboot src compile management
	(11) mk				// shell script: clean -> config -> compile
	(12) mkconfig		// uboot配置阶段的主要配置脚本, uboot可移植性的主要依赖, uboot配置编译过程的实现
	(13) mkmovi			// script for iNand/SD boot
	(15) rules.mk		// uboot的Makefile使用的规则

	// important file:	                                \
                      (1)config.mk:  toolchain配置相关  \
					  (2)mkconfig :  uboot配置相关      \
					  (3)Makefile :  uboot编译相关

2.3.4.uboot的源码目录分析2
	(1) api			硬件无关的功能函数API, uboot自身使用的API, 移植时不需要考虑
	(3) board		问题一: 思考uboot如何支持多套开发板, 如何具有可移植性 -- //每个开发板在board目录下区分
					问题二: board下有这么多文件夹，究竟如何确定具体使用的是哪一个 -- // 编译前配置的目的
					问题三: board目录下以vendor命名, board/samsung/x210 -- 配置时需要注意路径
					//warning: uboot的配置阶段(mkconfig脚本和Makefile中配置有关的部分)	\
						主要解决的问题: 确定编译时源文件路径 -- 在可移植性上的支持
	(4) common		平台无关代码, (1)uboot系统命令--cmd (2)实现环境变量--env
	(5) cpu			SOC相关初始化和控制代码, 譬如CPU、中断、串口等SoC内部外设的, 包括/*起始代码*/start.S
					//warning: 硬件平台相关, 移植时一般无需修改
	(6) disk
	(7) doc
	(8) drivers		drivers from Linux devices driver // eth inand/sd nand...
					uboot中的驱动其实就是linux中的驱动, uboot在一定程度上移植了linux的驱动
					但是linux是操作系统而uboot只是个裸机程序, 因此这种移植会有不同
					//uboot中的驱动其实是linux中的驱动的一部分
	(9)examples
	(10)fs			file system from linux src, 用来管理flash等资源
	(11)include		uboot和linux kernel在管理头文件时, 把所有的头文件全部集中存放在include目录下
	(12)lib_*		典型的lib_arm(arm架构)和lib_generic架构(其他架构通用)
	(13)libfdt		设备树相关, linux kernel 3.4 更改了启动时传参的机制, 改用设备树来进行启动传参, 进行硬件信息的描述
	(14)nand_spl	nand相关 //no use
	(15)net			网络相关, 譬如uboot中的tftp nfs ping实现
	(16)onenand
	(17)post
	(18)sd_fusing	代码实现了烧录uboot镜像到SD卡 //important
	(19)tools		里面是一些工具类的代码, 譬如mkimage

	// important file directory:	\
		board, common, cpu, drivers, include, lib_arm, lib_generic, sd_fusing

	uboot文件:
		1. 版本相关 		include/version_autogenerated.h		// 编译生成
		2. 宏定义			include/autoconf.mk					// 编译生成
		3. 设备信息相关		include/config.mk					// 配置生成
        4. 头文件路径       include/config.h                    // 配置生成
		5. 工具链配置相关	./config.mk							// 源码
		6. uboot配置相关 	./mkconfig							// 源码

    */源文件
        1. ./config.mk
        2. ./mkconfig
        3. ./include/version.h
        4. ./include/configs/x210_sd.h
        5. ./board/samsung/x210/u-boot.lds

    */配置后生成的文件:
        1. 执行mkconfig生成的文件:
            (1) include/config.mk           // param: ARCH ...
            (2) include/config.h            // #include <configs/x210_sd.h>
            (3) include/asm                 // 符号链接: asm    -> asm-arm
            (4) include/regs.h              // 符号链接: regs.h -> s5pc110.h
            (5) include/asm-arm/arch        // 符号链接: arch   -> arch-s5pc11x
            (6) include/asm-arm/proc        // 符号链接: proc   -> proc-armv
                // 确定ARCH...生成config.mk
                // 建立板相关的头文件./include/config.h
                // 建立指向其他文件的软链接

        2. board/samsung/x210/config.mk     // 变量定义: TEXT_BASE = 0xc3e0_0000
            // other special config.mk already in src code
            // ./arm_config.mk
            // ./cpu/s5pc11x/config.mk

    */编译后生成的文件:
        1. include/version_autogenerated.h      // version                      --|
        2. include/autoconf.mk                  // 变量定义 from x210_sd.h      --| --> 编译阶段很早生成

	*/config.mk
		./config.mk
		./include/config.mk
		./board/samsung/x210/config.mk
		./arm_config.mk
		./cpu/s5pc11x/config.mk

	*/important source file
		./cpu/s5pc11x/start.S
		./board/samsung/x210/lowlevel_init.S
		./board/samsung/x210/u-boot.lds
		./cpu/s5pc11x/s5pc110/cpu_init.S
	
        
        
        
        
        
        
        
        
        
        
        
     
        
        
        
        
        