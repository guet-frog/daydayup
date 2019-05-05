
2.20.1.X210的bsp介绍
2.20.1.1、嵌入式linux产品的bsp介绍
	(1) 大部分的ARM架构的linux平台的bsp的内容和结构都是相似的。
	(2) bsp一般是芯片厂家/板卡厂家提供的。

2.20.1.2、X210的linux+QT bsp整体介绍
	(1) tslib_x210_qtopia.tgz是用来支持QT的触摸屏操作的应用层库
	(2) xboot和uboot是X210支持的2个bootloader源代码。kernel文件夹中是内核源代码，
		buildroot文件夹是用来构建根文件系统的文件夹。tools里是一些有用工具。
	(2) mk脚本是用来管理和编译整个bsp的。

2.20.1.3、mk的帮助信息
	(1) linux下的惯例就是，执行程序时加-h或者--help就可以看到这个程序执行的帮助信息。
	(2) mk脚本的主要作用是编译bsp中的所有的源代码（包括bootloader、kernel、rootfs等），
		但是我们可以完整编译也可以部分编译，我们通过执行mk 后面加不同的参数来指定mk脚本去编译相应的部分。
		譬如：
		mk -a	即可编译所有的bsp源代码
		mk -x	即可只编译xboot
		mk -ui	即可只编译uboot针对inand版本开发板的源代码
		mk -r  	即可只编译buildroot，-r只是得到了文件夹形式的rootfs，并没有将其制作成镜像文件。
		mk -re	即可编译buildroot并且制作得到ext3格式的rootfs镜像
		mk -rj	即可编译buildroot并且制作得到jffs2格式的rootfs镜像

2.20.1.4、工作示例
	(1) 编译uboot		./mk -ui
	(2) 编译内核			

	注：./mk和mk都是执行mk这个脚本文件，区别在于./mk是带路径的，mk是不带路径的。
		还有source mk，这个和前两个的区别是source执行时不需要mk文件具有可执行权限，
		而前面两种要求mk必须在当前用户下具有可执行权限

2.20.2.mk文件分析
2.20.2.1、shell程序结构
	(1) shell脚本程序分为：变量定义、函数、代码。shell脚本程序的结构非常类似于C语言程序
	(2) shell程序和C语言程序很大的一个差别就是shell没有main函数，shell脚本执行时也是先执行主函数的，
		不过主函数没有放在一个类似于main这样的函数中，而是直接放在全局下的一些代码。
	(3) shell程序执行时首先执行变量定义，然后执行主函数，其他函数在主函数代码中被调用执行。
2.20.2.2、主函数
	(1) 上节课说过mk可以编译整个bsp的源代码，也可以只编译其中一部分。
		怎么实现控制编译哪个不编译哪个？实现原理就是：用一个函数来完成编译一个
		（譬如编译内核用build_kernel函数，编译inand的uboot用build_bootloader_uboot_inand），
		然后用相应的一些变量来控制这个函数要不要被编译
		（譬如uboot_inand变量=yes就表示要编译inand版本的uboot，=no就表示不要编译），
		我们编译时通过-xxxxx来传参时，这些传参会影响这些变量的值=yes或者=no
	(2) 如果我们直接./mk并不传参，则$1为空，这时候按照一套默认的配置来编译。
2.20.2.3、各个源文件的编译函数

2.20.3.buildroot引入
2.20.3.1、buildroot作用介绍
	(1) 之前自己从零开始构建根文件系统，一路下来事情还挺多，步骤比较麻烦。
	(2) 交叉编译工具链arm-linux-gcc，我们目前都是从soc官方直接拿来使用的，官方的工具链从何而来？
		实际上交叉编译工具链都是由gcc配置编译生成的，这个配置编译过程比较复杂，
		一般人自己去配置编译得到自己的交叉编译工具链是比较麻烦的，所以经常都是用别人最好的。
	(3) buildroot就是一个集成包，这个包里集成了交叉编译工具链的制作，以及整个rootfs的配置编译过程。
		也就是说，使用buildroot可以很简便的得到一个做好的文件夹形式的根文件系统。
	(4) buildroot将很多东西集成进来后，移植了linux kernel的make xxx_defconfig+make menuconfig的2步配置法，
		我们可以在buildroot的配置界面下完成集成在里边的所有东西的配置，然后直接make就可以最终得到文件夹形式的rootfs。

2.20.3.2、make x210ii_defconfig
	(1) 先make xxx_defconfig
	(2) make menuconfig
	(3) make

2.20.3.3、make及其错误解决
	(1) 直接make会遇到很多错误，这些错误原因都是因为ubuntu中缺乏一些必要软件包造成的。解决方案是先安装这些必要的软件包。
	(2) 编译过程会需要从网上下载一些软件包，因此整个编译过程需要在联网状态下进行。

2.20.2.4、编译后结果查看与分析
	(1) 编译后生成的文件夹格式的rootfs在buildroot/output/images/rootfs.tar。
		我们将其复制到了根目录下的release目录下去，这个文件就是一个完整的可以工作的文件夹形式的rootfs。







