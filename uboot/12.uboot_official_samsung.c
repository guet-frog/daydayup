



2.12.4.先解决官方版本uboot的烧录运行
	本节首先移植sd_fusing文件夹，然后分析并修改代码使编译得到的u-boot.bin能够在SD卡中运行起来。


2.12.15.board_init_r移植
	本节开始移植board_init_r中的一些函数，主要内容是去掉原来的oneNand支持，加上SD/MMC的支持并将环境变量修改到SD/MMC中。
2.12.16.uboot2013.10中SD/MMC驱动浏览
	本节进行SD/MMC驱动的浏览和错误问题定位分析，最终分析得到解决方案思路。
2.12.17.SD卡驱动移植1
	本节开始移植SD卡驱动，主要进行驱动文件的逐个分析、文件关系的对应复制Makefile的修改等。
2.12.18.SD卡驱动移植2
	本节继续进行SD卡驱动移植实验，将移植的驱动文件整理然后进行编译、修改等使之可以通过编译。
2.12.19.SD卡驱动移植3
	本节继续进行SD卡驱动移植实验，主要在编译脚本cp.sh中添加代码使之选择性清理配置编译，这样可以提升效率。
2.12.20.环境变量的移植
	本节分析环境变量分区的问题，通过代码分区确认了当前环境变量被放在何处，并且通过修改代码将环境变量放在合适的位置。
2.12.21.环境变量的测试和配置移植
	本节对环境变量进行测试和效果确认，并且将uboot中默认的环境变量表重新设置以使该uboot更方便使用。
2.12.22.网卡驱动的移植1
	本节开始移植网卡驱动，主要是分析网卡驱动初始化代码，然后进行网卡初始化的添加、代码实践、效果查看。
2.12.23.网卡驱动的移植2
	本节接上节继续进行网卡驱动的移植。主要是分析网卡驱动不工作的原因并且进行解决方案分析、实践及效果确认。




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

2.12.4.2、分析：为什么烧录运行不正确？	// SD checksum Error
	// 烧录方法错误: 同样方法
	// code错误:  -- sd checksum计算方式, sd checksum的生成方式

2.12.5.2、添加开发板制锁和串口打印字符"O"

	// 判断某个文件是否被编译链接 -- Makefile

2.12.6.1、添加LED点亮代码跟踪程序运行

	b lowlevel_init		// error -- 挂掉

2.12.6.2、修改u-boot.lds将lowlevel_init.S放到前部

	// u-boot.map判断lowlevel_init.S是否在前8K

2.12.6.3、修改board/samsung/goni/Makefile解决编译问题	// -- redefinition

(3)参考当前版本的uboot的start.S文件的处理技巧	// all:

2.12.7、添加SDRAM初始化

2.12.9.添加uboot第二阶段重定位，清bss段

2.12.10.添加uboot第二阶段重定位2
2.12.10.1、_mian函数中基本处理
(1)主要就是把里面的重定位代码部分给删除掉。剩下就是：设置栈、调用board_init_f函数和board_init_r函数。

2.12.10.2、代码同步及编译
(1)主要是crt0.S和movi.h。

2.12.10.3、编译中出现问题解决
(1)movi.h中宏定义出错，最后在s5p_goni.h中添加了 CONFIG_EVT1这个宏解决了
(2)连接错误：u-boot contains relocations other than 	R_ARM_RELATIVE
在uboot下用grep "R_ARM_RELATIVE" -nR *搜索，发现Makefile中有一个检查重定位的规则，屏蔽掉这个规则后编译连接成功。

2.12.10.4、结果验证及下阶段展望
(1)看到了uboot启动打印出来的一系列信息，但是uboot没有进入命令行。
(2)这说明uboot中的DDR初始化和重定位功能都已经完美实现，后面就是第二阶段的继续移植了。

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

2.12.14.6、关于MACH_TYPE的定义问题。
	// MACH_TYPE集中定义在一个文件arch/arm/include/asm/mach-types.h -- 类似于linux kernel

2.12.15.board_init_r移植
	// 去掉oneNand支持, 添加SD/MMC支持

2.12.16.2、相关函数和文件
drivers/mmc/mmc.c、
drivers/mmc/sdhci.c
board/samsung/goni/goni.c
arch/arm/include/asm/arch-s5pc1xx/mmc.h

2.12.16.3、当前错误定位及解决方案分析
(1)错误发生路径定位
board_init_r
	mmc_initialize
		do_preinit
			mmc_start_init
				mmc_go_idle
					mmc_send_cmd
						sdhci_send_command
							sdhci_transfer_data	错误在这个函数中
(2)错误原因分析
sdhic.c中的所有函数构成了三星210CPU的SD/MMC控制器的驱动。
这里面的函数是三星公司的工程师写的，内容就是用来控制210CPU的内部的SD/MMC控制器和外部的SD卡通信的。这就是所谓的驱动。
sdhci_transfer_data函数出错，说明是SoC的SD/MMC控制器和外部SD卡（其实现在用的是SD0的iNand）的数据传输出了问题。
（细节分析发现是控制器内部有一个中断状态错误标志被置位了。）


2.12.17.SD卡驱动移植1
2.12.17.1、分析两个版本的uboot中SD卡驱动差异
(1)uboot2013.10中：驱动相关的文件主要有：
drivers/mmc/mmc.c
drivers/mmc/sdhci.c
drivers/mmc/s5p_sdhci.c
board/samsung/goni/goni.c
(2)三星移植版本中，驱动相关的文件主要有：
drivers/mmc/mmc.c
drivers/mmc/s3c_hsmmc.c
cpu/s5pc11x/cpu.c
cpu/s5pc11x/setup_hsmmc.c
(3)经过分析发现：SD卡驱动要工作要包含2部分内容，一部分是drivers/mmc目录下的是驱动，另外一部分是uboot自己提供的初始化代码（譬如GPIO初始化、时钟初始化）
2.12.17.2、复制必要的文件并修改相应Makefile
(1)首先解决drivers/mmc目录下的文件替换。
(2)修改初始化代码。
2.12.17.3、代码浏览及修补
(1)按照代码运行时的流程来逐步浏览代码，看哪里需要修补。


2.12.18.SD卡驱动移植2
2.12.18.1、继续修补驱动代码
(1)include/mmc.h
(2)include/s3c_hsmmc.h
2.12.18.2、同步及编译、问题解决
(1)出错1：cmd_mmc.c中出错。原因是cmd_mmc.c和mmc驱动密切相关，所以改了驱动后这个实现文件也要跟着改，解决方法是从三星版本的直接同名文件复制过来替换
(2)出错2：drivers/mmc/mmc_write.c编译出错。
	原因是这个文件和本来版本中的mmc.c文件相关，但是mmc.c被替换掉了所以这个文件编译报错。解决方案就是修改makefile去掉这个文件的依赖，让他不被编译。
(3)出错3：#include<regs.h>注释掉，然后添加#include <s5pc110.h>


2.12.19.SD卡驱动移植3
2.12.19.1、解决每次编译时间都很长的问题。
(1)每次编译脚本cp.sh执行时都会先cp同步代码，然后make distclean···所以每次都会清空后从头编译，这就很费时间了。
(2)但是实际上有时候是不会make distclean的，只需要先cp然后直接make即可
	（当更改没有涉及到配置头文件s5p_goni.h，没有涉及到makefile文件，或者其他项目配置文件，也就是说我们的更改只是普通代码文件的更改时）

2.12.19.2、效果测试
(1)读写测试均成功


2.12.20.环境变量的移植
2.12.20.1、iNand分区表检查-env究竟应该放在哪
(1)测试环境变量是否可以保存，通过开机set设置环境变量然后save，然后关机后重启来测试环境变量的保存是否成功。
(2)我们的环境变量究竟保存到哪里去了？这个就要去分析代码中的分区表。
(3)环境变量应该被放在哪里？虽然无法确定ENV一定要放在哪里，但是有一些地方肯定是不能放的，否则将来会出问题。
	原则是同一个SD卡扇区只能放一种东西，不能叠加，否则就会被覆盖掉。
	uboot烧录时使用的扇区数是：SD2的扇区1-16和49-x（x-49大于等于uboot的大小）
(3)从uboot的烧录情况来看，SD2的扇区0空闲，扇区1-16被uboot的BL1占用，扇区17-48空闲，扇区49-x被uboot的BL2占用。
	再往后就是内核、rootfs等镜像的分区了。系统移植工程师可以根据kernel镜像大小、rootfs大小等来自由给SD分区。
(4)从uboot的分区情况来看，ENV不能往扇区1-16或者49-x中来放置，其他地方都可以商量。ENV的大小是16K字节也就是32个扇区。

2.12.20.2、环境变量相关代码浏览
(1)目前情况是uboot在SD2中，而ENV在SD0中，所以现在ENV不管放在哪个扇区都能工作，不会有问题。
	但是我们还是得找到ENV分区所在并且改到不会和uboot冲突，因为将来部署系统时我们会将uboot和kernel、rootfs等都烧录到iNnand中去，那时候也要确保不会冲突。
(2)static inline int write_env(struct mmc *mmc, unsigned long size,
			    unsigned long offset, const void *buffer)
类似于这种函数，在代码分析中，关键是弄明白各种参数的意义。mmc表示要写的mmc设备，size表示要写的大小，offset表示要写到SD卡的哪个扇区去，buffer是要写的内容。
(3)CONFIG_ENV_OFFSET这个宏决定了我们的ENV在SD卡中相对SD卡扇区0的偏移量，也就是ENV写到SD卡的哪里去了。经过分析发现这个宏的值为0.所以我们的ENV
被写到了0扇区开始的32个扇区中。
(4)写到这里肯定不行，因为和uboot的BL1冲突了。解决方案是改变这个CONFIG_ENV_OFFSET的值，将ENV写到别的空闲扇区去。
(5)#define MOVI_BL2_POS		((eFUSE_SIZE / MOVI_BLKSIZE) + MOVI_BL1_BLKCNT + MOVI_ENV_BLKCNT)		后面这三个其实分别是1+16+32=49
其中的1就是扇区0（空闲的），16是就是扇区1-16（uboot的BL1），32就是扇区17-48（存放ENV的），49自然就是uboot的BL2开始扇区了。这种安排是三星移植的uboot版本中推荐的SD卡的分区方式，不一定是唯一的。
(6)我们参考这个设计，即可实现环境变量不冲突。所以只要将ENV放到17扇区起始的地方即可。


2.12.21.环境变量的测试和配置移植
2.12.21.1、如何测试环境变量的保存是否正确
(1)程序修改重新编译后启动，启动后要注意iNand中本来有没有环境变量。为了保险起见对iNand的前49个扇区进行擦除，然后就可以确保里面没有之前保存过的环境变量了。使用命令：mmc write 0 30000000 0# 49来擦除SD0的扇区0-48，保证以前的环境变量都没有了。
(2)重新开机后先set随便改一个环境变量作为标记然后saveenv然后重启。
(3)测试方法是，使用：mmc read 0 30000000 17# 32命令将iNand的17开始的32个扇区读出来到内存30000000处，然后md查看。找到显示区域里面的各个环境变量，看读出来的和自己刚才修改的值是否一样。

2.12.21.2、常用环境变量的配置移植
(1)常用的环境变量就是网络相关的那几个，和CONFIG_BOOTCOMMAND、CONFIG_BOOTARGS等。


2.12.22.网卡驱动的移植1
2.12.22.1、添加网络支持
(1)uboot中对各种功能也是一个条件编译可以配置可以裁剪的设计（从linux内核学来的），默认情况下我们的uboot没有选择支持网络。
(2)在配置头文件中添加一行 #define CONFIG_CMD_NET
(3)添加了网络支持宏之后，在uboot初始化时就会执行eth_initialize函数，从而网络相关代码初始化就会被执行，将来网络就有可能能用。

2.12.22.2、添加ping和tftp命令
(1)在linux系统中网络底层驱动被上层应用调用的接口是socket，是一个典型的分层结构，底层和上层是完全被socket接口隔离的。
(2)但是在uboot中网络底层驱动和上层应用是黏在一起的，不分层。意思就是上层网络的每一个应用都是自己去调用底层驱动中的操作硬件的代码来实现的。
(3)uboot中有很多预先设计的需要用到网络的命令，和我们直接相关的就是ping和tftp这两个命令。这两个命令在uboot中也是需要用相应的宏开关来打开或者关闭的。
(4)经过代码检查，发现ping命令开关宏为CONFIG_CMD_PING，而tftp命令的开关为CONFIG_CMD_NET，确认添加。

2.12.22.3、代码实践。结果是ping和tftp命令都被识别了，但是都提示no ethernet found`````网络不通。为什么不通？因为还没做初始化等移植

2.12.22.4、移植网卡初始化代码


2.12.23.网卡驱动的移植2
2.12.23.1、实验现象分析
(1)因为我们没有自定义的网卡初始化函数（board_eth_init或者cpu_eth_init），所以uboot启动时初始化网卡时打印：Net:   Net Initialization Skipped
(2)eth.c中有2个很重要的全局变量：eth_devices（用来指向一个链表，这个链表中保存了当前系统中所有的网卡信息）和eth_current（eth_current指针指向当前我们正在操作的那个网卡）。
(3)在linux的网卡驱动体系中，有一个数据结构（struct eth_device）用来表示（封装）一个网卡的所有信息，系统中注册一个网卡时就是要建立一个这个结构体的实例，然后填充这个实例中的各个元素，最后将这个结构体实例加入到eth_devices这个链表上，就完成了注册。了解了这些之后，你就明白了网卡驱动在初始化时必须负责将自己注册到系统的网卡驱动体系中（其实就是把自己的eth_device结构体实例添加到eth_devices链表中）。如果你不做这个过程就会出现：网卡找不到的错误。
(4)分析当前的问题是：在305行判断eth_devices是否为NULL之前没有去做网卡驱动的注册，所以这里为NULL，所以打印出了“No ethernet found.”

2.12.23.2、DM9000驱动浏览
(1)想解决这个问题，就是要在305行之前去注册网卡驱动。注册网卡驱动的代码不能随便乱写，一定要遵守linux网卡驱动架构的要求。这一块的代码一般属于网卡驱动的一部分，像这里就在dm9000x.c中。
(2)dm9000x.c中的最后一个函数int dm9000_initialize(bd_t *bis)，这个函数就是用来注册dm9000网卡驱动的。

2.12.24.3、问题修复
(1)根据之前分析uboot函数，发现前面有2个函数预留的可以用来放网卡初始化函数的，经过对比感觉board_eth_init函数稍微合适点，于是乎去添加。
2.12.24.4、对比和总结












2.12.23.uboot启动内核的移植












