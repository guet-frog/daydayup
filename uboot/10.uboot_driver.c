
2.10.1.1、裸机uboot与驱动
	(1)裸机本来是没有驱动的概念(狭义的驱动: 操作系统中用来具体操控硬件的代码)
	(2)裸机程序中是直接操控硬件的, 操作系统中必须通过驱动来操控硬件	//二者的本质区别: 分层
	(3)OS下驱动中使用的是虚拟地址, 裸机中操控硬件使用物理地址	// OS和裸机下操控硬件的区别
	(4)uboot中的虚拟地址映射表, 只有 0x30000000-0x3FFFFFFF 映射到 0xC0000000-0xCFFFFFFF, 其余的虚拟地址空间全是原样映射的
	   驱动主要是操控硬件寄存器, S5PV210的SFR都在0xExxxxxx地址空间, 因此驱动中不必考虑虚拟地址

2.10.1.3、uboot借用（移植）了linux驱动
	(1)linux驱动本身做了模块化设计(与内核分离), uboot可以从源代码级别去移植linux驱动
	(2)linux驱动本身有更复杂的框架, 需要实现更多的附带功能	// 如Linux下的/sys /proc虚拟文件目录的实现

2.10.2.2、mmc_initialize	// inand/sd驱动 -- start_armboot()	-- uboot/drivers/mmc/mmc.c
	(1)soc内部与mmc相关的gpio初始化		//soc的mmc controller对内体现为regs, 对外体现为gpio		-- 外设通过相关gpio连接到soc
	(2)soc内部mmc controller init(mmc系统时钟初始化, sfr初始化)
	(3)sd卡/inand芯片初始化		// 外设复位
		// sd卡内部有主控单元(可自行完成flash读写相关操作), soc初始化sd卡时, 首先发送sd init命令给sd卡

	(4)mmc_devices链表全局变量, 用来记录系统中所有已经注册的SD/iNand设备
		所以向系统中插入一个SD卡/iNand设备, 则系统驱动就会向mmc_devices链表中插入一个数据结构表示这个设备

2.10.2.3、cpu_mmc_init		// uboot/cpu/s5pc11x/cpu.c
	(1)setup_hsmmc_clock	//uboot/cpu/s5pc11x/setup_hsmmc.c
		初始化SoC中MMC控制器中的时钟部分的
		// s5pc210.pdf -- system/clock/register/clock_src_4
		// SOC mmc控制器 和 mmc卡 时钟都不能超过50M
		// 开发板inand在mmc0通道, SD卡在mmc2和mmc3, 因为需要使用到sd启动, 所以mmc2
	(2)setup_hsmmc_cfg_gpio	//uboot/cpu/s5pc11x/setup_hsmmc.c
		配置SoC中MMC控制器相关的GPIO
		// 1bit 4bit 8bit

2.10.3.1、smdk_s3c_hsmmc_init		//uboot/drivers/mmc/s3c_hsmmc.c
	(2)函数内部通过宏定义USE_MMCx来决定是否调用s3c_hsmmc_initialize来进行具体的初始化操作。

2.10.3.2、s3c_hsmmc_initialize		//uboot/drivers/mmc/s3c_hsmmc.c
	(2)定义并且/*实例化*/一个struct mmc类型的对象, 然后将成员变量赋值, 最后调用mmc_register函数来向驱动框架注册这个mmc设备驱动
		// mmc的所有信息和操作方法都封装到struct mmc中
	(3)mmc_register功能是进行mmc设备的注册，注册方法其实就是将当前这个struct mmc使用链表连接到mmc_devices这个全局变量中去
		// list_add_tail() // 挂载到系统内核链表中
	(4)在X210中定义了USE_MMC0和USE_MMC2, 因此uboot初始化时会调用2次s3c_hsmmc_initialize函数, 传递参数分别是0和2
		因此完成之后系统中会注册上2个mmc设备, 表示当前系统中有2个mmc通道在工作

2.10.3.3、find_mmc_device			//uboot/drivers/mmc/mmc.c
	(2)通过mmc设备编号来在系统中查找对应的mmc设备（struct mmc的对象，根据上面分析系统中有2个，编号分别是0和2）。
	(3)通过遍历mmc_devices链表，去依次寻找系统中注册的mmc设备，然后对比其设备编号和我们当前要查找的设备编号，如果相同则就找到了要找的设备。
		找到了后调用mmc_init函数来初始化它。

2.10.3.4、mmc_init					//drivers/mmc/mmc.c
	(2)分析猜测这个函数应该要进行mmc卡的初始化了（前面已经进行了SoC端控制器的初始化）
	(3)函数的调用关系为：
	mmc_init
		mmc_go_idle
			mmc_send_cmd
		mmc_send_if_cond
			mmc_send_cmd
		······
	具体分析可以看出，mmc_init函数内部就是依次通过向mmc卡发送命令码（CMD0、CMD2那些）来初始化SD卡/iNand内部的控制器，以达到初始化SD卡的目的。

2.10.3.3、总结
	(1)至此整个MMC系统初始化结束。
	(2)整个MMC系统初始化分为2大部分：SoC这一端的MMC控制器的初始化，SD卡这一端卡本身的初始化。
		前一步主要是在cpu_mmc_init函数中完成，后一部分主要是在mmc_init函数中完成。
	(3)整个初始化完成后去使用sd卡/iNand时，操作方法和mmc_init函数中初始化SD卡的操作一样的方式。
		读写sd卡时也是通过总线向SD卡发送命令、读取/写入数据来完成的。
	(4)顺着操作追下去，到了mmc_send_cmd函数处就断了，真正的向SD卡发送命令的硬件操作的函数找不到。这就是学习驱动的麻烦之处。
	(5)struct mmc结构体是关键。两部分初始化之间用mmc结构体来链接的，初始化完了后对mmc卡的常规读写操作也是通过mmc结构体来链接的。

2.10.4.1、struct mmc
	(1)驱动的设计中有一个关键数据结构
		譬如MMC驱动的结构体就是struct mmc这些结构体中包含一些变量和一些函数指针，
		变量用来记录驱动相关的一些属性，函数指针用来记录驱动相关的操作方法。这些变量和函数指针加起来就构成了驱动
		驱动就被抽象为这个结构体。
	(2)一个驱动工作时主要就分几部分：驱动构建（构建一个struct mmc然后填充它）（驱动初始化）、驱动运行时（调用这些函数指针指针的函数和变量）

2.10.4.2、分离思想
	(1)分离思想就是说在驱动中将操作方法和数据分开
	(2)操作方法就是函数，数据就是变量。所谓操作方法和数据分离的意思就是：在不同的地方来存储和管理驱动的操作方法和变量，这样的优势就是驱动便于移植
	// 很多时候进行驱动移植, 驱动代码不需要任何改动, 也即是操作方法不需要改动, 仅变量需要改动
	// 这里的方法应该理解为设备驱动函数初始化注册不需要变动, 变量改动理解为如: 函数指针变量改动

	// Linux设备树方式传参

2.10.4.3、分层思想
	(1)分层思想是指一个整个的驱动分为好多个层次。简单理解就是驱动分为很多个源文件，放在很多个文件夹中。
		譬如本课程讲的mmc的驱动涉及到drivers/mmc下面的2个文件和cpu/s5pc11x下的好几个文件。

	(2)以mmc驱动为例来分析各个文件的作用：
		uboot/drivers/mmc/mmc.c：本文件的主要内容是和MMC卡操作有关的方法，譬如MMC卡设置空闲状态的、卡读写数据等。
			/*但是本文件中并没有具体的硬件操作函数*/，操作最终指向的是struct mmc结构体中的函数指针，
			这些函数指针是在驱动构建的时候和真正硬件操作的函数挂接的（真正的硬件操作的函数在别的文件中）。
		uboot/drivers/mmc/s3c_hsmmc.c:本文件中是SoC内部MMC控制器的硬件操作的方法，譬如向SD卡发送命令的函数（s3c_hsmmc_send_command），
			譬如和SD卡读写数据的函数（s3c_hsmmc_set_ios），这些函数就是具体操作硬件的函数，也就是mmc.c中需要的那些硬件操作函数。
			这些函数在mmc驱动初始化构建时（s3c_hsmmc_initialize函数中）和struct mmc挂接起来备用。

		分析：mmc.c和s3c_hsmmc.c构成了一个分层，mmc.c中调用了s3c_hsmmc.中的函数，
			所以mmc.c在上层，s3c_hsmmc.c在下层。
			这两个分层后我们发现mmc.c中不涉及具体硬件的操作，s3c_hsmmc.c中不涉及驱动工程时的时序操作。
			因此移植的时候就有好处：譬如我们要把这一套mmc驱动移植到别的SoC上mmc.c就不用动，s3c_hsmmc.c动就可以了；
			譬如SoC没变但是SD卡升级了，这时候只需要更换mmc.c，不需要更换s3c_hsmmc.即可。

	(3)cpu/s5pc11x/下面还有一个setup_hsmmc.c，也和MMC驱动有关。但是这些代码为什么不能放到drivers目录下去，而要放到cpu目录下去？
			因为这里面的2个函数（setup_hsmmc_clock和setup_hsmmc_cfg_gpio）都是和SoC有关的初始化函数，这两个函数不能放到drivers目录下去。
			实际上如果非把这两个函数放在uboot/drivers/mmc/s3c_hsmmc.c文件中也凑活能说过去。

	// 将驱动设备抽象为结构体













