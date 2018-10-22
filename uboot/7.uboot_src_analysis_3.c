
	// cmd_bootm.c -- do_bootm()	// 启动内核

2.7.1.2、内核本质也是一个"裸机程序"	 // zImage
	(2)操作系统运行起来后在软件上分为内核层和应用层, 分层后权限不同(内存访问和设备操作的管理)
	   内核可以任意访问各种硬件, 而应用程序只能被限制的访问硬件和内存地址

2.7.1.3、部署在sd卡中特定分区内
	掉电时: bootloader、kernel、rootfs等以/*镜像*/的形式存储在启动介质中的/*指定分区*/
	启动时: 运行启动代码进行相关的硬件初始化和/*软件架构*/的建立, 加载镜像到ddr中, 最终达到运行时稳定状态
	运行时: 在ddr中运行, 与存储介质无关
	// uboot和kernel中的分区表必须一致, 同时和sd卡的实际使用的分区要一致

2.7.1.4、运行时必须先加载到ddr中/*链接地址*/处
	(1)uboot在step1进行重定位, 将step2(整个uboot)加载到ddr中	// uboot link addr:  0x33E0_0000 -- MMU映射表
	(2)uboot启动内核时将zImage加载到ddr中(kernel relocate)		// kernel link addr: 0x3000_8000

2.7.1.5、内核启动需要必要的启动参数
	(2)内核启动需要: ①image relocate ②boot args	// uboot可以自启动或bootROM引导

2.7.2.0、启动内核第一步：加载内核到ddr中		// read kernel to ddr not uboot

2.7.2.1、静态内核镜像在哪里？
	(1)SD卡/iNand/Nand/NorFlash 		// raw分区
		uboot读取命令: movi or nand
	(2)movi read kernel 0x30008000		// kernel分区

	// bootcmd ""		// env
	// bootm ""	''

	(3)tftp、nfs等网络下载方式从远端服务器获取镜像		// uboot支持远程启动  -- 通过网络从server下载kernel

	// fastboot 命令 -- 查看分区起始地址 + 长度, 没有指明则默认接连上一个分区
	// android 默认分区较多, 典型linux只有3个分区: bootloader kernel rootfs
	// help -- movi

2.7.3.2、vmlinuz和zImage和uImage
	(1) uboot经过编译直接生成的elf格式的可执行程序是u-boot		// 900+ KByte
		elf类似于windows下的exe格式, 在/*操作系统下*/可以直接执行, elf格式不能用来烧录下载
		/*u-boot.bin*/: 镜像(image)由u-boot使用arm-linux-objcopy工具制作, 用来烧录下载

	(2) linux内核编译后生成可执行程序vmlinux或vmlinuz(/boot/vmlinuz-3.13.0-32-generic)	// 原始的内核elf文件
		使用objcopy工具制作嵌入式系统部署时的烧录镜像文件Image	// 无.bin	-- 78M -> 7.5M

	(3) Image可以直接烧录到flash上启动执行
		/*自解压*/kernel: Image压缩后文件的前端附加一段解压缩代码, 构成了一个压缩格式的镜像就叫zImage

	(4) uboot默认支持uImage启动		// x210_sd.h -- #define CONFIG_ZIMAGE_BOOT
		uboot使用mkimage工具生成uImage, 在zImage头部添加64byte的uImage的头信息	// /tools/mkimage.c

		// ubuntu用grob来启动 -- 可以直接加载elf格式镜像
		// du -h linux
		// linux file 命令

2.7.3.3、编译内核得到uImage去启动
	(1)如果直接在kernel下make uImage会提示mkimage command not found
		uboot/tools下cp mkimage /usr/local/bin/(一般为用户自己添加的程序文件, 直接copy过去)复制mkimage工具到系统目录下

2.7.4.1、LINUX_ZIMAGE_MAGIC		// 首先第一步将kernel放置到ddr固定位置
	(2) 命令 bootm 0x30008000，所以do_boom的argc=2 argv[0]=bootm argv[1]=0x30008000
		但是实际bootm命令还可以不带参数执行。如果不带参数直接bootm，
		则会从CFG_LOAD_ADDR地址去执行(定义在x210_sd.h中)
	(3)zImage头部开始的第37-40字节处存放着zImage标志魔数，从这个位置取出然后对比LINUX_ZIMAGE_MAGIC
		可以用二进制阅读软件来打开zImage查看，就可以证明
		// winhex
		// *(ulong *)(addr + 9*4) == LINUX_ZIMAGE_MAGIC	// unsigned long

2.7.4.2、image_header_t
(1)这个数据结构是我们uboot启动内核使用的一个标准启动数据结构，zImage头信息也是一个image_header_t，但是在实际启动之前需要进行一些改造。hdr->ih_os = IH_OS_LINUX;
		hdr->ih_ep = ntohl(addr);这两句就是在进行改造。
(2)images全局变量是do_bootm函数中使用，用来完成启动过程的。zImage的校验过程其实就是先确认是不是zImage，确认后再修改zImage的头信息到合适，修改后用头信息去初始化images这个全局变量，然后就完成了校验。


2.7.5.uImage启动
2.7.5.1、uImage启动
(1)LEGACY(遗留的)，在do_bootm函数中，这种方式指的就是uImage的方式。
(2)uImage方式是uboot本身发明的支持linux启动的镜像格式，但是后来这种方式被一种新的方式替代，
	这个新的方式就是设备树方式（在do_bootm方式中叫FIT）
(3)uImage的启动校验主要在boot_get_kernel函数中，主要任务就是校验uImage的头信息，并且得到真正的kernel的起始位置去启动。

// uImage -> uImage_format_legacy
// -- IMAGE_FORMAT_LEGACY

2.7.5.2、设备树方式内核启动	// new, libfdt based format
(1)设备树方式启动暂时不讲，课程结束后会用补充专题的方式来讲解（很多类似的知识点都会这样处理，譬如前面讲的MMU）

总结1：uboot本身设计时只支持uImage启动，原来uboot的代码也是这样写的。
	后来有了fdt方式之后，就把uImage方式命令为LEGACY方式，fdt方式命令为FIT方式，于是乎多了写#if #endif添加的代码。
	后来移植的人又为了省事添加了zImage启动的方式，又为了省事把zImage启动方式直接写在了uImage和fdt启动方式之前，
	于是乎又有了一对#if  #endif
总结2：第二阶段校验头信息结束，下面进入第三阶段，第三阶段主要任务是启动linux内核，调用do_bootm_linux函数来完成。

	// linux kernel 更新快
	// linux kernel driver 经典书籍 ldd3 Linux内核驱动3(很多事例代码在当前kernel已经编译不过) -- 使用2.6早期内核(与2.6晚期内核差异非常大)(2.6晚期内核与3.0早期内核也有一定差异)
	// 2.6晚期内核使用量多, 3.x 4.x

	// 完成校验进入step3
	// 根据镜像头中os种类, 执行do_bootm_linux(), 传参通过&images(把镜像启动所需要的有效信息存储在images)

2.7.6.2、镜像的entrypoint
(1)ep就是entrypoint的缩写，就是程序入口。
	一个镜像文件的起始执行部分不是在镜像的开头（镜像开头有n个字节的头信息），
	真正的镜像文件执行时第一句代码在镜像的中部某个字节处，相当于头是有一定的偏移量的。
	这个偏移量记录在头信息中。
(2)一般执行一个镜像都是：第一步先读取头信息，然后在头信息的特定地址找MAGIC_NUM，由此来确定镜像种类；
			第二步对镜像进行校验；
			第三步再次读取头信息，由特定地址知道这个镜像的各种信息（镜像长度、镜像种类、入口地址）；
			第四步就去entrypoint处开始执行镜像。
(3)theKernel = (void (*)(int, int, uint))ep;将ep赋值给theKernel
	则这个函数指向就指向了内存中加载的OS镜像的真正入口地址（就是操作系统的第一句执行的代码）。

2.7.6.3、机器码的再次确定
(1)uboot在启动内核时，机器码要传给内核。uboot传给内核的机器码是怎么确定的？第一顺序备选是环境变量machid，第二顺序备选是gd->bd->bi_arch_num（x210_sd.h中硬编码配置的）

2.7.6.4、传参并启动概述
(1)从110行到144行就是uboot在给linux内核准备传递的参数处理。
(2)Starting kernel ...  这个是uboot中最后一句打印出来的东西。
	这句如果能出现，说明uboot整个是成功的，也成功的加载了内核镜像，也校验通过了，也找到入口地址了，也试图去执行了
	如果这句后串口就没输出了，说明内核并没有被成功执行。原因一般是：传参（80%）、内核在DDR中的加载地址·······


2.7.7.传参详解
2.7.7.1、tag方式传参
(1)struct tag，tag是一个数据结构，在uboot和linux kernel中都有定义tag数据机构，而且定义是一样的。
(2)tag_header和tag_xxx。tag_header中有这个tag的size和类型编码，kernel拿到一个tag后先分析tag_header得到tag的类型和大小，然后将tag中剩余部分当作一个tag_xxx来处理。
(3)tag_start与tag_end。kernel接收到的传参是若干个tag构成的，这些tag由tag_start起始，到tag_end结束。
(4)tag传参的方式是由linux kernel发明的，kernel定义了这种向我传参的方式，uboot只是实现了这种传参方式从而可以支持给kernel传参。

2.7.7.2、x210_sd.h中配置传参宏
(1)CONFIG_SETUP_MEMORY_TAGS，tag_mem，传参内容是内存配置信息。
(2)CONFIG_CMDLINE_TAG，tag_cmdline，传参内容是启动命令行参数，也就是uboot环境变量的bootargs.
(3)CONFIG_INITRD_TAG
(4)CONFIG_MTDPARTITION，传参内容是iNand/SD卡的分区表。
(5)起始tag是ATAG_CORE、结束tag是ATAG_NONE，其他的ATAG_XXX都是有效信息tag。
思考：内核如何拿到这些tag？
	uboot最终是调用theKernel函数来执行linux内核的，uboot调用这个函数（其实就是linux内核）时传递了3个参数。
	这3个参数就是uboot直接传递给linux内核的3个参数，通过寄存器来实现传参的。
	（第1个参数就放在r0中，第二个参数放在r1中，第3个参数放在r2中）第1个参数固定为0，第2个参数是机器码，第3个参数传递的就是大片传参tag的首地址。

2.7.7.3、移植时注意事项
(1)uboot移植时一般只需要配置相应的宏即可
(2)kernel启动不成功，注意传参是否成功。传参不成功首先看uboot中bootargs设置是否正确，
	其次看uboot是否开启了相应宏以支持传参。

	// 首先烧写了nand版本的uboot, flash中的环境变量被更新
	// 再次烧写inand版本的uboot, bootargs仍是nand版本的, 导致不能正常启动

2.7.8.uboot启动内核的总结
2.7.8.1、启动4步骤	
第一步：将内核搬移到DDR中		// flash(local or remote) -> ddr(链接地址处)
第二步：校验内核格式、CRC等		// 校验ddr中内核镜像格式
第三步：准备传参
第四步：跳转执行内核			// r0 r1 r2传参：0、机器码、tag(0x20003000)
2.7.8.2、涉及到的主要函数是：do_bootm(内核搬移, 头检验, CRC校验)和do_bootm_linux(传参的准备工作, 跳转执行)
2.7.8.3、uboot能启动的内核格式: zImage(很多uboot默认不支持zImage启动) uImage fdt方式
2.7.8.4、跳转与函数指针的方式运行内核


// C语言字符串解析
// AT命令使用argc来确定传参个数判定 -- 解析命令不同 执行命令时的传参不同
// default env与current env的区别
// env的排布方法
// set xxx用来








