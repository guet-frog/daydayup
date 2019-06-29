
2.20.1.2、 linux+QT bsp	
	(1) uboot、xboot		// bootloader
	(2) kernel
	(3) buildroot			// rootfs
	(4) tool
	(5) mk					// mk脚本是用来管理和编译整个bsp

2.20.1.3、mk的帮助信息		// mk -h, mk --help

2.20.2.mk文件分析

2.20.3.1、buildroot作用介绍
	(2) 交叉编译工具链arm-linux-gcc，我们目前都是从soc官方直接拿来使用的，官方的工具链从何而来？
		实际上交叉编译工具链都是由gcc配置编译生成的，这个配置编译过程比较复杂，
	(3) buildroot就是一个集成包，这个包里集成了交叉编译工具链的制作，以及整个rootfs的配置编译过程。
		也就是说，使用buildroot可以很简便的得到一个做好的文件夹形式的根文件系统。
	(4) buildroot将很多东西集成进来后，移植了linux kernel的make xxx_defconfig+make menuconfig的2步配置法

2.20.3.2、make x210ii_defconfig
	(1) 先make xxx_defconfig
	(2) make menuconfig
	(3) make				// buildroot/output/images/rootfs.tar



