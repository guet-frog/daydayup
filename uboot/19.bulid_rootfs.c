
/etc/inittab		// 不断检测执行	-- linux启动时的inittab
/etc/init.d/rcS		// 运行时配置文件

2.19.2.1、busybox源码下载

2.19.2.2、修改Makefile
	(1) ARCH = arm
	(2) CROSS_COMPILE = /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

2.19.2.3、make menuconfig
	(1) busybox menuconfig配置.txt

2.19.2.4、make 然后 make install			// make 与 make install
	(1) 解决编译问题
	// find -name "sync.c"
	// find -name "*sync*"
	// make menuconfig 禁止 CONFIG_SYNC； 搜索SYNC

	(2) linux下源码安装软件
		make menuconfig			// 配置
		make -j4				// 编译
		make install			// 安装到默认目录或指定目录

	// make install 将编译生成的可执行程序及其依赖的库文件、配置文件、头文	\
		件安装到当前系统中默认目录或指定的目录下

2.19.3.2、inittab格式解析		// 	/etc/inittab运行时配置文件
	(1) inittab中的每一行配置项的值 id:runlevels:action:process
	(2) 只要进程1在运行，则/*不断*/检测action是否满足，是否需要执行process

	// 进程1 /linuxrc busybox	-- busybox最终进入一个死循环

2.19.4.3、整个程序入口确认
	(1) 在OS环境下，程序入口一般是main()	// busybox执行是在OS环境下
	(2) uboot、kernel程序入口由链接脚本指定

	// libbb/appletlib.c -- main()  -- grep -nR * "busybox-lds"

2.19.4.4、busybox中main函数全解析
	(1) busybox先执行main，在main函数中通过传参argv[0]来调用相应的xxx_main()
		来具体实现某一个命令
		// xxx_main()命令实现通过调用linux相关api或库函数

2.19.5.1、inittab解析与执行
	(1) inittab的解析busybox/init/init.c/init_main
	(2) 执行逻辑是：先通过parse_inittab函数解析/etc/inittab（解析的重点是
		将inittab中的各个action和process解析出来）
		然后后面先直接执行sysinit和wait和once（注意这里只执行一遍），然后
		在while(1)死循环中去执行respwan和askfirst

2.19.5.3、busybox的体积优势原理

2.19.6.rcS文件介绍1
	(1) /etc/init.d/rcS文件是linux的运行时配置文件中最重要的一个，其他的
		一些配置都是由这个文件引出来的
	(2) PATH=xxx
	// busybox-1.24.1/init/init.c	-- line1073
	// echo $HOME
	// echo $USER
	// echo $SHELL

2.19.6.2、runlevel=
	(1) runlevel也是一个shell变量，并且被导出为环境变量		// init 0  -- 设置 runlevel
	(3) runlevel=S表示将系统设置为单用户模式

2.19.6.3、umask=
	(1) umask是linux的一个命令，作用是设置linux系统的umask值
	(2) umask值决定当前用户在创建文件时的默认权限

2.19.6.4、mount -a
	(2) mount -a是挂载所有的应该被挂载的文件系统，在busybox中mount -a时busybox
		会去查找一个文件/etc/fstab文件，这个文件按照一定的格式列出来所有应该被
		挂载的文件系统（包括了虚拟文件系统）

	// 应用程序的动态链接库找不到时，可能也会提示文件不存在
	// shell脚本文件格式不对，提示文件不存在

2.19.8.1、mdev
	(1) mdev是udev的嵌入式简化版本，应用层软件
		用来配合linux驱动生成相应/dev目录下的设备文件

2.19.8.2、hostname

2.19.8.3、ifconfig

2.19.9.profile文件和用户登录理论
2.19.9.1、profile文件添加
	(1) 之前添加了/bin/hostname在/etc/sysconfig/HOSTNAME文件中定义了一个hostname（aston210），
		实际效果是：命令行下hostname命令查到的host名字确实是aston210。
		但是问题就是命令行的提示符是没有显示的。
	(2) 这个问题的解决就要靠profile文件。将提供的profile文件放入/etc/目录下即可。
	(3) 添加了之后的实验现象：命令行提示符前面显示：[@aston210 ]#
		结论是：第一，profile文件起了作用，hostname显示出来了。
		第二，还有个问题，登录用户名没显示出来。
		原因就是我们直接进入了命令行而没有做登录。
		等我们添加了用户登录功能，并且成功登陆后这个问题就能解决。
	(4) profile文件工作原理是：profile文件也是被busybox（init进程）自动调用的，
		所以是认名字的。

2.19.9.2、如何看到用户登录界面
	(1) linux中有一个原则就是用一个小程序来完成一个功能。
		如果我们产品确实需要很复杂的综合型的功能，我们倾向于先使用很多个小程序
		完成其中的一个功能，然后再将这些小程序集成起来完成整个大功能的产品。
	(2) 这种集成很多个小程序来完成一个大的功能，有很多种技术实现。
		譬如shell脚本，还有一些别的技术，譬如linux启动中的inittab。
	(3) 因为我们之前intttab中有一个配置项 ::askfirst:-/bin/sh，
		这个配置项作用就是当系统启动后就去执行/bin/sh，执行这个就会出现命令行。
		因此我们这样的安排就会直接进入命令行而不会出现登录界面。
	(4) 我们要出现登录界面，就不能直接执行/bin/sh，
		而应该执行一个负责出现登录界面并且负责管理用户名和密码的一个程序，
		busybox中也集成了这个程序（就是/bin/login和/sbin/gettty）
		因此我们要在inittab中用/bin/login或者/sbin/getty去替代/bin/sh。

2.19.9.3、用户名和密码的设置
	(1) 用户名和密码的设置是和登录程序有关联的，但是/bin/login和/sbin/getty在
		用户名和密码的管理上是一样的。其实常见的所有的linux系统的用户名和密码
		的管理几乎都是一样的。
	(2) 密码一般都是用加密文字的，而不是用明文。意思就是系统中的密码肯定是在系
		统中的一个专门用来存密码的文件中存储的，用明文存密码有风险，因此linux系
		统都是用密文来存储密码的。关于密文密码的使用下节课实践时会详细讲。

2.19.10.用户登录实战
2.19.10.1、添加/bin/login到sysinit
	(1) 在inittab中修改，去掉/bin/sh，换上/bin/login，则系统启动后出现登录界面
		可以输入用户名和密码。
	(2) 实验现象：成功出现用户登录界面，但是死活密码不对。

2.19.10.2、添加passwd和shadow文件
	(1) 为什么用户名和密码不对？因为我们根本没有为root用户设置密码。
	(2) linux系统中用来描述用户名和密码的文件是passwd和shadow文件，
		这两个文件都在etc目录下。passwd文件中存储的是用户的密码设置，
		shadow文件中存储的是加密后的密码。
	(3) 我们直接复制ubuntu系统中的/etc/passwd和/etc/shadow文件到当前制作的
		rootfs目录下，然后再做修改即可。
	(4) /etc/passwd和/etc/shadow修理好后，shadow中默认有一个加密的密码口令，
		这个口令和你拷贝的shadow本身有关，像我的ubuntu中root用户的密码就
		是root，因此复制过来后登陆时的密码还是root。

2.19.10.3、重置密码实践
	(1) ubuntu刚装好的时候默认登录是用普通用户登录的，默认root用户是关闭的。
		普通用户的密码是在装系统的时候设置的，普通用户登陆后可以使用
		su passwd root给root用户设置密码，设置了密码后root用户才可以登录。
	(2) 其实这个原因就是root用户在/etc/shadow文件中加密口令是空白的
		所以是不能登录的。
	(3) busybox中因为没有普通用户，所以做法是：默认root用户如果加密口令是空的
		则默认无密码直接登录。等我们登陆了之后还是可以用passwd root给root用户
		设置密码。
	(4) 平时有时候我们忘记了自己的操作系统的密码，怎么办？
		有一种解决方法就是用其他系统（WindowsPE系统或者ubuntu的单用户模式等···）
		来引导启动，启动后挂载到我们的硬盘上，然后找到/etc/shadow文件，去掉密文
		密码后保存。然后再重启系统后密码就没了。

2.19.10.4、getty实战
	(1) 大家后面做项目会发现，inittab中最常见的用于登录的程序不是/bin/login，
		反而是/sbin/getty。
	(2) 这两个的差别不详，但是在busybox中这两个是一样的。这两个其实都是
		busybox的符号链接而已。因此不用严格区分这两个
	(3) 我们可以在inittab中用getty替换login程序来实现同样的效果。

2.19.11.1、静态编译链接helloworld程序并执行
	(1) 静态链接：arm-linux-gcc hello.c -o hello_satic -static
	(2) 动态链接：arm-linux-gcc hello.c -o hello_dynamic

	// 平台相关性，C语言不是跨平台语言

2.19.11.3、找到并复制动态链接库文件到rootfs中	//	/lib/xxx.so 	/usr/lib/
	(1) cross_compile arm-2009q3 的动态链接库目录：
		/usr/local/arm/arm-2009q3/arm-none-linux-gnueabi/libc/lib
	(2) 复制命令：cp lib/ *so* /root/porting_x210/rootfs/rootfs/lib/ -rdf
		主要目的就是符号链接复制过来还是符号链接

2.19.11.4、使用strip工具去掉库中符号信
	去掉符号命令：arm-linux-strip *so*

2.19.12.1、修改rcS实现开机自启动
	// etc/inittab
	// etc/init.d/rcS

2.19.12.2、前台运行与后台运行	// ./xxx &
	(1) 程序运行时占用了当前的控制台，导致控制台无法使用

2.19.12.3、开机装载驱动等其他开机自动执行

2.19.12.4、实际开发中rootfs的rcS是怎样的
	(2) inittab文件：sysinit执行rcS，shutdown时执行rcK
	(3) 分析/etc/init.d/rcS和rcK文件发现，rcS和rcK都是去遍历执行/etc/init.d/
		目录下的S开头的脚本文件，区别是rcS传参是start，rcK传参是stop
	(4) 由此可以分析出来，正式产品中的rcS和rcK都是一个引入，而不是真正干活的。
		真正干活的配置脚本是/etc/init.d/S??*。
		这些文件中肯定有一个判断参数是start还是stop，然后start时去做一些初始化
		stop时做一些清理工作

2.19.13.2、动手制作ext2格式的镜像		// 注意创建的镜像大小


