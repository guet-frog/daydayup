
	// 串口做系统控制台 -- 超级终端、SecureCRT、minicom

1.3.4.0、破坏iNand中的bootloader
	step1: busybox dd if=/dev/zero of=/dev/block/mmcblk0 bs=512 seek=1 count=1 conv=sync
	step2: sync
	// 使用linux dd 命令读写磁盘
	// if: input file 	/dev/zero				// 特殊设备 对应全0    --  将0写入到inand中
	// of: output file  /dev/block/mmcblk0 		// 输出设备对应inand
	// bs: block size 扇区大小512字节
	// seek: 第一个扇区(开始)					// bootloader从扇区1开始
	// count: 扇区数量

	// first boot校验和不通过打印: SD checksum Error, 接下来执行second boot

1.3.4.1、制作启动SD卡（其实就是烧录uboot到SD卡中） // windows tool or linux dd

1.3.4.2、fastboot介绍
	fastboot是uboot中用来/*快速下载镜像*/的一个命令, fastboot同时还是一个windows上的软件
	//fastboot通过USB传输, fastboot安装驱动

1.3.4.3、fastboot驱动	//X210 Android驱动
	驱动位置：A盘\tools\USB驱动\x210_android_driver //连网自动安装

1.3.4.4、fastboot常用命令
	(1)首先在uboot串口终端中执行"fastboot"命令(windows设备管理器发现adb device)
	(2)在windows cmd中执行fastboot应用程序, 执行以下命令	// 如: fastboot devices

1.3.4.5、使用fastboot烧录android4.0.4镜像
	在cmd下使用以下三个命令来完成烧录
	fastboot flash bootloader android4.0/uboot.bin		//uboot
	fastboot flash kernel android4.0/zImage-android		//linux kernel
	fastboot flash system android4.0/x210.img			//android rom

	uboot的参数设置: set bootcmd 'movi read kernel 30008000; bootm 30008000'  // default param
	刷linux+QT镜像后, uboot的参数不用修改(刷的是linux+QT定制的uboot, 默认参数应该相应修改)

	// fastboot flash到inand中, 没有下载到SD卡中 -- write read写死channel0

1.3.9.3、x210的dnw刷机
	step1: 刷x210_usb.bin, 地址0xd0020010
	step2: 刷uboot.bin,    地址0x23e00000
	uboot启动起来后, ①fdisk -c 0 重新分区, ②fastboot		// fdisk -c 0

1.3.10.linux下使用dd命令刷写uboot

1.3.10.1、SD卡连接入linux中
	连接进linux中，在命令行中：ls /dev/sd*查看, 一般是/dev/sdb
	执行 ./nand_fusing.sh /dev/sdb

1.3.10.3、使用SD卡启动
	先确定开发板是从SD卡启动的
	然后确定板子内的iNand中的uboot是被破坏的，不能启动的
	SD卡插入SD2通道，启动即可。启动后(看有没必要fdisk -c 0来分区)使用fastboot继续刷机

======================================================================================
1.3.11.2、刷机常见问题及分析处理
(8)Windows不提示安装驱动。如果开发板中就在运行uboot或者linux+QT，这时候插上USB线主机Windows也不会发现开发板；
	当我们uboot下执行了fastboot命令后主机就会提示装fastboot驱动。当开发板运行了android系统后主机就会提示装fastboot驱动；
	当开发板设置了USB启动并且重启时就会提示装dnw驱动。

1.3.12.补充-linux下dnw软件的使用
1.3.12.1、linux下dnw获取
(1)源码包获取
(2)编译和安装

1.3.12.2、开发板连接到linux下
	(1)开发板设置USB启动，开机，windows设备管理器中提示发现新硬件
	(2)VMware菜单：虚拟机->可移动设备->Samsung S5PC110 SEC Test B/D，点击连接
	(3)成功连接后现象：1、Windows下设备管理器没了；2、ls /dev/secbulk0 设备节点自动出现

1.3.12.3、使用dnw进行下载uboot.bin
	(1)dnw -a 0xd0020010 x210_usb.bin 
	(2)dnw -a 0x23e00000 uboot.bin
	成功标志：SecureCRT中成功看到了uboot的启动信息，并且进入了uboot命令行。

1.3.12.4、uboot启动后的刷机注意事项
	(1)正确的uboot启动后一定要先fdisk -c 0进行分区，然后再fastboot
	(2)uboot的参数中bootcmd和bootargs一定要正确。
	set bootcmd 'movi read kernel 30008000; bootm 30008000'
	set bootargs console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3











