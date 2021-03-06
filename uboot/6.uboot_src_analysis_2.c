
2.6.1.3、宏观分析：uboot第二阶段应该做什么
	(1)uboot step1: 主要初始化clock, ddr, 重定位(device copy function)
	(2)uboot step2: init SoC板级硬件(iNand、网卡芯片等), uboot本身的一些东西(uboot的命令、环境变量等)
					最终初始化完成后进入uboot的命令行准备接受命令

2.6.1.4、思考：uboot第二阶段完结于何处
	(2)如果用户没有干涉则会执行bootcmd进入自动启动内核流程, uboot执行完毕
	(3)uboot的命令行是一个死循环, 循环体内不断重复: 接收命令, 解析命令, 执行命令

2.6.2.2、DECLARE_GLOBAL_DATA_PTR   			// 结构体指针会不会重复定义? -- 这是一个declare
	(1)#define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("r8")	//gd: global data

2.6.3.2、内存排布
	(1)uboot code	// | 0xC3E0_0000	// #define CONFIG_MEMORY_UPPER_CODE
	(2)reserved		// |
	(3)bd			// |
	(4)gd			// | addr
	(5)ENV			// |
	(6)堆区			// |
	(7)栈区			// ↓

	// (5) (6) (7) 调节顺序 -- board.c start_armboot() line530

	// CFG_UBOOT_BASE	0xC3E0_0000
	// CFG_UBOOT_SIZE	2*1024*1024

	// CFG_MALLOC_LEN	CFG_ENV_SIZE + 896*1024
	// CFG_ENV_SIZE		0x4000
	// CFG_STACK_SIZE	512*1024
	// sizeof(gd_t) = 36 sizeof(bd) = 44

	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t))	// bd实例化
												// 结构体变量首地址在低地址, bd->bi_baudrate在低地址

	//compile optimization barrier: 内存间隔, 为了防止高版本的gcc的优化造成错误

======================================================================================================
======================================================================================================

2.6.4.1、init_sequence 与 init_fnc_t

	(1)typedef int (init_fnc_t) (void)		// 函数类型  -- init_fnc_t *init_sequence[] = {}
	(2)init_fnc_t **init_fnc_ptr			// 二重指针  -> 指针数组
											// 干掉一个 * -- init_fnc_t *init_sequence[]
	// 二重函数指针: ①指向一重指针, ②指向指针数组 -- 指向数组
	// 一般将二重指针理解为指针数组

	(1)init_fnc_t *init_sequence[]	//函数指针数组 -- init_fnc_t *
	(3)init_fnc_t **init_fnc_ptr	//二重函数指针 -- init_fnc_t **		init_fnc_ptr 指向 init_sequence	

	// array_name not &array_name
	// init_fnc_ptr++ (+4) 		-- 二重指针类型自增

2.6.4.2、board_init
	(1)eth init: config controller's gpio, controller control regs
	(2)gd->bd->bi_arch_number		// board mac id
	(3)gd->bd->bi_boot_params

	// PC机设备标准化, 嵌入式设备定制化软硬件可裁剪
	// 强制类型转换给内存地址赋值  	指针 -> 变量(内存)
	// 实例化: 		 				类   -> 对象
	// 控制台是基于串口, 控制台可以实现标准输入输出
	// hang()/*挂起*/

2.6.4.3、dram_init	//	ddr的配置
	(1)硬件初始化: ddr可以正常工作	// lowlevel_init

	(2)软件初始化: 软件架构中一些ddr相关的属性配置, 地址设置
		// uboot&&linux如何获取当前系统内存相关信息, 如: 内存数量, 内存总容量, 起始地址等
		①: x210_sd.h中用宏定义指明, 然后uboot直接使用这些信息
		②: 类似于PC的BIOS通过读取硬件信息来确定
		
		// 对于特定的开发板，其内存分布是明确的，所以可以直接设置

2.6.4.4、interrupt_init
		(1)TIM4 init
			read_reg -> r,m,p,s -> get_pllclk() -> get_hclk -> get_pclk() -> timer_load_val

2.6.6.2、env_init
	(2)这个函数只是对/*内存*/里维护的那一份uboot的env做了基本的初始化
	   当前env没有从SD卡到DDR中的relocate/*only relocate firmware -- not relocate env.bin */
	(4)在start_armboot函数中(line776)调用env_relocate(), 进行env从SD卡中到DDR中的重定位

		重定位之后需要环境变量时才可以从DDR中去取, 重定位之前如果要使用环境变量只能从SD卡中去读取 -- // 这句话应该有问题?

2.6.7.1、init_baudrate
	(2)getenv_r函数用来读取环境变量的值	// 读取到的为string类型而不是int类型 -- simple_strtoul() -- // ?

2.6.8.1、console_init_f
	(1)console_init_f是console(控制台)的第一阶段初始化, _f表示是第一阶段初始化, _r表示第二阶段初始化
	//有时候初始化函数不能一次完成, 中间必须要夹杂一些代码 -- line826 console_init_r

2.6.8.2、display_banner		// 输出U_BOOT_VERSION
	(1) ./common/console.c/printf -- CR600 tool.c/printf实现
	(2) printf() -> puts()
	(4) 控制台就是一个用软件虚拟出来的设备, 这个设备有一套专用的通信函数(发送、接收...)
		控制台的通信函数/*最终会映射到硬件的通信函数中来实现*/
		uboot中实际上控制台的通信函数是直接映射到硬件串口的通信函数中
	(5)	但是在别的体系中，控制台的通信函数映射到硬件通信函数时可以用软件来做一些中间优化，譬如说缓冲机制
		（操作系统中的控制台都使用了缓冲机制，所以有时候我们printf了内容但是屏幕上并没有看到输出信息，
		就是因为被缓冲了。我们输出的信息只是到了console的buffer中，buffer还没有被刷新到硬件输出设备上，
		尤其是在输出设备是LCD屏幕时）

// cpu被串口拖慢 添加缓冲机制

2.6.8.3、print_cpuinfo	// cpu clock && usart message

2.6.9.3、uboot学习实践
	(1)对uboot源代码进行完修改
	(2)make distclean然后make x210_sd_config然后make
	(3)编译完成得到u-boot.bin，然后去烧录。烧录方法按照裸机第三部分讲的linux下使用dd命令来烧写的方法来烧写
	(4)烧写过程：
		第一步：进入sd_fusing目录下		// file mkbl1  ,,,x64
		第二步：make clean
		第三步：make
		第四步：插入sd卡，ls /dev/sd*得到SD卡在ubuntu中的设备号(一般是/dev/sdb)
		第五步：./sd_fusing.sh /dev/sdb完成烧录(注意不是sd_fusing2.sh)

	// bl1_position=1
	// uboot_position=49	// video start_armboot解析7 15′

	// bl2_position=9
	// uboot_position=57

2.6.10.1、dram_init
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;		// 记录当前开发板ddr配置信息
	gd->bd->bi_dram[0].size  = PHYS_SDRAM_1_SIZE; 

2.6.10.2、display_dram_config	// print dram config info

	//	NandFlash: nand		NorFlash: flash

======================================================================================================
======================================================================================================

2.6.11.2、CONFIG_VFD、CONFIG_LCD
		uboot中自带的LCD显示的软件框架, 本次移植自己添加LCD显示相关代码

2.6.11.3、mem_malloc_init	// init uboot的堆管理器(用来维护堆内存的一些代码) -- uboot heap size: 896KB

2.6.12.1、开发板独有初始化：mmc初始化		// ./drivers/mmc/mmc.c	-- linux src drivers
	(1)mmc_initialize(): 抽象出的mmc初始化函数(与具体硬件无关), 内部调用具体硬件初始化函数
	(2)uboot本身没有单独实现驱动, 借用linux驱动架构实现(mtd驱动架构)	// ./drivers is from linux src
	(3)uboot是裸机程序使用物理地址, linux使用虚拟地址, 理论上uboot不能使用linux中的驱动

	// int cur_dev_num = -1
	// struct list_head  mmc_devices
	// struct sdhci_host mmc_host[MMC_MAX_CHANNEL]
	// struct mmc 	     mmc_channel[MMC_MAX_CHANNEL]

	-- mmc_initialize		// uboot/drivers/mmc/mmc.c
	 |
	 | -- struct mmc *mmc
	 | -- INIT_LIST_HEAD(&mmc_devices)
	 |
	 | -- if (board_mmc_init() < 0)		// baord层面的sd卡控制器, 如: usb读卡器
		|
		| -- cpu_mmc_init()		// uboot/cpu/s5pc11x/cpu.c	-- soc内置sd/mmc controller init
		   |
		   | -- setup_hsmmc_clock()		// uboot/cpu/s5pc11x/setup_hsmmc.c
		   | -- setup_hsmmc_cfg_gpio()
		   |
		   | -- smdk_s3c_hsmmc_init()	// uboot/drivers/mmc/s3c_hsmmc.c  -- 不同mmc支持的channel不同
			  |
			  | -- s3c_hsmmc_initialize(0)	// uboot/drivers/mmc/s3c_hsmmc.c
				 |
				 | -- struct mmc *mmc;
				 | -- mmc = &mmc_channel[channel]	// 实例化
				 |
				 | -- ... 成员变量、成员方法初始化
				 |
				 | -- mmc_register(mmc)		// uboot/drivers/mmc/mmc.c -- mmc_channel[channel]内存注册到mmc_devices链表
				    |
					| -- INIT_LIST_HEAD(&mmc->link)
					|
					| -- list_add_tail(&mmc->link, &mmc_devices)
	 |
	 | -- find_mmc_device(dev_num)	// uboot/drivers/mmc/mmc.c
		|
		| -- list_for_each(entry, &mmc_devices)		// iterate over list
		   |
		   | -- m = list_entry(entry, struct mmc, link)
		   |
		   | -- if (m->block_dev.dev == dev_num)
			  |
			  | -- return m
	 |
	 | -- mmc_init(m)	// uboot/drivers/mmc/mmc.c	-- send cmd(init mmc card NOT mmc card controller, mmc card software struct)

2.6.13.1、env_relocate		// env is embeded in text segment  -- warning: text segment will copy to ddr

	(2) flash中有8个独立的扇区作为环境变量存储区域(4KB)
		烧录/部署系统时, 烧录(uboot、kernel、rootfs), 没有烧录env分区
		第一次启动时flash中env分区为空, 加载flash中env失败, uboot加载内部default env

		// flash中env分区是如何更新 	-- saveenv or auto?	 -- save

	-- env_relocate()	// 环境变量重定位到ddr中
	  |
	  | -- env_ptr = (env_t *)malloc(CFG_ENV_SIZE)
	  |
	  | -- env_relocate_spec()
		 |
		 | -- env_relocate_spec_movinand()
		    |
			| -- movi_read_env(virt_to_phys(env_ptr))	// assign value env_ptr
			|
			| -- if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)	// crc exe error
			   |
		       | -- use_default()
	  |
	  | -- gd->env_addr = (uint32_t)&(env_ptr->data)		// 索引env -- gd->env_addr

2.6.14.2、devices_init		// linux kernel启动的devices_init
	// 集中执行各种硬件设备的驱动的init函数

2.6.14.3、jumptable_init	// no use
	// C语言是非面向对象的，但是C语言编写的Linux内核是面向对象的

2.6.15.1、console_init_r
	(1)console_init_f是控制台的第一阶段初始化，console_init_r是第二阶段初始化
		实际上第一阶段初始化并没有实质性工作，第二阶段初始化才进行了实质性工作
	(3)console_init_r就是console的纯软件架构方面的初始化
		说白了就是去给console相关的数据结构中填充相应的值，所以属于纯软件配置类型的初始化
	(4)uboot的console实际上并没有干有意义的转化，它就是直接调用的串口通信的函数
		所以用不用console实际并没有什么分别
		在linux内console就可以提供缓冲机制等不用console不能实现的东西

2.6.15.2、enable_interrupts		// no use, uboot not use irq

2.6.15.3、loadaddr、bootfile两个环境变量
	// 这两个环境变量都是内核启动有关的，在启动linux内核时会参考这两个环境变量的值。

2.6.15.4、board_late_init	// no use

2.6.16.1、eth_initialize
	(1)看名字应该是网卡相关的初始化。这里不是SoC与网卡芯片连接时SoC这边的初始化，而是网卡芯片本身的一些初始化
	(2)对于X210（DM9000）来说，这个函数是空的。X210的网卡初始化在board_init函数中，网卡芯片的初始化在驱动中

	// IDE接口硬盘

2.6.16.2、x210_preboot_init(LCD和logo显示)	//start_armboot解析14

2.6.16.3、check menu key to update from sd		// SD量产卡

2.6.16.4、main_loop

======================================================================================================
======================================================================================================

(1)第二阶段主要是对开发板级别的硬件、软件数据结构进行初始化。

	init_sequence[]
		|
		| -- cpu_init()	// if necessary: 设置IRQ FIQ stack	-- msp or psp
		|
		| -- board_init()	// 网卡, 机器码, 内存传参地址
		   |
		   | -- dm9000_pre_init()	// SROM controller reg init
		   | -- gd->bd->bi_arch_number
		   | -- gd->bd->bi_boot_params	// memory_base + 0x100
	    |
		| -- interrupt_init()  // init timer4 -- auto_load, 10ms, no_interrupt, turn_on
		|
		| -- env_init()
		   |
		   | -- gd->env_addr = (uint32_t)&default_environment[0]
		   | -- gd->env_valid = 1
		|
		| -- init_baudrate()
		   |
		   | -- gd->bd->bi_baudrate = gd->baudrate = CONFIG_BAUDRATE
		|
		| -- serial_init()	// nothing
		|
		| -- console_init_f()
		   |
		   | -- gd->have_console = 1
		|
		| -- display_banner()
		   |
		   | -- printf("%s\n", version_string)		// _DATE_ _TIME_
		|
		| -- print_cpuinfo()	// speed.c
		|
		| -- checkboard() 	// printf x210
		|
		| -- dram_init()	// gd struct init -- ddr init in asm_code
		   |
		   | -- gd->bd->bi_dram[0].start = PHY_SDRAM_1			// 0x3000_0000
		   | -- gd->bd->bi_dram[0].size  = PHYS_SDRAM_1_SIZE	// 0x1000_0000
		   | -- gd->bd->bi_dram[1].start = PHYS_SDRAM_2			// 0x4000_0000
		   | -- gd->bd->bi_dram[1].size  = PHYS_SDRAM_2_SIZE	// 0x1000_0000
		|
		| -- display_dram_config()	// printf ddr config_info(size)

2.6.17.2、启动过程特征总结
	(1)第一阶段为汇编阶段、第二阶段为C阶段
	(2)第一阶段在SRAM中、第二阶段在DRAM中
	(3)第一阶段注重SoC内部、第二阶段注重SoC外部Board内部

2.6.17.3、移植时的注意点
	(1)x210_sd.h头文件中的宏定义
	(2)特定硬件的初始化函数位置（譬如网卡）




















