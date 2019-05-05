2.17.1.3、配置编译下载尝试
(1) Makefile: ARCH, CROSS_COMPILE
(2) make xx_defconfig				// arch/arm/configs/...
(3) make menuconfig
(4) make -j4

2.17.2.1、分析问题
(1) 现象：未出现Uncompressing Linux... done, booting the kernel.	// Linux自解压代码没有执行
	修改：arch/arm/mach-s5pv210/mach/Makefile.boot		// link addr

(2) 现象：kernel无打印信息
	修改：link addr

2.17.3.内核中机器码的确定
// mach-smdkc110.c --  line1245

2.17.3.2、硬件驱动的加载和初始化函数执行
(1).init_machine	= smdkc110_machine_init		// not smdkv210_machine_init when define CONFIG_MACH_SMDKV210

2.17.4.1、认识内核启动OOPS
(1) 电源管理IC
// CONFIG_MFD_MAX8698: 	控制drivers目录下的max8698对应的驱动程序源代码是否被编译		\
						控制kernel启动过程中是否会调用一些max8698的相关的代码
(2) 挂载rootfs
	// SD/iNand本身都是由一个一个的扇区组成
	// x210的启动时，BL1在SD卡的1扇区开始，SD卡的0扇区默认保留(MBR: 描述块设备的分区信息)  -- 分区表信息不需要设定
	// fdisk -c 0

	// nand分区表一般是在内核中用代码构建

(3) 网卡驱动
	① make menuconfig添加DM9000支持
	② mach-smdkc100.c -- smdkc110_machine_init		//整个开发板所有的硬件初始化
	③ 移植smdkc110_devices和smdkc110_dm9000_set()

2.17.6.4、修改相应的配置参数
	(1)DM9000相关的数据配置在arch/arm/plat-s5p/devs.c中更改
	(2)在arch/arm/mach-s5pv210/include/mach/map.h中定义了DM9000的IO基地址，和DM9000接在哪个bank有关。
	(3)还有+2改成+4，IRQ_EINT9改成10即可。
