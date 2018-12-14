

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

2.15.3.2、make x210ii_qt_defconfig
	// configuration written to .config		-- 生成 .config

2.15.3.2、make menuconfig
	// apt-get install libncurses5-dev		-- 安装 ncurse库

2.15.3.3、make
	// make distclean && make -j4
	// arch/arm/boot/zImage

2.15.4.2、配置的关键是得到.config文件	// -- 类似于x210_sd.h
	// 读取.config中配置项, 指导整个编译链接过程

2.15.4.3、 make xx_defconfig和make menuconfig相配合
	(1)我们为了对.config文件中的两三千个配置项做逐一合适的配置，专门发明了两步结合的配置方式。
	(3)第一步：make xxx_defconfig解决的问题是大部分的配置项（这一步结束后99%的配置项就已经正确了），
		下来就是对个别不同的针对我们的开发板进行细节调整，细节调整就通过make menuconfig来完成。
	(4)make xxx_defconfig这一步其实是参考别人已经做好的，这样做有很多好处：减少很多工作量，避开了很多自己不懂的配置项（譬如对内存管理的、调度系统的等模块的配置项），我们只用管自己需要管的。
	(5)make menuconfig其实就是读取第一步得到的.config，然后给我们一个图形化的界面，让我们可以更加容易的找到自己想要修改的配置项，然后更改配置他。

2.15.4.4、make xx_defconfig到底做了什么？
(1)make x210ii_qt_defconfig其实相当于：cp arch/arm/configs/x210ii_qt_defconfig .config
(2)arch/arm/configs目录下的这么多个xxx_defconfig哪里来的？其实这些文件都是别人手工配置好适合一定的开发板的.config文件后自己把.config文件保存过去的。譬如说我们用S5PV210这个SoC，针对这个SoC的开发板的最初配置肯定是三星的工程师去做的。


2.15.5.menuconfig的使用和演示
2.15.5.1、使用说明解释
(1)make ，menuconfig中本身自带的提示就有所有的用法，这里只要全部理解就可以了。
(2)menuconfig中间的选择区中有很多个选择项，每个选择项对应.config文件中的一个配置项，每一个选择项都可以被选择和配置操作，选择区中的每一项都是有子目录的，将光标放在选择项上按Enter键可以进入子目录（子目录可能还会有子目录）。选择区太短放不下所有的一个目录层级的选项，可以用箭头按键的向上箭头和向下箭头来上翻和下翻。

注：在menuconfig中操作相关的几个键盘按键，主要是；Enter、ESC、四个方向箭头按键。还有一些特殊字符按键，如/  ? 
向上和向下箭头，主要用来在选择项菜单中目录浏览时上下翻
回车，主要作用是选中并且执行select/exit/help。
ESC，主要作用是返回上一层
向左和向右箭头，主要作用是在菜单选项（select、exit、help）间切换。

(3)用法翻译：
箭头按键导航整个菜单，回车按键选择子菜单（注意选项后面有 --->的选项才是有子菜单的，没有这个标识的没有子菜单），高亮的字母是热键（快捷键），键盘按键Y、N、M三个按键的作用分别是将选中模块编入、去除、模块化。双击ESC表示退出，按下？按键可以显示帮助信息，按下/按键可以输入搜索内容来全局搜索信息（类似于vi中的搜索），[]不可以模块化，<>的才可以模块化。

注：linux内核中一个功能模块有三种编译方法：一种是编入、一种去去除、一种是模块化。所谓编入就是将这个模块的代码直接编译连接到zImage中去，去除就是将这个模块不编译链接到zImage中，模块化是将这个模块仍然编译，但是不会将其链接到zImage中，会将这个模块单独链接成一个内核模块.ko文件，将来linux系统内核启动起来后可以动态的加载或卸载这个模块。
在menuconfig中选项前面的括号里，*表示编入，空白表示去除，M表示模块化


2.15.6.menuconfig的工作原理
2.15.6.1、menuconfig本身由一套软件支持
(1)linux为了实现图形化界面的配置，专门提供了一套配置工具menuconfig。
(2)ncurses库是linux中用来实现文字式的图形界面，linux内核中使用了ncurses库来提供menuconfig
(3)scripts\kconfig\lxdialog目录下的一些c文件就是用来提供menuconfig的那些程序源代码。

2.15.6.2、menuconfig读取Kconfig文件
(1)menuconfig本身的软件只负责提供menuconfig工作的这一套逻辑（譬如在menuconfig中通过上下左右箭头按键来调整光标，Enter ESC键等按键按下的响应），而并不负责提供内容（菜单里的项目）。
(2)menuconfig显示的菜单内容（一方面是菜单的目录结构，另一方面是每一个菜单项目的细节）是由内核源码树各个目录下的Kconfig文件来支持的。Kconfig文件中按照一定的格式包含了一个又一个的配置项，每一个配置项在make menuconfig中都会成为一个菜单项目。而且menuconfig中显示的菜单目录结构和源码目录中的Kconfig的目录结构是一样的。
(3)在相应的Kconfig文件中删除一个config项，则再次make menuconfig时这个项目已经看不到了。

2.15.6.3、menuconfig读取/写入.config文件
(1)刚才已经知道menuconfig的菜单内容来自于Kconfig文件，但是每一个菜单的选择结果（Y、N、M）却不是保存在Kconfig文件中的。Kconfig文件是不变的，Kconfig文件只是决定有没有这个菜单项，并不管这个菜单项的选择结果。
(2)menuconfig工作时在我们make menuconfig打开时，他会读取.config文件，并且用.config文件中的配置选择结果来初始化menuconfig中各个菜单项的选择值。

总结：菜单项的项目内容从Kconfig文件来，菜单项的选择值从.config文件来

(3)当我们每次退出make menuconfig时，menuconfig机制会首先检查我们有没有更改某些配置项的值，如果我们本次没有更改过任意一个配置项目的值那直接退出；如果我们有改动配置项的值则会提示我们是否保存。此时如果点保存，则会将我们更改过的配置重新写入.config文件中记录，下一次再次打开make menuconfig时会再次加载.config，最终去编译内核时编译连接程序会考虑.config中的配置值指导整个编译连接过程。

总结：本节课主要内容就是讲：menuconfig和Kconfig和.config的关系。


2.15.7.Kconfig文件详解1
2.15.7.1、Kconfig的格式
(1)Kconfig按照一定的格式来书写，menuconfig程序可以识别这种格式，然后从中提取出有效信息组成menuconfig中的菜单项。
(2)将来在做驱动移植等工作时，有时需要自己添加Kconfig中的一个配置项来将某个设备驱动添加到内核的配置项目中，这时候就需要对Kconfig的配置项格式有所了解，否则就不会添加。
(3)#开头的行是注释行
(4)menuconfig表示菜单（本身属于一个菜单中的项目，但是他又有子菜单项目）、config表示菜单中的一个配置项（本身并没有子菜单下的项目）。
(5)menuconfig或者config后面空格隔开的大写字母表示的类似于 NETDEVICES 的就是这个配置项的配置项名字，这个字符串前面添加CONFIG_后就构成了.config中的配置项名字。
(6)一个menuconfig后面跟着的所有config项就是这个menuconfig的子菜单。这就是Kconfig中表示的目录关系。
(7)内核源码目录树中每一个Kconfig都会source引入其所有子目录下的Kconfig，从而保证了所有的Kconfig项目都被包含进menuconfig中。这个也告诉我们：如果你自己在linux内核中添加了一个文件夹，一定要在这个文件夹下创建一个Kconfig文件，然后在这个文件夹的上一层目录的Kconfig中source引入这个文件夹下的Kconfig文件。

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














































