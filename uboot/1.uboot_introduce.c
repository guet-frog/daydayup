

2.1.3.uboot必须解决哪些问题
	本节分析uboot必须解决的一些问题，这些问题也正是uboot中的主要代码
2.1.4.uboot的工作方式
	本节主要介绍uboot的命令行工作方式，重点强调了命令和环境变量这两个关键点。
2.1.5.uboot的常用命令1
	本节开始介绍uboot的常用命令。首先讲了uboot命令的一些特征如行缓冲、命令别名、参数等然后讲了print命令。
2.1.6.uboot的常用命令2
	本节讲述uboot常用命令中设置、保存环境变量的命令、ping命令。
2.1.7.开发板和主机的ping通
	本节详细介绍开发板和主机windows、虚拟机ubuntu的ping通及设置。
2.1.8.uboot的常用命令3
	本节继续介绍uboot的常用命令，主要是tftp和nfs命令，这两个命令都是uboot用来和主机进行网络连接获取文件相关的。
2.1.9.uboot的常用命令4
	本节主要介绍了uboot操作iNand的movi命令和操作内存的mm、mw、md命令
2.1.10.uboot的常用环境变量1
	本节总结了uboot的环境的原理和使用方法，并以bootdelay等环境变量来演示这种使用效果。
2.1.11.uboot的常用环境变量2
	本节主要讲解uboot的2个重要环境变量bootcmd和bootargs。
2.1.12.uboot中对Flash和DDR的管理
	本节讲述uboot对Flash和DDR的分区管理，其中最重要的是Flash的分区。



2.1.1.为什么要有uboot
    2.2.1.1、计算机系统的主要部件
        (1)计算机系统就是用CPU来做核心进行运行的系统	// warning: PC System
        (2)PC System core: CPU + 内部存储器(DDR/SDRAM/SRAM) + 外部存储器(Flash/硬盘)

	2.2.1.2、PC机的启动过程
		(1)典型的/*PC机的部署*/: BIOS在PC机主板上, OS在硬盘上, 内存在掉电时无作用, CPU在掉电时不工作
		(2)启动过程: PC上电后先执行BIOS程序(in NorFlash), 
			// BIOS init DDR && disk, copy OS to DDR, then jump to DDR to exe OS

	2.2.1.3、典型嵌入式linux系统启动过程
		(1)embedded system deployment && boot is refer to PC	// warning: system deployment
		(2)嵌入式系统的部署: uboot程序部署在flash/*能作为启动设备的Flash*/, OS在FLash, 内存CPU在掉电时不工作
		(3)启动过程: 嵌入式系统上电后先执行uboot, 然后uboot负责init ddr && flash, 
						然后将OS从Flash中读取到DDR中, 然后启动OS(OS启动后uboot就无用了)
	//embedded system boot && pc system boot (BIOS->uboot, HardDisk->Flash)

	2.2.1.4、android系统启动过程
		(1)android系统的启动和linux系统几乎一样, 只是在内核启动后加载根文件系统后不同了
		(2)可以认为启动分为2个阶段: (1)uboot到OS启动; (2)OS启动后到rootfs加载到命令行执行

	2.2.1.5、总结：uboot的作用
		(1)uboot主要作用/*启动操作系统内核*/
		(2)uboot其他作用: 部署整个计算机系统, 操作flash等板子上硬盘的驱动, 提供一个命令行界面 

	2.1.2.2、uboot的发展历程
		(1)uboot: universal BootLoader
		(2)事实上的业内bootloader标准, 大部分的嵌入式设备都会默认使用uboot来做为bootloader
		(3)早期uboot版本号: /*uboot1.3.4*/
		   后来uboot版本号: /*uboot-2010.06*/
		(4)//uboot的核心部分几乎没怎么变化, 越新的版本支持的开发板越多而已

2.1.3.uboot必须解决哪些问题
	2.1.3.1、自身可开机直接启动
		(1)一般的SoC都支持多种启动方式, 譬如SD卡启动, NorFlash启动, NandFlash启动等
			uboot要能够开机启动, 必须根据具体的SoC的启动设计来设计uboot
		(2)uboot必须进行和硬件相对应的代码级别的更改和移植, 才能够保证可以从相应的启动介质启动
			uboot中第一阶段的start.S文件中具体处理了这一块

	2.1.3.2、能够引导操作系统内核启动并给内核传参
		(1)uboot的终极目标就是/*启动内核*/
		(2)linux内核在设计的时候, 设计为可以被传参
			uboot中事先给linux内核准备一些启动参数/*放在内存中特定位置*/然后传给内核 // ShareMemory
			内核启动后会到这个特定位置去取uboot传给他的参数
			然后在内核中解析这些参数, 这些参数将被用来指导linux内核的启动过程

	2.1.2.3、能提供系统部署功能
		(1)uboot完成整个系统(包括uboot、kernel、rootfs等镜像)在flash上的烧录下载工作
		(2)刷机利用uboot中的fastboot功能将各种镜像烧录到iNand中, 然后从iNand启动

	2.1.2.4、能进行soc级和板级硬件管理
		(2)SoC级(譬如串口)就是SoC内部外设, 板级就是SoC外面开发板上面的硬件(譬如网卡、iNand)

	2.1.2.5、uboot的“生命周期”
		uboot入口, 开机ompin选择何种启动方式, 则uboot就先放在哪里
		//选择dnw启动就将uboot事先烧录到ram中

2.1.4.uboot的工作方式
	2.1.4.1、从裸机程序镜像uboot.bin说起

	2.1.4.2、uboot的命令式shell界面
		(2)有些程序需要进行交互, 程序中就需要实现了一个shell, uboot就实现了一个shell
			注意: 	shell并不是操作系统, 和操作系统一点关系都没有
					linux中打开一个终端后就得到了一个shell, 可以输入命令回车执行
					// uboot command no "ls"

	2.1.4.3、掌握uboot使用的2个关键点：命令和环境变量
		(1)uboot下的shell: uboot部署系统, 设置环境变量, 启动内核
		(3)uboot shell -> linux shell, environment variable -> environment variable, driver management
		(4)环境变量可以被认为是/*system global variable*/
			环境变量名都是系统内置的
			（认识就认识，不认识就不认识，这部分是系统自带的默认的环境变量，譬如PATH
			但是也有一部分环境变量是自己添加的, 自己添加的系统就不认识但是我们自己认识
			系统或者我们自己的程序在运行时可以通过读取环境变量来指导程序的运行
			这样设计的好处就是灵活，譬如我们要让一个程序更改运行方法，
			不用去重新修改程序代码再重新编译运行，而只要修改相应的环境变量就可以了。
		(5)环境变量就是运行时的配置属性
			uboot的环境变量和操作系统的环境变量工作原理和方式几乎完全相同

2.1.5.uboot的常用命令1
	2.1.5.1、类似linux终端的行缓冲命令行
		(2)linux终端设计有3种缓冲机制：无缓冲、行缓冲、全缓冲  //回车键(换行, 行缓冲)

	2.1.5.2、有些命令有简化的别名
		(1)譬如printenv命令可以简化为print, 譬如setenv可以简化为set

	2.1.5.3、有些命令会带参数（注意格式是固定的）

	2.1.5.4、命令中的特殊符号
		(1)/*单引号*/将这个很长且中间有空格隔开的参数引起来

	2.1.5.5、有些命令是一个命令族（譬如movi）
		(1)同一个命令关键字, 参数不同, 功能和作用不同, 譬如movi都和moviNand(EMMC, iNand)操作有关

	2.1.5.5、第一个命令：printenv/print
		(1)print(无参数): 打印出系统中所有的环境变量
		(2)环境变量 ≈ 全局变量
			//环境变量被存储在flash的另一块专门区域(flash上环境变量分区)
			
2.1.6.uboot的常用命令2
	2.1.6.1、设置（添加/更改）环境变量：
		(1)/*设置/添加*/环境变量: set name value	// setenv/set
		(2)/*保存*/环境变量: saveenv/save			// 块设备属性所决定, CR600可能考虑到传输参数的开销


2.1.7.2、开发板运行linux下和虚拟机ubuntu的ping通
(1)在linux基础课中讲过：虚拟机的网卡设置可以选择好几种方式，常用的就是NAT和桥接（bridged）。
(2)虚拟机要和开发板进行网络通信，只能通过桥接方式连接。
(3)虚拟机要想被开发板ping通，设置步骤如下：
第一步：虚拟机设置成桥接方式。
第二步：虚拟机的菜单中有个“虚拟网络编辑器”，这里面要设置为桥接到有线网卡。（默认是自动的，自动的一般会影响ping通。因为电脑现在一般都有2个网卡：一个有线的一个无线的。如果选了自动，那么虚拟机会自动桥接到无线网卡上，但是我们却是通过有线网卡来连接开发板的，自然ping不通）
第三步：在虚拟机ubuntu中设置IP地址为192.168.1.141（可以通过/etc/network/interfaces文件来设置静态的然后重启；也可以直接命令行ifconfig去设置）
(4)此时开发板ping虚拟机ubuntu应该就通了。
(5)此时虚拟机ubuntu中ping开发板也是通的。

2.1.7.3、开发板运行uboot下和主机Windows的ping通
(1)刚才开发板运行linux时和主机windows、虚拟机ubuntu都ping通了，说明硬件和连接和主机设置没错。
(2)此时开发板重启进入uboot，设置好ipaddr、gatewayip，然后去ping windows发现还是不通。 怀疑uboot本身网络驱动有问题。
(3)然后同样情况下尝试去ping通虚拟机ubuntu，理论分析应该也不通，但是实际发现是通的。

2.1.7.4、开发板运行uboot下和虚拟机ubuntu的ping通
(1)uboot和虚拟机ubuntu互相ping通（前提是虚拟机ubuntu设置为桥接，且桥接到有线网卡，且ip地址设置正确的情况下）
结论：开发板中运行的uboot有点小bug，ping windows就不通，ping虚拟机ubuntu就通。


2.1.8.uboot常用命令3
2.1.8.1、tftp下载指令：tftp
(1)uboot本身主要目标是启动内核，为了完成启动内核必须要能够部署内核，uboot为了部署内核就需要将内核镜像从主机中下载过来然后烧录到本地flash中。uboot如何从主机（windows或者虚拟机ubuntu）下载镜像到开发板上？有很多种方式，主流方式是：fastboot和tftp。
fastboot的方式是通过USB线进行数据传输。
tftp的方式是通过有线网络的。典型的方式就是通过网络，fastboot是近些年才新发展的。
(2)tftp方式下载时实际上uboot扮演的是tftp客户端程序角色，主机windows或虚拟机ubuntu中必须有一个tftp服务器，然后将要下载的镜像文件放在服务器的下载目录中，然后开发板中使用uboot的tftp命令去下载即可。
(3)有些人习惯在windows中搭建tftp服务器，一般是用一些软件来搭建（譬如tftpd32，使用起来比较简单）；有些人习惯在linux下搭建tftp服务器，可以参考网盘中的虚拟机下载目录下的一个教程《嵌入式开发环境搭建-基于14.04.pdf》，这里面有ubuntu中搭建tftp服务器的教程，也可以自己上网搜索教程尝试。（如果你直接就用我的虚拟机，那就已经搭建好了，不用再搭建了；如果是自己新装的那就参考文档搭建；如果你的版本和我的不一样那搭建过程可能不一样）
(4)我的虚拟机搭建的时候设置的tftp下载目录是/tftpboot，将要被下载的镜像复制到这个目录下。
(5)检查开发板uboot的环境变量，注意serverip必须设置为虚拟机ubuntu的ip地址。（serverip这个环境变量的意义就是主机tftp服务器的ip地址）
(6)然后在开发板的uboot下先ping通虚拟机ubuntu，然后再尝试下载：tftp 0x30000000 zImage-qt（意思是将服务器上名为zImage-qt的文件下载到开发板内存的0x30000000地址处。）
(7)镜像下载到开发板的DDR中后，uboot就可以用movi指令进行镜像的烧写了。
注意：
1）如果你是用的windows下的tftp服务器，那uboot的serverip就要设置为和windwos下tftp服务器的ip地址一样（windows下的tftp服务器软件设置的时候就有个步骤是让你设置服务器的ip地址，这个ip地址和主机windows必须在一个网段）。
2）整个过程中间环节比较多，实际做的时候可能最后会下载不下来。这时候可能的问题非常多，不要问我，自己对照视频课程讲的思路来排查。（譬如：第一步应该先保证uboot和ubuntu可以ping通；第二步再保证ubuntu中tftp服务器搭建没错；第三步再实现tftp下载。如果第一步有问题参考网络设置部分，第二步有问题（tftp本地测试下载ok，但是开发板就是不行），有一个解决方案就是使用windows下的tftp服务器）

2.1.8.2、nfs启动内核命令：nfs
(1)uboot中也支持nfs命令，但是我基本没用过。


2.1.9.uboot的常用命令4
    2.1.9.1 SD卡/iNand操作指令movi
        (1)开发板如果用SD卡/EMMC/iNand等作为Flash，则在uboot中操作flash的指令为movi（或mmc）
            movei init S3C_HSMMC-0
        (2)movi指令是一个命令集，有很多子命令，具体用法可以help movi查看。
        (3)movi的指令都是movi read和movi write一组的, movi read用来读取iNand到DDR上
            movi write用来将DDR中的内容写入iNand中。理解这些指令时一定要注意涉及到的2个硬件：iNand和DDR内存。
        (4)movi read  {u-boot | kernel} {addr}   
            这个命令使用了一种通用型的描述方法来描述: movi和read外面没有任何标记说明每一次使用这个指令都是必选的；
            一对大括号{}括起来的部分必选1个, 大括号中的竖线表是多选一
            中括号[]表示可选参数(可以有也可以没有)
        (5)指令有多种用法，譬如 movi read u-boot 0x30000000  // inand 中 u-boot分区 (起始地址 + 终止地址)
            把iNand中的u-boot分区读出到DDR的0x30000000起始的位置处
            (uboot代码中将iNand分成了很多个分区, 每个分区有地址范围和分区名
            uboot程序操作中可以使用直接地址来操作iNand分区, 也可以使用分区名来操作分区
            //uboot shell下默认是十六进制
    
    2.1.9.2、NandFlash操作指令nand
        (1)理解方法和操作方法完全类似于movi指令

    2.1.9.3、内存操作指令：mm、mw、md
        (1)*/DDR中是没有分区的(只听说过对硬盘、Flash进行分区)
            但是内存使用时要注意，千万不能越界踩到别人了
            uboot是一个裸机程序，操作系统会由系统整体管理所有内存，系统负责分配和管理，系统会保证内存不会随便越界
            然后裸机程序中uboot并不管理所有内存，内存是散的随便用的，
            所以如果程序员（使用uboot的人）自己不注意就可能出现自己把自己的数据给覆盖了
            //思考: dnw(usb刷机时)uboot放在23E00000地址处
            uboot中实际链接地址在0x30000000中(可能使uboot中开启了虚拟地址映射)
            1. uboot中源码对内存地址的处理方式 2. 虚拟地址映射表, 如何建立页表映射

        (2)md: memory display   显示内存中的内容
        (3)mw: memory write     将内容写到内存中
        (4)mm: memory modify    修改内存中(批量 逐个单元 修改内存)

    2.1.9.4、启动内核指令：bootm、go
        (1)uboot的革命理想: 启动内核, 启动内核在uboot中表现为一个指令(never get back)
        (2)差别: bootm启动内核同时给内核传参, 而go命令启动内核不传参
                 go命令内部其实就是一个函数指针指向一个内存地址然后直接调用那个函数
                 go命令的实质就是PC直接跳转到一个内存地址去运行
                 调试裸机程序的方法就是事先启动uboot，然后在uboot中去下载裸机程序，用go命令去执行裸机程序

2.1.10.uboot的常用环境变量1
    2.1.10.2环境变量如何参与程序运行
        (1)environment variable in DDR && ROM
        (2)环境变量在uboot中是用/*字符串*/表示，uboot是按照/*字符匹配*/的方式来区分各个环境变量 // ASCII码
            
    2.1.10.3自动运行倒数时间：bootdelay

    2.1.10.4网络设置：ipaddr、serverip 
        (1)ipaddr
        (2)serverip is for tftp指令
        (3)gatewayip
        (4)netmask
        (5)ethaddr

2.1.11.uboot的常用环境变量2
    2.1.11.1、自动运行命令设置：bootcmd
        (1)uboot启动后会开机自动倒数bootdelay秒, uboot会自动执行启动命令来启动内核
        (2)uboot开机自动启动时实际就是在内部执行了bootcmd这个环境变量的值所对应的/*命令集*/
        (3)bootcmd=movi read kernel 0x30008000; bootm 0x30008000    // read kernel && bl
        (4)set bootcmd printenv // for test
        (5)set bootcmd 'movi read kernel 30008000; bootm 30008000'  // warning ''

    2.1.11.2、uboot给kernel传参：bootargs
        (1)kernel启动时可以接收uboot传递的启动参数(约定好格式内容)
           kernel在这些启动参数的指导下完成启动过程(内核可以在不重新编译的情况下采用不同的方式启动)
        (2)uboot的环境变量中/*设置bootargs*/ 然后bootm命令启动kernel时会/*自动将bootargs传给kernel*/
        (3)'bootargs=console=ttySAC2,115200 root=/dev/mmcblk0p2 rw init=/linuxrc rootfstype=ext3'
            'console=ttySAC2,115200'	控制台使用串口2, 波特率115200
            'root=/dev/mmcblk0p2 rw'	rootfs在SD卡端口0设备(iNand)第2分区, 根文件系统是可读可写
            'init=/linuxrc'			    linux的进程1（init进程）的路径
            'rootfstype=ext3'			根文件系统的类型是ext3
        (4)内核传参非常重要(不传参或传参错误导致内核启动失败)

    2.1.11.3、新建、更改、删除一个环境变量的方法
        (1)新建一个环境变量，使用set var value
        (2)更改一个环境变量，使用set var value
        (3)删除一个环境变量，使用set var

    2.1.11.4、注意：环境变量更改后的保存

    2.1.12.uboot中对Flash和DDR的管理
        2.1.12.1、uboot阶段Flash的分区
            (1)分区: Flash进行分块管理 // uboot var kernel rootfs
            (2)PC整个硬盘由OS统一管理, OS使用文件系统管理硬盘空间
            (3)uboot运行时需要手动管理flash分区, 对Flash的管理必须事先使用/*分区界定*/
                /*实际上在uboot中和kernel中都有个分区表*/(分区表: 系统移植时对Flash的整体管理分配方法)
                在部署系统时按照/*分区表*/中分区界定方法来部署
            (4)分区方法不是固定的, 可以变动的, 一般在设计系统移植时就会定好：
                // sd卡启动时：烧录脚本中seek=1
                uboot分区的大小必须保证uboot肯定能放下，一般设计为512KB或者1MB
                环境变量：环境变量分区一般紧贴着uboot来存放，大小为32KB或者更多一点。
                kernel：kernel大小一般为3MB或5MB
                rootfs：······
                剩下的就是自由分区，一般kernel启动后将自由分区挂载到rootfs下使用

            总结: 
            (1)各分区彼此相连
            (3)uboot必须在Flash开头/*soc决定*/, 其他分区相对位置是可变 //SOC bootROM决定
            (5)分区在系统移植前确定好, 在uboot中和kernel中使用同一个分区表
                将来在/*系统部署时和系统代码*/中的分区方法也必须一样

            //fastboot通过系统分区表来部署系统(或者通过手动设置内存地址)

    2.1.12.2、uboot阶段DDR的分区
        (3)内存分区关键就在于内存中哪一块用来干什么必须分配好
            以避免各个不同功能使用了同一块内存造成的互相踩踏
            譬如: tftp 0x23E00000 zImage error //uboot image in here



























































