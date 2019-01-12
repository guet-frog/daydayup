
2.12.1.3、新版uboot配置体系的改变
	(1) 引入linux kernel的配置体系(Kbuild、Kconfig、menuconfig), 可以在图形界面下, 像配置内核一样配置uboot

2.12.2.1、文件夹结构浏览
	// 和架构有关的头文件放在arch/arm/include目录下

2.12.2.3、主Makefile浏览及boards.cfg文件
	(1)uboot配置相关放在board.cfg文件中		// make xxx_config

	cpu:	u-boot-2013.10\arch\arm\cpu\armv7
	board:	u-boot-2013.10\board\samsung\goni

2.12.3.mkconfig脚本分析
	(1)uboot config -- make s5p_goni_config

	(2)新版本的Makefile中:
		%_config::	unconfig
			@$(MKCONFIG) -A $(@:_config=)	// 调用mkconfig 传参: -A s5p_goni

	(3)新版本mkconfig脚本中:
		在24到35行中使用awk正则表达式/*字符串匹配*/将boards.cfg中与$(s5p_goni)匹配的那一行截取出来赋值给变量line
		然后将line的内容以空格为间隔分开, 分别赋值给$1、$2···$8

	(4)注意在解析完boards.cfg之后，$1到$8就有了新的值
		$1 = Active
		$2 = arm
		$3 = armv7
		$4 = s5pc1xx
		$5 = samsung
		$6 = goni
		$7 = s5p_goni
		$8 = -

2.12.3.2、几个传参和其含义		// 主要是通过这几个变量, 创建符号链接, 链接到相应的文件
	(1)几个很重要的变量
		arch=arm
		cpu=armv7
		vendor=samsung
		soc=s5pc1xx

2.12.3.3、符号链接
	(1)include/asm  -> arch/arm/include/asm
	(2)include/asm/arch -> include/asm/arch-s5pc1xx
	(3)include/asm/proc -> include/asm/proc-armv

	最后创建了include/config.h文件

2.12.3.4、Makefile中添加交叉编译工具链	// 首先检查tool chain配置
	(1)没有定义CROSS_COMPILE	// 默认gcc编译的是在inter CPU下使用, 不是交叉编译

2.12.3.5、配置编译测试
	(1)	编译过程：
		make distclean			// make clean -- error
		make s5p_goni_config
		make
	
	(2) 链接相关
	
	// arch/arm/cpu/u-boot.lds
	
	ifneq ($(CONFIG_SYS_TEXT_BASE),)
		LDFLAGS_u-boot += -Ttext $(CONFIG_SYS_TEXT_BASE)
	endif

2.12.4.2、分析：为什么烧录运行不正确？	// 两遍SD checksum Error
	// 烧录方法错误: 同样方法
	// code错误:  -- sd checksum计算方式, sd checksum的生成方式

2.12.6.2、修改u-boot.lds将lowlevel_init.S放到前部
	// u-boot.map判断lowlevel_init.S是否在前8K
	// 判断某个文件是否被编译链接 -- Makefile

2.12.6.3、修改board/samsung/goni/Makefile编译链接问题	// -- redefinition
	(3)参考当前版本的uboot的start.S文件的处理技巧	// all:

2.12.7、添加SDRAM初始化
2.12.9、添加uboot第二阶段重定位，清bss段
2.12.10、移除oneNand，添加mmc初始化
	// grep "R_ARM_RELATIVE" -nR *

2.12.11.CPU时钟信息显示移植1
// cpu.h
	#define IS_SAMSUNG_TYPE(type, id)	\
		static inline int cpu_is_##type(void)
		{
			return s5p_cpu_id == id ? 1 : 0;
		}

	IS_SAMSUNG_TYPE(s5pc100, 0xc100)
	IS_SAMSUNG_TYPE(s5pc110, 0xc110)

	// 将宏改成函数, 下一阶段的预处理不能进行
	// 宏定义在预处理阶段进行

2.12.14.6、关于MACH_TYPE的定义问题
	// MACH_TYPE集中定义在一个文件arch/arm/include/asm/mach-types.h -- 类似于linux kernel

2.12.15.board_init_r移植
	// 去掉oneNand支持, 添加sd/mmc支持

	// cmd_mmc.c mmc读写相关实现

2.12.20.环境变量的移植		// 0x4000, 16KB, 32sector	-- blk #17

2.12.21.环境变量的测试和配置移植

2.12.22.网卡驱动的移植1

	// CONGIF_CMD_NET
	// CONFIG_CMD_PING
	// drivers/net/dm9000x.c -- dm9000_initialize() init eth_devices




