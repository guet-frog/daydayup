
2.15.1.2、分析源码目录下的单个文件
	// Kbuild(kernel build)、Kconfig
	// Makefile、mk

2.15.2.linux内核源码目录结构2
	(10)ipc	inter process communication, 进程间通信, linux支持的IPC的代码实现
	
	(12)lib	不同于C语言的标准库函数
			内核编程中不能使用C语言标准库函数, lib下的库函数用来替代C标准库函数
			内核编程中要把字符串转成数字只能用lib目录下的atoi函数, 不能用标准C库中的atoi
			内核编程中要打印使用lib目录下的printk, 不能用标准C库中的printf

	// important: arch、drivers

2.15.3.1、先确认Makefile
	// ARCH=arm
	// CROSS_COMPILE   ?= /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-

2.15.3.1、配置编译
	step1: make x210ii_qt_defconfig
		// cp arch/arm/configs/x210ii_qt_defconfig .config

	step2: make menuconfig
		// touch .config or modify .config

		// apt-get install libncurses5-dev		-- 安装 ncurse库

	step3: make
		// arch/arm/boot/zImage

2.15.4.2、配置的关键是得到.config文件	// -- 类似于x210_sd.h
	// 读取.config中配置项, 指导整个编译链接过程

2.15.5.menuconfig的使用和演示

	模块化: 单独链接成一个内核模块.ko文件(不链接到zImage), linux系统内核启动后可以动态的加载或卸载这个模块

2.15.6.1、menuconfig本身由一套软件支持
	// scripts/kconfig/lxdialog -- menuconfig源码

	// menuconfig、Kconfig、.config的关系

2.15.7.1、Kconfig的格式
(2)驱动移植等工作时, 需要修改Kconfig中的配置项, 将某个设备驱动添加到内核的配置项目中

(4)menuconfig表示菜单（本身属于一个菜单中的项目，但是他又有子菜单项目）、
	config表示菜单中的一个配置项（本身并没有子菜单下的项目）。
(5)menuconfig或者config后面空格隔开的大写字母表示的类似于 NETDEVICES 的就是这个配置项的配置项名字，
	这个字符串前面添加CONFIG_后就构成了.config中的配置项名字。
(6)一个menuconfig后面跟着的所有config项就是这个menuconfig的子菜单。这就是Kconfig中表示的目录关系。
(7)内核源码目录树中每一个Kconfig都会source引入其所有子目录下的Kconfig，
	从而保证了所有的Kconfig项目都被包含进menuconfig中。
	这个也告诉我们：如果你自己在linux内核中添加了一个文件夹，
	一定要在这个文件夹下创建一个Kconfig文件，
	然后在这个文件夹的上一层目录的Kconfig中source引入这个文件夹下的Kconfig文件。

2.15.7.2、tristate和bool的含义
(1)tristate意思是三态（3种状态，对应Y、N、M三种选择方式），bool是要么真要么假（对应Y和N）。所以tristate的意思就是这个配置项可以被三种选择，bool的意思是这个配置项只能被2种选择。


2.15.8.Kconfig文件详解2
2.15.8.1、depends的含义
(1)depends中文意思是“取决于”或者“依赖于”，所以depends在这里的意思是：本配置项依赖于另一个配置项。如果那个依赖的配置项为Y或者M，则本配置项才有意义；如果依赖的哪个配置项本身被设置为N，则本配置项根本没有意义。
(2)depends项目会导致make menuconfig的时候找不到一些配置项。所以你在menuconfig中如果找不到一个选项，但是这个选项在Kconfig中却是有的，则可能的原因就是这个配置项依赖的一个配置项是不成立的。
(3)depends并不要求依赖的配置项一定是一个，可以是多个，而且还可以有逻辑运算。这种时候只要依赖项目运算式子的裸机结果为真则依赖就成立。

2.15.8.2、help
(1)帮助信息，告诉我们这个配置项的含义，以及如何去配置他。

2.15.8.3、Kconfig和.config文件和Makefile三者的关联
(1)配置项被配置成Y、N、M会影响.config文件中的CONFIG_XXX变量的配置值。
(2)这个.config中的配置值（=y、=m、没有）会影响最终的编译链接过程。如果=y则会被编入（built-in），如果=m会被单独连接成一个ko模块，如果没有则对应的代码不会被编译。那么这么是怎么实现的？都是通过makefile实现的。
(3)obj-$(CONFIG_DM9000) += dm9000.o
如果CONFIG_DM9000变量值为y，则obj += dm9000.o，因此dm9000.c会被编译；如果CONFIG_DM9000变量未定义，则dm9000.c不会被编译。如果CONFIG_DM9000变量的值为m则会被连接成ko模块（这个是在linux内核的Makefile中定义的规则）

总结：把menuconfig中的菜单项、Kconfig中的配置项、.config中的一行、 Makefile中的一行，这4个东西结合起来理解，则整个linux内核的配置体系就明了了。


2.15.9.menuconfig的实验学习思路
2.15.9.1、验证menuconfig和.config的关系
(1)make menuconfig时，会读取.config中的配置值来初始化menuconfig中的配置项。
验证：如果理论正确的，那么我自己手工修改了.config的配置后，再次make menuconfig时看到的初始值就应该是我手工修改的。
(2)menuconfig中修改了（按Y、N、M）配置项的值，然后退出时保存，则这个保存结果会修改.config文件中的相应行。
验证：如果结论是正确的，那么在menucofig中修改了配置后保存退出，再次去手工打开.config文件则可以看到相应配置的一行内容被修改了。

2.15.9.2、验证menuconfig和Kconfig的关系
(1)menuconfig读取Kconfig的内容作为菜单项目内容。
验证1：在Kconfig中删除一个config项，则再次make menuconfig时就看不到这个项目了。（上课时已经验证过了）
验证2：在Kconfig中自己添加创建一个config项，则再次make menuconfig时就能看到多了一个项目。

2.15.9.3、验证验证menuconfig和Makefile的关系
(1)我找一个模块，把他配制成y，然后去make编译连接，最后得到的zImage中这个模块就应该被编译连接进去到zImage中了。
验证：
	方法一：去这个模块对应的源代码目录看一下这个源码有没有被编译
	方法二：去zImage对应的elf格式的vmlinux中查看符号
	方法三：将vmlinux反编译（objdump）后得到的文件中找模块对应的符号
	方法四：将zImage下载到开发板中启动，启动后看你的模块能不能工作












