
1.5.2.1、C语言运行时需要和栈的意义
	"C语言运行时(runtime)"需要一定的条件, 主要是需要栈, 这些条件由汇编来提供
	C语言的局部变量是用栈来实现的, 如果汇编部分没有给C部分预先设置/*合法的*/栈地址, 非法的内存访问
	单片机代码的栈是由开发环境设置(set msp)
	应用程序中编写的C程序并不是全部, 编译器(gcc)在链接的时候会自动添加一个头(汇编实现的代码, 来引导C程序执行(设置栈或其他C运行时环境)) // -- PE头?

1.5.2.2、CPU模式和各种模式下的栈
	在ARM中37个寄存器中, 每种模式下都有自己的独立的SP寄存器(r13) -- // cortex-m 有多少个通用寄存器 

1.5.2.3、查阅文档并设置栈指针至合法位置
	栈必须是当前一段/*可用的*/内存	//初始化过可以访问的内存, 只会被用作栈, 不会被其他程序使用
	当前CPU刚复位(刚启动), 外部的ddr未初始化, 内部的sram不需要初始化可以直接使用	// 0xD003_7D80
	
	栈有四种：满减栈 满增栈 空减栈 空增栈
	满栈：进栈：先移动指针再存； 出栈：先出数据再移动指针
	空栈：xxx
	减栈：进栈：指针向下移动；	 出栈：指针向上移动
	增栈：xxx

1.5.2.4、汇编程序和C程序互相调用
	bl cfuncion
	启动代码一般叫start.s	// 其他文件名, _start修改命名?

1.5.3.1、C函数的编写和被汇编调用
	// 汇编调用C, 传参比较麻烦

1.5.3.2、使用C语言来访问寄存器的语法
	// IO与内存统一编址, 访问寄存器类似于访问内存地址, C语言访问内存地址通过指针

1.5.3.3、神奇的volatile
	volatile的作用是让程序在编译时, 编译器不对程序该变量做优化
	
1.5.3.4、总结：
	C和汇编函数的互相调用(函数名和汇编标号的真实意义)
	C语法对内存访问的封装方式(使用指针来访问内存的技巧)
	汇编的意义(起始代码&效率关键部位)

1.5.3.5、编译报错（实际上是连接阶段报错）：undefined reference to `__aeabi_unwind_cpp_pr1'	
	解决：在编译时添加-nostdlib这个编译选项即可解决。nostdlib就是不使用标准函数库。
		标准函数库就是编译器中自带的函数库，用-nostdlib可以让编译器链接器优先选择我程序内自己写的函数库。


1.5.4.1、什么是cache，有什么用
	cache是一种内存，叫高速缓存。
	容量: CPU <　寄存器 < cache < DDR
	速度: CPU >  寄存器 > cache > DDR

	icache: 清缓存, 重新缓存
	dcache: 缓存数据, dcache与MMU有关, 必须开MMU虚拟地址才能够使用

1.5.4.3、汇编代码读写cp15以开关icache
	mrc p15,0,r0,c1,c0,0;			// 读出cp15的c1到r0中
	bic r0, r0, #(1<<12)			// bit12 置0  关icache
	orr r0, r0, #(1<<12)			// bit12 置1  开icache
	mcr p15,0,r0,c1,c0,0;

1.5.5.1、一个事实：大部分指令是位置有关编码
	链接地址: 链接时指定的地址(Makefile中用-Ttext, 或者链接脚本)
	运行地址: 程序实际运行时地址

1.5.5.3、再解S5PV210的启动过程：三星推荐和uboot的实现是不同的
	三星推荐的启动方式中：bootloader必须小于96KB并大于16KB，假定bootloader为80KB，
	启动过程是这样子：先开机上电后BL0运行，BL0会加载外部启动设备中的bootloader的前16KB（BL1）到SRAM中去运行，
	BL1运行时会加载BL2（bootloader中80-16=64KB）到SRAM中（从SRAM的16KB处开始用）去运行；
	BL2运行时会初始化DDR并且将OS搬运到DDR去执行OS，启动完成。
	uboot实际使用的方式：uboot大小随意，假定为200KB。
	启动过程是这样子：先开机上电后BL0运行，BL0会加载外部启动设备中的uboot的前16KB（BL1）到SRAM中去运行，
	BL1运行时会初始化DDR，然后将整个uboot搬运到DDR中，
	然后用一句长跳转（从SRAM跳转到DDR）指令从SRAM中直接跳转到DDR中继续执行uboot直到uboot完全启动
	uboot启动后在uboot命令行中去启动OS

	// 重定位原因: uboot, kernel的链接地址在ddr中, ddr初始化后需要将flash中的uboot, kernel重定位到ddr中

	分散加载: 把uboot分成2部分（BL1和整个uboot），两部分分别指定不同的链接地址
	启动时将两部分加载到不同的地址（BL1加载到SRAM，整个uboot加载到DDR），这时候不用重定位也能启动。
	分散加载其实相当于手工重定位。重定位是用代码来进行重定位，分散加载是手工操作重定位的。


1.5.6.2、链接地址由什么决定？
	链接地址在编译链接的过程中, 通过Makefile中-Ttext xxx或者在链接脚本中指定 // 其中-Ttext xxx优先级高

	举例：linux中的应用程序, gcc hello.c -o hello, 这时使用默认的链接地址0x0, 所以应用程序都是链接在0x0地址
		  因为应用程序运行在操作系统的一个进程中, 在这个进程中这个应用程序独享4G的虚拟地址空间, 所以应用程序都可以链接到0x0地址
		  因为每个进程都是从0地址开始, 编译时可以不给定链接地址而都使用0x0

1.5.6.3、从源码到可执行程序的步骤：预编译、编译、链接、strip
	预编译 : 预编译器	//C中的宏定义, 注释, #include
	编译   : 编译器		//源码.c .S编译为机器码.o文件(编程以函数为单位, 编译也是以函数为单位)
	链接   : 链接器		//.o文件中的各函数(段)按照一定规则(链接脚本来指定)链接为可执行文件
	strip  : 			//strip是把可执行程序中的符号信息给拿掉, 以节省空间(Debug版本和Release版本)
	objcopy:			//由可执行程序生成可烧录的镜像bin文件

	编译链接的过程中会产生很多符号信息, 如:函数名相关符号会输出一个表格(函数表), 所以在反汇编的时候能找到每一个函数的名字
	程序在运行的时候符号表不起任何作用, 符号表可以用来反向调试
	如: Linux内核如果启动失败会打印一个OOPS信息, 打印信息里面就会包含符号, 它可以指示在哪个内存地址error, 以及这个内存地址对应的符号
	//如果没有符号表, 则通过内存地址表示

1.5.6.4、程序段的概念：代码段、数据段、bss段（ZI段）、自定义段
	整个可执行程序的所有内容被分成了一个一个的段, 链接脚本用段名将所用的段放在可执行程序的合适位置	// -- 全局变量会导致bin文件变化
	
	段名分为2种:
	(1)编译器链接器内部指定
		代码段(.text), 数据段(.data), bss段(.bss)

	(2)自定义段名	// 段的属性和特征也需要定义?
	
	//C语言中对全局变量和静态局部变量的处理
	
1.5.6.5、链接脚本究竟要做什么？
	(1)链接脚本是个规则文件, 链接器会参考链接脚本, 并且使用其中规定的规则来处理.o文件中那些段, 将其链接成一个可执行程序
	(2)链接脚本的关键内容: 段名, 地址

1.5.8.2、思路：
	ldr pc, =led_blink		// 长跳转执行0xD002_4000位置的led_blink
	bl  led_blink			// 短跳转执行0xD002_0010位置的led_blink		// 没有 =
	
	//test current PC

	当链接地址和运行地址相同时, 短跳转和长跳转实际效果是一样
	但是当链接地址不等于运行地址时, 短跳转和长跳转就有差异		// 短跳转执行运行地址code; 长跳转执行链接地址code

1.5.9.代码重定位实战2	//Start.S中重定位部分汇编代码讲解
1.5.9.1、adr与ldr伪指令的区别
	ldr和adr都是伪指令，区别是ldr是长加载、adr是短加载。
	重点：adr指令加载符号地址，加载的是运行时地址；ldr指令在加载符号地址时，加载的是链接地址。

	//adr和ldr分别用于加载运行地址和链接地址，从而可以判断是否需要重定位即可

1.5.9.2、重定位（代码拷贝）
	重定位就是汇编代码中的copy_loop函数，代码的作用是使用循环结构来逐句复制代码到链接地址。
	复制的源地址是SRAM的0xd0020010，复制目标地址是SRAM的0xd0024000，复制长度是bss_start减去_start
	所以复制的长度就是整个重定位需要重定位的长度，也就是整个程序中代码段+数据段的长度。
	bss段（bss段中就是0初始化的全局变量）不需要重定位。
1.5.9.3、清bss段
	清除bss段是为了满足C语言的运行时要求
	（C语言要求显式初始化为0的全局变量，或者未显式初始化的全局变量的值为0，实际上C语言编译器就是通过清bss段来实现C语言的这个特性的）
	一般情况下我们的程序是不需要负责清零bss段的
	（C语言编译器和链接器会帮我们的程序自动添加一段头程序，这段程序会在我们的main函数之前运行，这段代码就负责清除bss）
	但是在我们代码重定位了之后，因为编译器帮我们附加的代码只是帮我们清除了运行地址那一份代码中的bss，
	而未清除重定位地址处开头的那一份代码的bss，所以重定位之后需要自己去清除bss。

	【你自己写的程序 main函数是程序的开始，编译器除了清bss,还有别的】
	【反汇编文件注释...】

1.5.9.4、长跳转

1.5.10.2、SDRAM的特性（容量大、价格低、掉电易失性、随机读写、总线式访问）
	//SDRAM: Syncronized Dynamic Ramdam Access Memory(同步动态随机存储器)	//DDR: double rate

1.5.10.3、SDRAM数据手册带读
	SDRAM通过地址总线和数据总线接口与SoC通信	// 总线接口

	//开发板原理图上使用的是K4T1G164QQ, 但是实际开发板上贴的不是这个, 但是这两款是完全兼容

	K4T1G164QE:
		K: Samsung, 4: DRAM, T: 产品号码, 1G: 容量(bit 128MB), 16: 单芯片16位宽, 4: 8 bank

1.5.11.SDRAM初始化
1.5.11.1、原理图中SDRAM相关部分
	S5PV210共有2个内存端口（就好象有2个内存插槽）。再结合查阅数据手册中内存映射部分，可知：两个内存端口分别叫DRAM0和DRAM1：
		DRAM0：内存地址范围：0x20000000～0x3FFFFFFF（512MB），对应引脚是Xm1xxxx
		DRAM1: 内存地址范围：0x40000000～0x7FFFFFFF（1024MB），对应引脚是Xm2xxxx
	结论：
	(1)整个210最多支持内存: 1.5GB
	(2)X210开发板512MB内存, DRAM0端口256MB, DRAM1端口256MB
	(3)由2可知，X210开发板上内存合法地址是: 0x20000000-0x2FFFFFFF + 0x40000000-0x4FFFFFFF

	原理图中每个DDR端口都由3类总线构成：地址总线（Xmn_ADDR0~XMnADDR13共14根地址总线） + 控制总线（中间部分，自己看原理图） + 数据总线（Xmn_DATA0~XMnDATA31共32根数据线）
	分析：从数据总线的位数可以看出，我们用的是32位的（物理）内存。

	原理图中画出4片内存芯片的一页，可以看出：X210开发板共使用了4片内存（每片1Gb=128MB，共512MB）
	每片内存的数据总线都是16位的（单芯片是16位内存）
	如何由16位内存得到32位内存呢？可以使用并联方法。
	在原理图上横向的2颗内存芯片就是并联连接的。
	并联时地址总线接法一样，但是数据总线要加起来。这样连接相当于在逻辑上可以把这2颗内存芯片看成是一个（这一个芯片是32位的，接在Xm1端口上）。

	// 物理上是两片16位内存, 逻辑上是一片32位内存
	
1.5.11.2、数据手册中SDRAM相关部分
	看数据手册《NT5TU64M16GG-DDR2-1G-G-R18-Consumer》第10页的block diagram。这个框图是128Bb×8结构的，这里的8指的是8bank，每bank128Mbit。
	210的DDR端口信号中有BA0～BA2，接在内存芯片的BA0～BA2上，这些引脚就是用来选择bank的。
	每个bank内部有128Mb，通过row address（14位） + column address（10位）的方式来综合寻址。
	一共能寻址的范围是：2的14次方+2的10次方 = 2的24次方。对应16MB（128Mbit）内存

1.5.12.汇编初始化SDRAM详解1
1.5.12.1、初始化代码框架介绍（函数调用和返回、步骤等）
	SDRAM初始化使用一个函数sdram_asm_init，函数在sdram_init.S文件中实现，是一个汇编函数。
	强调：汇编实现的函数在返回时需要明确使用返回指令（mov pc, lr）
1.5.12.2、27步初始化DDR2
(1)首先，DDR初始化和SoC（准确说是和SoC中的DDR控制器）有关，也和开发板使用的DDR芯片有关，和开发板设计时DDR的连接方式也有关。
(2)S5PV210的DDR初始化步骤在SoC数据手册：1.2.1.3 DDR2这个章节。可知初始化DDR共需27个步骤。
(3)之前分析过X210的内存连接方式是：在DRAM0上连接256MB，在DRAM1上连接了256MB。所以初始化DRAM时分为2部分，第一部分初始化DRAM0，第二部分初始化DRAM1.
(4)我们的代码不是自己写的，这个代码来自于：第一，九鼎官方的uboot中；第二，参考了九鼎的裸机教程中对DDR的初始化；第三，有些参数是我根据自己理解修改过的。
1.5.12.3、设置IO端口驱动强度
因为DDR芯片和S5PV210之间是通过很多总线连接的，总线的物理表现就是很多个引脚，也就是说DDR芯片和S5PV210芯片是通过一些引脚连接的。
DDR芯片工作时需要一定的驱动信号，这个驱动信号需要一定的电平水平才能抗干扰，所以需要设置这些引脚的驱动能力，使DDR正常工作。
DRAM控制器对应的引脚设置为驱动强度2X（我也不知道为什么是2X，什么时候设置成3X 4X?，这东西只能问DDR芯片厂商或者SoC厂商，我们一般是参考原厂给的代码）

【ldr r0, =ELFIN_GPIO_BASE】
【ldr r1, =0x0000AAAA】
【str r1, [r0, #MP1_0DRV_SR_OFFSET]】基址+变址  寻址  【类似于 数组名+偏移量】

1.5.12.4、DRAM port 时钟设置
从代码第128行到154行。主要是开启DLL（DRAM PLL）然后等待锁存。
这段代码对应27步中的第2到第4步。

1.5.13.汇编初始化SDRAM详解2

// CONCONTROL控制器时序相关

// memory burst length 一次读几个字节

1.5.13.1、DMC0_MEMCONTROL
	burst length=4，1chip，······  对应值是0x00202400
1.5.13.2、DMC0_MEMCONFIG_0
	DRAM0通道中memory chip0的参数设置寄存器		//	chip_mask决定address range
1.5.13.3、DMC0_MEMCONFIG_1
	DRAM0通道中memory chip1的参数设置寄存器
总结：我猜测（推论）：三星设置DRAM0通道，允许我们接2片256MB的内存，分别叫memory chip0和memory chip1，分别用这两个寄存器来设置它的参数。
按照三星的设计，chip0的地址应该是0x20000000到0x2FFFFFFF，然后chip1的地址应该是0x30000000~0x3FFFFFFF.各自256MB。
但是我们X210开发板实际在DRAM0端口只接了256MB的内存，所以只用了chip0，没有使用chip1.
（我们虽然是2片芯片，然后这两片是并联形成32位内存的，逻辑上只能算1片）。
按照这个推论，DMC0_MEMCONFIG_0有用，而DMC0_MEMCONFIG_1无用，所以我直接给他了默认值。
1.5.13.4、DMC_DIRECTCMD
这个寄存器是个命令寄存器，我们210通过向这个寄存器写值来向DDR芯片发送命令（通过命令总线），这些命令应该都是用来配置DDR芯片工作参数。

总结：DDR配置过程比较复杂，基本上是按照DDR控制器的时序要求来做的，其中很多参数要结合DDR芯片本身的参数来定，还有些参数是时序参数，要去详细计算。
所以DDR配置非常繁琐、细致、专业。所以我们对DDR初始化的态度就是：学会这种思路和方法，结合文档和代码能看懂，会算一些常见的参数即可。

























