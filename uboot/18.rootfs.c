
2.18.1.1、为什么需要根文件系统
	(1) init进程的应用程序在根文件系统上
	(2) 根文件系统提供了根目录/
	(3) 内核启动后的应用层配置(etc目录)在根文件系统上	// 发行版=内核+rootfs
	(4) shell命令程序在根文件系统上。譬如ls、cd等命令
		总结：一套linux体系，只有内核本身不能使用，
			必须要rootfs(包括etc目录下的配置文件、/bin  /sbin等目录下的shell命令，还有/lib目录下的库文件等)相配合才能工作

// which ls
// file /bin/ls
// file /bin/ping		-- shell下命令对应一个应用程序

2.18.1.2、根文件系统的实质是什么
	(1) 根文件系统是特殊用途的文件系统
	(2) 根文件系统也必须属于某种文件系统格式。rootfstype=

2.18.2.1、镜像文件形式	// rootfs_qt4.ext3
	(1) 使用工具制作的可供烧录的镜像文件，镜像中包含了根文件系统中的所有文件
		烧录此镜像类似于对相应分区格式化
	(4) 镜像文件系统具有一定的格式，格式是内化的，跟文件名后缀无关
	
		// 烧录，内核启动挂载

2.18.2.2、文件夹形式		// 不存在格式，格式是由制作镜像工具确定
	(1) 包含特定内容的文件夹

		// nfs方式挂载

2.18.3.1、mke2fs介绍
	(1) mke2fs是一个应用程序，在ubuntu中默认是安装了的。这个应用程序就是用来
		制作ext2、ext3、ext4等格式的根文件系统的
	(2) 一般用来制作各种不同格式的rootfs的应用程序的名字都很相似，
		类似于mkfs.xxx（譬如用来制作ext2格式的rootfs的工具叫mkfs.ext2、用来
		制作jffs2格式的rootfs的工具就叫mkfs.jffs2）
	(3) ubuntu14.04 中的mkfs.ext2等都是mke2fs的符号链接而已。

2.18.3.2、动手制作ext3格式的根文件系统
	(1) 创建rootfs.ext2文件并挂载到某一个目录下测试访问
		// http://blog.csdn.net/zhengmeifu/article/details/24174513
		dd if=/dev/zero of=rootfs.ext2 bs=1024 count=20480
		losetup  /dev/loop1 rootfs.ext2
		mke2fs -m 0 /dev/loop1 20480
		mount -t ext2 /dev/loop1 ./rootfs/

		umount /dev/loop1		// 卸载
		losetup -d /dev/loop1

2.18.4.2、配置内核以支持nfs作为rootfs
	(1) bootloader中设置nfs启动方式的bootargs

	(2) 在menuconfig中配置支持nfs启动方式(参考视频文件夹下的文档)
		// setenv bootargs root=/dev/nfs nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC2,115200

		本地测试nfs服务是否开启
		// sudo mount -t nfs -o nolock 192.168.0.88:/home/aston/src/rootfs/rootfs /opt
		// umount /opt

2.18.5.到底什么是linuxrc
		/linuxrc是一个可执行的应用程序和内核无关，由arm-linux-gcc编译得到，而且
		一般由静态链接得到

2.18.5.3、/linuxrc负责系统启动后的配置		// etc：execute time config
	(1) 就好像一个房子建好之后不能直接住，还要装修一样；操作系统启动起来后也不能直接用，要配置下。
	(2) 操作系统启动后的应用层的配置（一般叫运行时配置，英文简写etc）
		是为了让我们的操作系统用起来更方便，更适合我个人的爱好或者实用性。

2.18.5.4、/linuxrc在嵌入式linux中一般就是busybox
	(1) busybox是一个C语言写出来的项目，里面包含了很多.c文件和.h文件。
		这个项目可以被配置编译成各个平台下面可以运行的应用程序。
		我们如果用arm-linux-gcc来编译busybox就会得到一个可以在我们
		开发板linux内核上运行的应用程序。
	(2) busybox这个程序开发出来就是为了在嵌入式环境下构建rootfs使用的，
		也就是说他就是专门开发的init进程应用程序。
	(3) busybox为当前系统提供了一整套的shell命令程序集。譬如vi、cd、mkdir、ls等。
		在桌面版的linux发行版（譬如ubuntu、redhat、centOS等）中vi、cd、ls等都是
		一个一个的单独的应用程序。
		但是在嵌入式linux中，为了省事我们把vi、cd等所有常用的shell命令集合到一起
		构成了一个shell命令包，起名叫busybox。

2.18.6.rootfs中还应该有什么					// /etc 运行时配置文件
	(1) /linuxrc
	(2)	/dev/		// 有部分设备文件，rootfs是必须的
	(3) /sys /proc
	(4) /usr
	(5) /etc
	(6) /lib		// 动态和静态链接库

2.18.7.VFS简介
2.18.7.1、什么是VFS
	(1) VFS是linux内核的一种设计理念、设计机制。VFS就是vitrual file system，叫虚拟文件系统。
	(2) 具体的一些文件系统如FAT、NTFS、ext2、ext3、jffs2、yaffs2、ubi等
		主要设计目的是为了管理块设备（硬盘、Nand···）
	(3) VFS是借鉴了文件系统的设计理念（通过文件系统将底层难以管理的
		物理磁盘扇区式访问，转换成目录+文件名的方式来访问）
		将硬件设备的访问也虚拟化成了对目录+文件的访问。
		所以有了VFS后我们可以通过设备文件（目录+文件名，譬如/dev/mmcblk0p2）
		的方式来访问系统中的硬件设备。

2.18.7.2、VFS的意义
	(1) 上面说过的对硬件设备的访问，将对硬件设备的访问和对普通文件的访问
		给接口统一化了（linux中一切届是文件）。
	(2) 将操作系统上层（应用层）对下层不同文件系统类型的访问细节给屏蔽掉了。
		因此如果没有VFS那我们写cp命令（其他命令也一样）的时候就不得不去
		考虑你cp的这个文件在什么文件系统类型下。
		所以cp命令非常复杂，因此要考虑具体的文件系统类型。有了VFS后情况就不同了。
		VFS成了一个隔离层，隔离了下层的不同文件系统的差异性，对上层应用提供一个统一的接口。
	(3) VFS将不同文件系统和下层硬件设备（块设备）驱动之间的细节也给屏蔽了。
		不同类型的文件系统在本身设计时是不用考虑各种不同的硬件设备的具体操作差异的，
		这里有一个类似于VFS的设计理念。

2.18.7.3、VFS和我们学习的关系
	(1) VFS机制和rootfs挂载，和其他文件系统的挂载都是有关联的
	(2) 内核中有一些sys proc这种虚拟文件系统，这东西也是和VFS机制有关
	(3) /dev/目录下的设备文件都和VFS有关，所以学习驱动绕不开VFS

// 配置和重启nfs服务
sudo vi /etc/exports
sudo /etc/init.d/nfs-kernel-server restart

// bootargs
setenv bootargs root=/dev/nfs nfsroot=192.168.0.88:/home/aston/src/rootfs/rootfs ip=192.168.0.66:192.168.0.88:192.168.0.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC2,115200 
setenv bootargs root=/dev/nfs nfsroot=192.168.0.88:/root/rootfs ip=192.168.0.66:192.168.0.88:192.168.0.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC2,115200 
setenv bootargs root=/dev/nfs nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off  init=/linuxrc console=ttySAC2,115200

// showmount localhost -e

setenv bootargs
root=/dev/nfs
nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off
init=/linuxrc console=ttySAC2,115200

set bootargs console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext2


