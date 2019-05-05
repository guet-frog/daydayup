
2.19.3.inittab详解
	本节引入并且解析inittab的格式，并且通过添加inittab文件使我们制作的最小根文件系统工作起来
2.19.4.busybox源码分析1
	本节开始解析busybox源码，主要分析了busybox中main函数和xxx_main，通过分析从而搞明白busybox的工作原理
2.19.5.busybox源码分析2
	本节继续解析busybox源码，主要分析了inittab文件解析部分、通用shell命令如pwd命令的执行部分以及busybox的体积优势。
2.19.6.rcS文件介绍1
	本节讲解/etc/init.d/rcS文件并且解析其中PATH、runlevel、umask、mount等步骤的具体含义。
2.19.7.rcS文件实战1
	本节根据上节理论实际添加rcS文件，并且解决过程中遇到的细节问题，通过实践现象来印证理论，加深理解。
2.19.8.rcS文件介绍2
	本节继续解析rcS文件，主要分析了hostname、ifconfig、mdev等步骤。
2.19.9.profile文件和用户登录理论
	本节通过添加profile文件在命令行中显示hostname，然后讲解了用户登录界面的相关理论。
2.19.10.用户登录实战
	本节进行用户登录界面的实战，通过实际修改inittab文件来实现用户登录，并且添加passwd和shadow文件来管理密码。
2.19.11.动态链接库的拷贝
	本节首先通过执行静态编译链接和动态编译链接的2个helloworld程序来引出动态链接库，然后进行实际的库文件拷贝工作解决动态链接程序的工作问题。
2.19.12.开机自启动与主流rcS格式介绍
	本节讲述如何在rcS文件中做修改实现开机自启动和后台运行，然后介绍了主流产品中rcS文件的式样。
2.19.13.制作ext2格式的镜像并烧录启动
	本节将前面做好的文件夹格式的rootfs制作成ext2格式的可烧录的镜像，然后实际烧录启动，让大家体会产品开发的全过程。



2.19.2.1、busybox源码下载

2.19.2.2、修改Makefile
	(1) ARCH = arm
	(2) CROSS_COMPILE = /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

2.19.2.3、make menuconfig	
	(1) busybox menuconfig配置.txt

2.19.2.4、make 然后 make install			// make 与 make install
	(1) 编译问题
	// find -name "sync.c"
	// find -name "*sync*"
	// make menuconfig 禁止 CONFIG_SYNC

	(2) make install在所有的linux下的软件中作用都是安装软件
		在传统的linux系统中安装软件时都是选择源代码方式安装的
		我们下载要安装的软件源代码，然后配置、编译、安装
		make install的目的就是将编译生成的可执行程序及其依赖的库文件、配置文件、头文件安装到当前系统中指定
		（一般都可以自己指定安装到哪个目录下，如果不指定一般都有个默认目录）的目录下

2.19.2.5、设置bootargs挂载添加了busybox移植的rootfs
	(1) 之前建立了一个空的文件夹然后自己touch linuxrc随便创建了一个不能用的/linuxrc然后去nfs挂载rootfs，
		实验结果是：挂载成功，执行/linuxrc失败。
	(2) 现在我们移植了busybox后/linuxrc就可以用了，然后再次去nfs挂载这个rootfs。
		预计看到的效果是：挂载成功，执行/linuxrc也能成功。
	(3) 注意uboot的bootargs设置成：
		setenv bootargs
		root=/dev/nfs
		nfsroot=192.168.1.141:/root/porting_x210/rootfs ip=192.168.1.10:192.168.1.141:192.168.1.1:255.255.255.0::eth0:off
		init=/linuxrc console=ttySAC2,115200
	(4) 实验结果：挂载成功，执行/linuxrc（也就是busybox）成功
		但是因为找不到/etc/init.d/rcS和/dev/tty2等文件所以一直在打印错误提示信息，但是其实有进入命令行

2.19.3.1、添加一个典型的inittab			// /etc/inittab

2.19.3.2、inittab格式解析		// 只要进程1在运行，则不断判断action是否满足，是否需要执行process
	(1)inittab的工作原理就是被/linuxrc（也就是busybox）执行时所调用起作用。
	(2)inittab在/etc目录下，所以属于一个运行时配置文件，是文本格式的（内容是由一系列的遵照一个格式组织的字符组成的）
		实际工作的时候busybox会（按照一定的格式）解析这个inittab文本文件，然后根据解析的内容来决定要怎么工作。
	(3)busybox究竟如何完成解析并且解析结果如何去工作（busybox中实现/etc/inittab的原理）并不是我们的目标，
		我们的重点是inittab的格式究竟怎样的？我们看到一个inittab后怎么去分析这个inittab对启动的影响。
	(4)inittab的格式在busybox中定义的，网上可以搜索到详细的格式说明，具体去参考即可：
	第一个：#开始的行是注释
	第二个：冒号在里面是分隔符，分隔开各个部分。
	第三个：inittab内容是以行为单位的，行与行之间没有关联，每行都是一个独立的配置项，每一个配置项表示一个具体的含义。
	第四个：每一行的配置项都是由3个冒号分隔开的4个配置值共同确定的。这四个配置值就是id:runlevels:action:process。
			值得注意得是有些配置值可以空缺，空缺后冒号不能空缺，所以有时候会看到连续2个冒号。
	第五个：每一行的配置项中4个配置值中最重要的是action和process，action是一个条件/状态，process是一个可被执行的程序的pathname。
			合起来的意思就是：当满足action的条件时就会执行process这个程序。

	注意：	理解inittab的关键就是明白
			“当满足action的条件时就会执行process这个程序 -- 只要进程1在运行则一直判断 ” 
			你去分析busybox的源代码就会发现，busybox最终会进入一个死循环，在这个死循环中去反复检查是否满足各个action的条件，
			如果某个action的条件满足就会去执行对应的process。

	第六个：明白各个action什么意思


2.19.4.busybox源码分析1		// libbb/appletlib.c -- main()		-- grep -nR * "busybox-lds"
2.19.4.3、整个程序入口确认
	(1) 分析一个程序，不管多庞大还是小，最好的路线都是按照程序运行时的逻辑顺序来。所以找到一个程序的入口至关重要。
	(2) 学C语言的时候都知道程序的主函数main函数就是整个程序的入口。这种情况适应于操作系统下工作的应用程序的情况。
	(3) 在uboot和linux kernel这两个大的C语言的项目中，main函数都没有，都不是入口。在我们这种裸机程序中入口不是main函数，而是由连接脚本来指定的。
	(4) busybox是linux启动起来后工作的一个应用程序，因此其中必然有main函数，而且main就是入口。
2.19.4.4、busybox中main函数全解析
	(1) busybox入口就是main函数，其中有很多个main但是只有一个起作用了，其他的是没起作用的。
		真正的busybox工作时的入口是libbb/appletlib.c中的main函数
	(2) busubox中有很多xxx_main函数，这些main函数每一个都是busybox支持的一个命令的真正入口。
		譬如ls_main函数就是busybox当作ls函数使用时的入口程序。
	(3) ls或者cd等命令其实都是busybox一个程序，但是实际执行时的效果却是各自的效果。
		busybox是如何实现一个程序化身万千还能各自工作的？答案就是main转xxx_main。
		也就是说busybox每次执行时都是先执行其main，在main函数中识别（靠main函数的传参argv[0]来识别）
		我们真正要执行的函数（譬如ls）然后去调用相应的xxx_main（譬如ls_main）来具体实现这个命令。

2.19.5.busybox源码分析2
2.19.5.1、inittab解析与执行
	(1) inittab的解析是在busybox/init/init.c/init_main函数中
	(2) 执行逻辑是：先通过parse_inittab函数解析/etc/inittab（解析的重点是将inittab中的各个action和process解析出来），
		然后后面先直接执行sysinit和wait和once（注意这里只执行一遍），然后在while(1)死循环中去执行respwan和askfirst。

2.19.5.2、pwd命令执行路径分析
	(1)根据上节讲的，我们在busybox命令行下执行pwd命令时实际执行的是pwd_main这个函数。

2.19.5.3、busybox的体积优势原理
	(1) busybox实际上就是把ls、cd、mkdir等很多个linux中常用的shell命令集成在一起了。
		集成在一起后有一个体积优势：就是busybox程序的大小比busybox中实现的那些命令的大小加起来要小很多。
	(2) busybox体系变小的原因主要有2个：
		第一个是busybox本身提供的shell命令是阉割版的
		（busybox中的命令支持的参数选项比发行版中要少，譬如ls在发行版中可以有几十个-x，
		但是在busybox中只保留了几个常用的选项，不常用的都删除掉了）；
		第二个是busybox中因为所有的命令的实现代码都在一个程序中实现，
		而各个命令中有很多代码函数都是通用的（譬如ls和cd、mkdir等命令都会需要去操作目录，
		因此在busybox中实现目录操作的函数就可以被这些命令共用），共用会降低重复代码出现的次数，从而减少总的代码量和体积。
	(3) 经过分析，busybox的体积优势是嵌入式系统本身的要求和特点造成的。

2.19.6.rcS文件介绍1			// PATH= ; which pwd ; /bin/ls
	(1) /etc/init.d/rcS文件是linux的运行时配置文件中最重要的一个，其他的一些配置都是由这个文件引出来的。
		这个文件可以很复杂也可以很简单，里面可以有很多的配置项。
	(2) PATH=xxx
	// busybox-1.24.1/init/init.c	-- line1073
	// echo $HOME
	// echo $USER
	// echo $SHELL

2.19.6.2、runlevel=
	(1) runlevel也是一个shell变量，并且被导出为环境变量。			// init 0  -- 设置 runlevel
	(2) runlevel这个环境变量到底有什么用？
	(3) runlevel=S表示将系统设置为单用户模式

2.19.6.3、umask=
	(1) umask是linux的一个命令，作用是设置linux系统的umask值
	(2) umask值决定当前用户在创建文件时的默认权限

2.19.6.4、mount -a
	(1) mount命令是用来挂载文件系统的
	(2) mount -a是挂载所有的应该被挂载的文件系统，在busybox中mount -a时busybox会去查找一个文件/etc/fstab文件，
		这个文件按照一定的格式列出来所有应该被挂载的文件系统（包括了虚拟文件系统）

							// 应用程序的动态链接库找不到时，可能也会提示文件不存在
2.19.7.rcS文件实战1			// shell脚本文件格式不对，提示文件不存在
2.19.7.1、PATH&runlevel
	(1) 我们实战发现rcS文件明明存在但是却提示不存在，问题原因就是rcS文件在windows下创建的，
		行尾换行符为'\r\n'，多了点东西。但是因为ubuntu中的vi对行尾做了优化，所以在ubuntu中是看不出来多了东西的。
		但是在securecrt下一看就发现每一行末尾多出来了一个^M。
	(2) 这个故事告诉我们：shell脚本文件如果格式不对，运行时可能会被提示文件不存在。
	(3) 扩展讲一个：有时候一个应用程序执行时也会提示文件不存在，问题可能是这个程序所调用的一个动态链接库找不到。
	(4) 测试结果：PATH本来在busybox中就已经用代码导出过了，所以rcS中再次导出没有任何明显的现象，
		因此看不出什么差别；runlevel实际执行结果一直是unknown，问题在于busybox并不支持runlevel这个特性。

2.19.7.2、umask测试
	(1) umask是022的时候，默认touch创建一个文件的权限是644
	(2) umask是044的时候，默认touch创建一个文件的权限是622
	(3) umask是444的时候，默认touch创建一个文件的权限是222
		总结：umask的规律就是：umask值和默认创建文件的权限值加起来是666.

2.19.7.3、mount测试
	(1) 挂载时全部出错：
		mount: mounting proc on /proc failed: No such file or directory
		mount: mounting sysfs on /sys failed: No such file or directory
		mount: mounting tmpfs on /var failed: No such file or directory
		mount: mounting tmpfs on /tmp failed: No such file or directory
		mount: mounting tmpfs on /dev failed: No such file or directory
	(2) 原因是因为根文件系统中找不到挂载点。
		所谓挂载点就是我们要将目标文件系统（当然这里都是虚拟文件系统）挂载到当前文件系统中的某一个目录中，这个目录就是挂载点。
	(3) 解决方案就是自己在制作的rootfs根目录下创建这些挂载点目录即可。
	(4) 验证是否挂载成功，可以看挂载时输出信息；还可以启动后去看proc和sys文件夹，如果有文件出现则证明挂载成功了，如果没东西就证明失败了。

2.19.8.rcS文件介绍2
2.19.8.1、mdev
	(1) mdev是udev的嵌入式简化版本，udev/mdev是用来配合linux驱动工作的一个应用层的软件，udev/mdev的工作就是配合linux驱动生成相应的/dev目录下的设备文件。
	(2) 因为这个问题涉及到驱动，因此详细讲解要等到驱动部分。这里我们只是通过一些直观的现象来初步理解udev/mdev的工作效果。
	(3) 在rcS文件中没有启动mdev的时候，/dev目录下启动后是空的；在rcS文件中添加上mdev有关的2行配置项后，再次启动系统后发现/dev目录下生成了很多的设备驱动文件。
	(4) /dev目录下的设备驱动文件就是mdev生成的，这就是mdev的效果和意义。

2.19.8.2、hostname
	(1) hostname是linux中的一个shell命令。命令（hostname xxx）执行后可以用来设置当前系统的主机名为xxx，
		直接hostname不加参数可以显示当前系统的主机名。
	(2) /bin/hostname -F /etc/sysconfig/HOSTNAME -F来指定了一个主机名配置文件（这个文件一般文件名叫hostname或者HOSTNAME）

2.19.8.3、ifconfig

2.19.9.profile文件和用户登录理论
2.19.9.1、profile文件添加
	(1) 之前添加了/bin/hostname在/etc/sysconfig/HOSTNAME文件中定义了一个hostname（aston210），
		实际效果是：命令行下hostname命令查到的host名字确实是aston210。但是问题就是命令行的提示符是没有显示的。
	(2) 这个问题的解决就要靠profile文件。将提供的profile文件放入/etc/目录下即可。
	(3) 添加了之后的实验现象：命令行提示符前面显示：[@aston210 ]#
		结论是：第一，profile文件起了作用，hostname显示出来了。第二，还有个问题，登录用户名没显示出来。
		原因就是我们直接进入了命令行而没有做登录。等我们添加了用户登录功能，并且成功登陆后这个问题就能解决。
	(4) profile文件工作原理是：profile文件也是被busybox（init进程）自动调用的，所以是认名字的。

2.19.9.2、如何看到用户登录界面
	(1) linux中有一个原则就是用一个小程序来完成一个功能。
		如果我们产品确实需要很复杂的综合型的功能，我们倾向于先使用很多个小程序完成其中的一个功能，
		然后再将这些小程序集成起来完成整个大功能的产品。
	(2) 这种集成很多个小程序来完成一个大的功能，有很多种技术实现。譬如shell脚本，还有一些别的技术，譬如linux启动中的inittab。
	(3) 因为我们之前intttab中有一个配置项 ::askfirst:-/bin/sh，
		这个配置项作用就是当系统启动后就去执行/bin/sh，执行这个就会出现命令行。
		因此我们这样的安排就会直接进入命令行而不会出现登录界面。
	(4) 我们要出现登录界面，就不能直接执行/bin/sh，
		而应该执行一个负责出现登录界面并且负责管理用户名和密码的一个程序，
		busybox中也集成了这个程序（就是/bin/login和/sbin/gettty），因此我们要在inittab中用/bin/login或者/sbin/getty去替代/bin/sh。

2.19.9.3、用户名和密码的设置
	(1) 用户名和密码的设置是和登录程序有关联的，但是/bin/login和/sbin/getty在用户名和密码的管理上是一样的。
		其实常见的所有的linux系统的用户名和密码的管理几乎都是一样的。
	(2) 密码一般都是用加密文字的，而不是用明文。意思就是系统中的密码肯定是在系统中的一个专门用来存密码的文件中存储的，
		用明文存密码有风险，因此linux系统都是用密文来存储密码的。关于密文密码的使用下节课实践时会详细讲。

2.19.10.用户登录实战
2.19.10.1、添加/bin/login到sysinit
	(1) 在inittab中修改，去掉/bin/sh，换上/bin/login，则系统启动后出现登录界面。可以输入用户名和密码。
	(2) 实验现象：成功出现用户登录界面，但是死活密码不对。

2.19.10.2、添加passwd和shadow文件
	(1) 为什么用户名和密码不对？因为我们根本没有为root用户设置密码。
	(2) linux系统中用来描述用户名和密码的文件是passwd和shadow文件，这两个文件都在etc目录下。
		passwd文件中存储的是用户的密码设置，shadow文件中存储的是加密后的密码。
	(3) 我们直接复制ubuntu系统中的/etc/passwd和/etc/shadow文件到当前制作的rootfs目录下，然后再做修改即可。
	(4) /etc/passwd和/etc/shadow修理好后，shadow中默认有一个加密的密码口令，
		这个口令和你拷贝的shadow本身有关，像我的ubuntu中root用户的密码就是root，因此复制过来后登陆时的密码还是root。

2.19.10.3、重置密码实践
	(1) ubuntu刚装好的时候默认登录是用普通用户登录的，默认root用户是关闭的。
		普通用户的密码是在装系统的时候设置的，普通用户登陆后可以使用su passwd root给root用户设置密码，设置了密码后root用户才可以登录。
	(2) 其实这个原因就是root用户在/etc/shadow文件中加密口令是空白的。所以是不能登录的。
	(3) busybox中因为没有普通用户，所以做法是：默认root用户如果加密口令是空的则默认无密码直接登录。
		等我们登陆了之后还是可以用passwd root给root用户设置密码。
	(4) 平时有时候我们忘记了自己的操作系统的密码，怎么办？
		有一种解决方法就是用其他系统（WindowsPE系统或者ubuntu的单用户模式等···）来引导启动，启动后挂载到我们的硬盘上，
		然后找到/etc/shadow文件，去掉密文密码后保存。然后再重启系统后密码就没了。

2.19.10.4、getty实战
	(1) 大家后面做项目会发现，inittab中最常见的用于登录的程序不是/bin/login，反而是/sbin/getty。
	(2) 这两个的差别不详，但是在busybox中这两个是一样的。这两个其实都是busybox的符号链接而已。因此不用严格区分这两个
	(3) 我们可以在inittab中用getty替换login程序来实现同样的效果。


2.19.11.动态链接库的拷贝
2.19.11.1、静态编译链接helloworld程序并执行
	(1) 任务：自己写一个helloworld程序，然后交叉编译连接，然后丢到开发板根文件系统中，开机后去运行。
	(2) C程序如果使用gcc来编译则可以在主机ubuntu中运行，但是不能在开发板运行；
		要在开发板运行需要用arm-linux-gcc来交叉编译，但是这时候就不能在主机ubuntu中运行了。
		我们可以用file xx命令来查看一个elf可执行程序是哪个架构的。
	(3) 静态链接：arm-linux-gcc hello.c -o hello_satic -static
	(4) 实验结果：静态编译连接后生成的hello_satic已经可以成功运行。

2.19.11.2、动态编译连接helloworld程序并执行
	(1) 动态链接：arm-linux-gcc hello.c -o hello_dynamic
	(2) 实验结果：-sh: ./hello_dynamic: not found运行时提示找不到程序。
	(3) 错误分析：动态连接的hello程序中调用到了printf函数，
		而printf函数在动态连接时要在运行时环境（开发板的rootfs）中去寻找对应的库文件
		（开发板rootfs中部署的动态链接库中包含了printf函数的那个库文件）。
		如果找到了则printf函数就会被成功解析，然后hello_dynamic程序就会被执行；
		如果找不到则程序就不能被执行，命令行会提示错误信息-sh: ./hello_dynamic: not found
	(4) 解决方案：将arm-linux-gcc的动态链接库文件复制到开发板rootfs的/lib目录下即可解决。

2.19.11.3、找到并复制动态链接库文件到rootfs中
	(1) 我们用的arm-2009q3 这个交叉编译工具链的动态链接库在/usr/local/arm/arm-2009q3/arm-none-linux-gnueabi/libc/lib目录下。
		其他的一些交叉编译工具链中动态链接库的目录不一定在这里，要去找一下。找的方法就是find
	(2) 复制动态链接库到roots/lib目录下。复制时要注意参数用-rdf，主要目的就是符号链接复制过来还是符号链接。
		复制命令：cp lib/*so* /root/porting_x210/rootfs/rootfs/lib/ -rdf			*/
	(3) 现在再去测试./hello_dynamic看看是否可以运行，实验结果是可以运行。

2.19.11.4、使用strip工具去掉库中符号信息
	动态链接库so文件中包含了调试符号信息，这些符号信息在运行时是没用的（调试时用的），这些符号会占用一定空间。
	在传统的嵌入式系统中flash空间是有限的，为了节省空间常常把这些符号信息去掉。这样节省空间并且不影响运行。
	去掉符号命令：arm-linux-strip *so*
	实际操作后发现库文件由3.8M变成了3.0M，节省了0.8M的空间。

2.19.12.开机自启动与主流rcS格式介绍
2.19.12.1、修改rcS实现开机自启动
	(1) 开机自启动指的是让一些应用程序能够开机后自动执行
	(2) 开机自启动的实现原理就是在开机会自动执行的脚本rcS中添加上执行某个程序的语句代码即可

2.19.12.2、前台运行与后台运行
	(1) 程序运行时占用了当前的控制台，因此这个程序不结束我们都无法使用控制台，这就叫前台运行。默认执行程序就是前台运行的。
	(2) 后台运行就是让这个程序运行，并且同时让出控制台。这时候运行的程序还能照常运行而且还能够不影响当前控制台的使用。
	(3) 让一个程序后台运行的方法就是 ./xxx &

2.19.12.3、开机装载驱动等其他开机自动执行

2.19.12.4、实际开发中rootfs的rcS是怎样的
	(1) 我们以X210开发板九鼎科技做的rootfs中rcS部分来分析
	(2) 分析inittab发现：sysinit执行rcS，shutdown时执行rcK。
	(3) 分析/etc/init.d/rcS和rcK文件发现，rcS和rcK都是去遍历执行/etc/init.d/目录下的S开头的脚本文件，区别是rcS传参是start，rcK传参是stop。
	(4) 由此可以分析出来，正式产品中的rcS和rcK都是一个引入，而不是真正干活的。真正干活的配置脚本是/etc/init.d/S??*。
		这些文件中肯定有一个判断参数是start还是stop，然后start时去做一些初始化，stop时做一些清理工作。

2.19.13.制作ext2格式的镜像并烧录启动
2.19.13.1、确定文件夹格式的rootfs可用
	(1) 设置bootargs为nfs启动方式，然后从主机ubuntu中做好的文件夹格式的rootfs去启动，然后看启动效果，作为将来的参照物。
2.19.13.2、动手制作ext2格式的镜像
	(1)	dd if=/dev/zero of=rootfs.ext2 bs=1024 count=10240
		losetup  /dev/loop1 rootfs.ext2
		mke2fs -m 0 /dev/loop1 10240
		mount -t ext2 /dev/loop1 ./ext2_rootfs/

	(2) 向./rootfs中复制内容，用cp ../rootfs/* ./ -rf	*/
	(3) umount /dev/loop1
		losetup -d /dev/loop1
(4)完成后得到的rootfs.ext2就是我们做好的rootfs镜像。拿去烧录即可。

2.19.13.3、烧录镜像并设置合适的bootargs
	(1) 使用fastboot烧录制作好的rootfs.ext2到开发板inand中
		fastboot flash system rootfs.ext2
		烧录完成后重启系统
	(2) 设置bootargs为：set bootargs console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext2
	(3) 启动后发现现象和之前nfs方式启动挂载rootfs后一样的，至此rootfs制作实验圆满完成。

2.19.13.4、总结
	(1) 13节课将rootfs的制作步骤和原理已经完全清楚的讲完了
	(2) 制作过程本身如果有文档指导非常容易，非常简单，但是我们学习时侧重于不是rootfs制作本身，而是rootfs工作的原理分析。





