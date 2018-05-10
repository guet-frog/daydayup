

第二部分、章节介绍
2.6.1.start_armboot函数简介
	本节课简要介绍start_armboot函数，回顾uboot启动第一阶段的同时展望了第二阶段要做的事情，并且分析了uboot启动最终的归宿。
2.6.2.start_armboot解析1
	本节开始解析start_armboot函数，主要解析了init_fnc_t和DECLARE_GLOBAL_DATA_PTR这两个表达式。
2.6.3.内存使用排布
	本节接应上节讲的gd和bd，介绍uboot中设计的内存使用和排布方法。
2.6.4.start_armboot解析2
	本节开始分析init_sequence，并从头开始分析其中的板级初始化函数。
2.6.5.start_armboot解析3
	本节主要讲解board_init中的DDR配置和初始化部分。
2.6.6.start_armboot解析4
	本节主要讲解interrupt_init和env_init，分别是定时器初始化和环境变量的初始化。
2.6.7.start_armboot解析5
	本节主要讲解init_baudrate和serial_init两个函数，都是串口相关的初始化。
2.6.8.start_armboot解析6
	本节主要讲解console_init_f、display_banner和print_cpuinfo这两个初始化函数，分别是控制台初始化、打印启动logo和cpu信息的函数。
2.6.9.start_armboot解析7
	本节介绍了check_board函数，同时重点给大家演示了如何进行uboot的代码实践和学习。
2.6.10.start_armboot解析8
	本节介绍了init_sequence中最后的2个dram相关初始化函数，并且对整个init_sequence中所有初始化函数做了总结和回顾。
2.6.11.start_armboot解析9
	本节讲解flash初始化和堆管理器内存初始化两个函数，并且进行了代码实践。
2.6.12.start_armboot解析10
	本节主要讲解MMC卡初始化相关代码，这是对驱动学习最好的启蒙。
2.6.13.start_armboot解析11
	本节主要讲解env_relocate函数，详细介绍了环境变量在uboot的整体实现。
2.6.14.start_armboot解析12
	本节主要讲解IP地址、MAC地址的获取及确定，devices_init和jumptable的初始化。
2.6.15.start_armboot解析13
	本节主要讲解console_init_f、enable_interrupts、board_late_init这几个初始化函数。
2.6.16.start_armboot解析14
	本节讲解剩下的几个初始化函数，并且结束了start_armboot函数。
2.6.17.uboot启动2阶段总结
	本节对本课程中讲到的start_armboot函数进行整体总结回顾，帮助大家加深记忆。

第三部分、随堂记录
2.6.1.start_armboot函数简介
2.6.1.1、一个很长的函数
(1)这个函数在uboot/lib_arm/board.c的第444行开始到908行结束。
(2)450行还不是全部，因为里面还调用了别的函数。
(3)为什么这么长的函数，怎么不分成两三个函数？主要因为这个函数整个构成了uboot启动的第二阶段。

2.6.1.2、一个函数组成uboot第二阶段

2.6.1.3、宏观分析：uboot第二阶段应该做什么
(1)概括来讲uboot第一阶段主要就是初始化了SoC内部的一些部件（譬如看门狗、时钟），然后初始化DDR并且完成重定位
(2)由宏观分析来讲，uboot的第二阶段就是要初始化剩下的还没被初始化的硬件
    主要是SoC外部硬件（譬如iNand、网卡芯片····）、uboot本身的一些东西（uboot的命令、环境变量等····）
    然后最终初始化完必要的东西后进入uboot的命令行准备接受命令

2.6.1.4、思考：uboot第二阶段完结于何处？
(1)uboot启动后自动运行打印出很多信息（这些信息就是uboot在第一和第二阶段不断进行初始化时，打印出来的信息）
    然后uboot进入了倒数bootdelay秒然后执行bootcmd对应的启动命令。
(2)如果用户没有干涉则会执行bootcmd进入自动启动内核流程（uboot就死掉了）
    此时用户可以按下回车键打断uboot的自动启动进入uboot的命令行下。然后uboot就一直工作在命令行下
(3)uboot的命令行就是一个死循环，循环体内不断重复：接收命令、解析命令、执行命令。这就是uboot最终的归宿

2.6.2.start_armboot解析1
2.6.2.1、init_fnc_t
(1)typedef int (init_fnc_t) (void);	这是一个函数类型
(2)init_fnc_ptr是一个二重函数指针，回顾高级C语言中讲过：二重指针的作用有2个（其中一个是用来指向一重指针）
    一个是用来指向指针数组。因此这里的init_fuc_ptr可以用来指向一个函数指针数组

// 控制台是基于串口, 控制台可以实现标准输入输出

2.6.2.DECLARE_GLOBAL_DATA_PTR   // 结构体指针会不会重复定义?
(1)#define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("r8")
    定义了一个全局变量名字叫gd,这个全局变量是一个指针类型，占4字节
    用volatile修饰表示可变的，用register修饰表示这个变量要尽量放到寄存器中，
    后面的asm("r8")是gcc支持的一种语法，意思就是要把gd放到寄存器r8中。
(2)综合分析，DECLARE_GLOBAL_DATA_PTR就是定义了一个要放在寄存器r8中的全局变量，名字叫gd，
     类型是一个指向gd_t类型变量的指针。
(3)为什么要定义为register？因为这个全局变量gd（global data的简称）是uboot中很重要的一个全局变量
    （准确的说这个全局变量是一个结构体，里面有很多内容，这些内容加起来构成的结构体就是uboot中常用的所有的全局变量）
    这个gd在程序中经常被访问，因此放在register中提升效率。因此纯粹是运行效率方面考虑，和功能要求无关。并不是必须的。
(4)gd_t定义在include/asm-arm/global_data.h中。
    gd_t中定义了很多全局变量，都是整个uboot使用的；其中有一个bd_t类型的指针，指向一个bd_t类型的变量，
    这个bd是开发板的板级信息的结构体，里面有不少硬件相关的参数，譬如波特率、IP地址、机器码、DDR内存分布


2.6.3.内存使用排布
2.6.3.1、为什么要分配内存
(1)DECLARE_GLOBAL_DATA_PTR只能定义了一个指针，也就是说gd里的这些全局变量并没有被分配内存，
    我们在使用gd之前要给他分配内存，否则gd也只是一个野指针而已。
(2)gd和bd需要内存，内存当前没有被人管理（因为没有操作系统统一管理内存），
    大片的DDR内存散放着可以随意使用（只要使用内存地址直接去访问内存即可）。
    但是因为uboot中后续很多操作还需要大片的连着内存块，因此这里使用内存要本着够用就好，紧凑排布的原则。
    所以我们在uboot中需要有一个整体规划。

2.6.3.2、内存排布
(1)uboot区	CFG_UBOOT_BASE-xx（长度为uboot的实际长度）
(2)堆区		长度为CFG_MALLOC_LEN，实际为912KB
(3)栈区		长度为CFG_STACK_SIZE，实际为512KB
(4)gd		长度为sizeof(gd_t)，实际36字节
(5)bd		长度为sizeof(bd_t)，实际为44字节左右
(6)内存间隔		为了防止高版本的gcc的优化造成错误。

强制类型转换  指针 -> 变量(内存)
实例化: 类 -> 对象

gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));     // bd->bi_baudrate在低地址  xxx-sizeof(bd_t) 首地址已经在低地址了

2.6.4.start_armboot解析2
2.6.4.1、for循环执行init_sequence
(1)init_sequence是一个函数指针数组，数组中存储了很多个函数指针，这些指向指向的函数都是init_fnc_t类型
（特征是接收参数是void类型，返回值是int）
(2)init_sequence在定义时就同时给了初始化，初始化的函数指针都是一些函数名。
（C语言高级专题中讲过：函数名的实质）
(3)init_fnc_ptr是一个二重函数指针(可以赋值 指针数组 数组名 not 数组名地址)，可以指向init_sequence这个函数指针数组。
(4)用for循环肯定是想要去遍历这个函数指针数组（遍历的目的也是去依次执行这个函数指针数组中的所有函数）
    思考：如何遍历一个函数指针数组？有2种方法：第一种也是最常用的一种，用下标去遍历，用数组元素个数来截至
    第二种不常用，但是也可以。就是在数组的有效元素末尾放一个标志，依次遍历到标准处即可截至（有点类似字符串的思路）。
    我们这里使用了第二种思路。因为数组中存的全是函数指针，因此我们选用了NULL来作为标志。
    我们遍历时从开头依次进行，直到看到NULL标志截至。这种方法的优势是不用事先统计数组有多少个元素。
(5)init_fnc_t的这些函数的返回值定义方式一样的，都是：函数执行正确时返回0，不正确时返回-1.
    所以我们在遍历时去检查函数返回值，如果遍历中有一个函数返回值不等于0则 hang()/*挂起*/
    从分析hang函数可知：uboot启动过程中初始化板级硬件时不能出任何错误，只要有一个错误整个启动就终止
    除了重启开发板没有任何办法。
(6)init_sequence中的这些函数，都是board级别的各种硬件初始化。

2.6.4.2、cpu_init
(1)看名字这个函数应该是cpu内部的初始化，所以这里是空的。
2.6.4.3、board_init
(1)board_init在uboot/board/samsung/x210/x210.c中，这个看名字就知道是x210开发板相关的初始化。
(2)DECLARE_GLOBAL_DATA_PTR在这里声明是为了后面使用gd方便。
    可以看出把gd的声明定义成一个宏的原因就是我们要到处去使用gd，因此就要到处声明，定义成宏比较方便。
(3)网卡初始化。CONFIG_DRIVER_DM9000这个宏是x210_sd.h中定义的，这个宏用来配置开发板的网卡的。
    dm9000_pre_init函数就是对应的DM9000网卡的初始化函数。开发板移植uboot时，如果要移植网卡，主要的工作就在这里。
(4)这个函数中主要是网卡的GPIO和端口的配置，而不是驱动。因为网卡的驱动都是现成的正确的，
    移植的时候驱动是不需要改动的，关键是这里的基本初始化。因为这些基本初始化是硬件相关的。


2.6.5.start_armboot解析3
2.6.5.1 gd->bd->bi_arch_number
(1)bi_arch_number是board_info中的一个元素，含义是：开发板的机器码。所谓机器码就是uboot给这个开发板定义的一个唯一编号。
(2)机器码的主要作用就是在uboot和linux内核之间进行比对和适配。
(3)嵌入式设备中每一个设备的硬件都是定制化的，不能通用。嵌入式设备的高度定制化导致硬件和软件不能随便适配使用。这就告诉我们：这个开发板移植的内核镜像绝对不能下载到另一个开发板去，否则也不能启动，就算启动也不能正常工作，有很多隐患。因此linux做了个设置：给每个开发板做个唯一编号（机器码），然后在uboot、linux内核中都有一个软件维护的机器码编号。然后开发板、uboot、linux三者去比对机器码，如果机器码对上了就启动，否则就不启动（因为软件认为我和这个硬件不适配）。
(4)MACH_TYPE在x210_sd.h中定义，值是2456，并没有特殊含义，只是当前开发板对应的编号。这个编号就代表了x210这个开发板的机器码，将来这个开发板上面移植的linux内核中的机器码也必须是2456，否则就启动不起来。
(5)uboot中配置的这个机器码，会作为uboot给linux内核的传参的一部分传给linux内核，内核启动过程中会比对这个接收到的机器码，和自己本身的机器码相对比，如果相等就启动，如果不想等就不启动。
(6)理论上来说，一个开发板的机器码不能自己随便定。理论来说有权利去发放这个机器码的只有uboot官方，所以我们做好一个开发板并且移植了uboot之后，理论上应该提交给uboot官方审核并发放机器码（好像是免费的）。但是国内的开发板基本都没有申请（主要是因为国内开发者英文都不行，和国外开源社区接触比较少），都是自己随便编号的。随便编号的问题就是有可能和别人的编号冲突，但是只要保证uboot和kernel中的编号是一致的，就不影响自己的开发板启动。

// pc机设备 标准化
// 嵌入式设备 定制化  软硬件可裁剪

2.6.5.2 gd->bd->bi_boot_params
(1)bd_info中另一个主要元素，bi_boot_params表示uboot给linux kernel启动时的传参的内存地址。也就是说uboot给linux内核传参的时候是这么传的：uboot事先将准备好的传参（字符串，就是bootargs）放在内存的一个地址处（就是bi_boot_params），然后uboot就启动了内核（uboot在启动内核时真正是通过寄存器r0 r1 r2来直接传递参数的，其中有一个寄存器中就是bi_boot_params）。内核启动后从寄存器中读取bi_boot_params就知道了uboot给我传递的参数到底在内存的哪里。然后自己去内存的那个地方去找bootargs。
(2)经过计算得知：X210中bi_boot_params的值为0x30000100，这个内存地址就被分配用来做内核传参了。所以在uboot的其他地方使用内存时要注意，千万不敢把这里给淹没了。

背景：关于DDR的配置：
(1)board_init中除了网卡的初始化之外，剩下的2行用来初始化DDR。
(2)注意：这里的初始化DDR和汇编阶段lowlevel_init中初始化DDR是不同的。当时是硬件的初始化，目的是让DDR可以开始工作。现在是软件结构中一些DDR相关的属性配置、地址设置的初始化，是纯软件层面的。
(3)软件层次初始化DDR的原因：对于uboot来说，他怎么知道开发板上到底有几片DDR内存，每一片的起始地址、长度这些信息呢？在uboot的设计中采用了一种简单直接有效的方式：程序员在移植uboot到一个开发板时，程序员自己在x210_sd.h中使用宏定义去配置出来板子上DDR内存的信息，然后uboot只要读取这些信息即可。（实际上还有另外一条思路：就是uboot通过代码读取硬件信息来知道DDR配置，但是uboot没有这样。实际上PC的BIOS采用的是这种）
(4)x210_sd.h的496行到501行中使用了标准的宏定义来配置DDR相关的参数。主要配置了这么几个信息：有几片DDR内存、每一片DDR的起始地址、长度。这里的配置信息我们在uboot代码中使用到内存时就可以从这里提取使用（想象uboot中使用到内存的地方都不是直接用地址数字的，都是用宏定义的）


2.6.6.start_armboot解析4
2.6.6.1、interrupt_init
(1)看名字函数是和中断初始化有关的，但是实际上不是，实际上这个函数是用来初始化定时器的（实际使用的是Timer4）。
(2)裸机中讲过：210共有5个PWM定时器。其中Timer0-timer3都有一个对应的PWM信号输出的引脚。而Timer4没有引脚，无法输出PWM波形。Timer4在设计的时候就不是用来输出PWM波形的（没有引脚，没有TCMPB寄存器），这个定时器被设计用来做计时。
(3)Timer4用来做计时时要使用到2个寄存器：TCNTB4、TCNTO4。TCNTB中存了一个数，这个数就是定时次数（每一次时间是由时钟决定的，其实就是由2级时钟分频器决定的）。我们定时时只需要把定时时间/基准时间=数，将这个数放入TCNTB中即可；我们通过TCNTO寄存器即可读取时间有没有减到0，读取到0后就知道定的时间已经到了。
(4)使用Timer4来定时，因为没有中断支持，所以CPU不能做其他事情同时定时，CPU只能使用轮询方式来不断查看TCNTO寄存器才能知道定时时间到了没。因为Timer4的定时是不能实现微观上的并行。uboot中定时就是通过Timer4来实现定时的。所以uboot中定时时不能做其他事（考虑下，典型的就是bootdelay，bootdelay中实现定时并且检查用户输入是用轮询方式实现的，原理参考裸机中按键章节中的轮询方式处理按键）
(5)interrupt_init函数将timer4设置为定时10ms。关键部位就是get_PCLK函数获取系统设置的PCLK_PSYS时钟频率，然后设置TCFG0和TCFG1进行分频，然后计算出设置为10ms时需要向TCNTB中写入的值，将其写入TCNTB，然后设置为auto reload模式，然后开定时器开始计时就没了。
总结：在学习这个函数时，注意标准代码和之前裸机代码中的区别，重点学会：通过定义结构体的方式来访问寄存器，通过函数来自动计算设置值以设置定时器。

2.6.6.2、env_init
(1)env_init，看名字就知道是和环境变量有关的初始化。
(2)为什么有很多env_init函数，主要原因是uboot支持各种不同的启动介质（譬如norflash、nandflash、inand、sd卡·····），我们一般从哪里启动就会把环境变量env放到哪里。而各种介质存取操作env的方法都是不一样的。因此uboot支持了各种不同介质中env的操作方法。所以有好多个env_xx开头的c文件。实际使用的是哪一个要根据自己开发板使用的存储介质来定（这些env_xx.c同时只有1个会起作用，其他是不能进去的，通过x210_sd.h中配置的宏来决定谁被包含的），对于x210来说，我们应该看env_movi.c中的函数。
(3)经过基本分析，这个函数只是对内存里维护的那一份uboot的env做了基本的初始化或者说是判定（判定里面有没有能用的环境变量）。当前因为我们还没进行环境变量从SD卡到DDR中的relocate，因此当前环境变量是不能用的。
(4)在start_armboot函数中（776行）调用env_relocate才进行环境变量从SD卡中到DDR中的重定位。重定位之后需要环境变量时才可以从DDR中去取，重定位之前如果要使用环境变量只能从SD卡中去读取。


2.6.7.start_armboot解析5
2.6.7.1、init_baudrate
(1)init_baudrate看名字就是初始化串口通信的波特率的。
(2)getenv_r函数用来读取环境变量的值。用getenv函数读取环境变量中“baudrate”的值（注意读取到的不是int型而是字符串类型），然后用simple_strtoul函数将字符串转成数字格式的波特率。
(3)baudrate初始化时的规则是：先去环境变量中读取"baudrate"这个环境变量的值。如果读取成功则使用这个值作为环境变量，记录在gd->baudrate和gd->bd->bi_baudrate中；如果读取不成功则使用x210_sd.h中的的CONFIG_BAUDRATE的值作为波特率。从这可以看出：环境变量的优先级是很高的。

2.6.7.2、serial_init
(1)serial_init看名字是初始化串口的。（疑问：start.S中调用的lowlevel_init.S中已经使用汇编初始化过串口了，这里怎么又初始化？这两个初始化是重复的还是各自有不同？）
(2)SI中可以看出uboot中有很多个serial_init函数，我们使用的是uboot/cpu/s5pc11x/serial.c中的serial_init函数。
(3)进来后发现serial_init函数其实什么都没做。因为在汇编阶段串口已经被初始化过了，因此这里就不再进行硬件寄存器的初始化了。


2.6.8.start_armboot解析6
2.6.8.1、console_init_f
(1)console_init_f是console（控制台）的第一阶段初始化。_f表示是第一阶段初始化，_r表示第二阶段初始化。有时候初始化函数不能一次一起完成，中间必须要夹杂一些代码，因此将完整的一个模块的初始化分成了2个阶段。（我们的uboot中start_armboot的826行进行了console_init_r的初始化）
(2)console_init_f在uboot/common/console.c中，仅仅是对gd->have_console设置为1而已，其他事情都没做。

2.6.8.2、display_banner
(1)display_banner用来串口输出显示uboot的logo
(2)display_banner中使用printf函数向串口输出了version_string这个字符串。那么上面的分析表示console_init_f并没有初始化好console怎么就可以printf了呢？
(3)通过追踪printf的实现，发现printf->puts，而puts函数中会判断当前uboot中console有没有被初始化好。如果console初始化好了则调用fputs完成串口发送（这条线才是控制台）；如果console尚未初始化好则会调用serial_puts(再调用serial_putc直接操作串口寄存器进行内容发送)。
(4)控制台也是通过串口输出，非控制台也是通过串口输出。究竟什么是控制台？和不用控制台的区别？实际上分析代码会发现，控制台就是一个用软件虚拟出来的设备，这个设备有一套专用的通信函数（发送、接收···），控制台的通信函数最终会映射到硬件的通信函数中来实现。uboot中实际上控制台的通信函数是直接映射到硬件串口的通信函数中的，也就是说uboot中用没用控制器其实并没有本质差别。
(5)但是在别的体系中，控制台的通信函数映射到硬件通信函数时可以用软件来做一些中间优化，譬如说缓冲机制。（操作系统中的控制台都使用了缓冲机制，所以有时候我们printf了内容但是屏幕上并没有看到输出信息，就是因为被缓冲了。我们输出的信息只是到了console的buffer中，buffer还没有被刷新到硬件输出设备上，尤其是在输出设备是LCD屏幕时）
(6)U_BOOT_VERSION在uboot源代码中找不到定义，这个变量实际上是在makefile中定义的，然后在编译时生成的include/version_autogenerated.h中用一个宏定义来实现的。

// cpu被串口拖慢 添加缓冲机制

2.6.8.3、print_cpuinfo
(1)uboot启动过程中：
CPU:  S5PV210@1000MHz(OK)
        APLL = 1000MHz, HclkMsys = 200MHz, PclkMsys = 100MHz
        MPLL = 667MHz, EPLL = 96MHz
                       HclkDsys = 166MHz, PclkDsys = 83MHz
                       HclkPsys = 133MHz, PclkPsys = 66MHz
                       SCLKA2M  = 200MHz
Serial = CLKUART 
这些信息都是print_cpuinfo打印出来的。
(2)回顾ARM裸机中时钟配置一章的内容，比对这里调用的函数中计算各种时钟的方法，自己去慢慢分析体会这些代码的原理和实现方法。这就是学习。


2.6.9.start_armboot解析7
2.6.9.1、checkboard
(1)checkboard看名字是检查、确认开发板的意思。这个函数的作用就是检查当前开发板是哪个开发板并且打印出开发板的名字。
2.6.9.2、init_func_i2c
(1)这个函数实际没有被执行，X210的uboot中并没有使用I2C。如果将来我们的开发板要扩展I2C来接外接硬件，则在x210_sd.h中配置相应的宏即可开启。

2.6.9.3、uboot学习实践
(1)对uboot源代码进行完修改（修改内容根据自己的理解和分析来修改）
(2)make distclean然后make x210_sd_config然后make
(3)编译完成得到u-boot.bin，然后去烧录。烧录方法按照裸机第三部分讲的linux下使用dd命令来烧写的方法来烧写。
(4)烧写过程：
第一步：进入sd_fusing目录下		// file mkbl1  ,,,x64
第二步：make clean
第三步：make
第四步：插入sd卡，ls /dev/sd*得到SD卡在ubuntu中的设备号（一般是/dev/sdb，注意SD卡要连接到虚拟机ubuntu中，不要接到windows中）
第五步：./sd_fusing.sh /dev/sdb完成烧录（注意不是sd_fusing2.sh）
(5)总结：uboot就是个庞大点复杂点的裸机程序而已，我们完全可以对他进行调试。调试的方法就是按照上面步骤，根据自己对代码的分析和理解对代码进行更改，然后重新编译烧录运行，根据运行结果来学习。

// bl1_position=1
// uboot_position=49	// video 15′

// bl2_position=9
// uboot_position=57

// 改的内容不涉及到Makefile 不涉及到 x210_sd.h 则直接make可以

2.6.10.start_armboot解析8
2.6.10.1、dram_init
(1)dram_init看名字是关于DDR的初始化。疑问：在汇编阶段已经初始化过DDR了否则也无法relocate到第二部分运行，怎么在这里又初始化DDR？
(2)dram_init都是在给gd->bd里面关于DDR配置部分的全局变量赋值，让gd->bd数据记录下当前开发板的DDR的配置信息，以便uboot中使用内存。
(3)从代码来看，其实就是初始化gd->bd->bi_dram这个结构体数组。

2.6.10.2、display_dram_config
(1)看名字意思就是打印显示dram的配置信息。
(2)启动信息中的：（DRAM:    512 MB）就是在这个函数中打印出来的。
(3)思考：如何在uboot运行中得知uboot的DDR配置信息？
	uboot中有一个命令叫bdinfo，这个命令可以打印出gd->bd中记录的所有硬件相关的全局变量的值，
	因此可以得知DDR的配置信息。
		DRAM bank   = 0x00000000
		-> start    = 0x30000000
		-> size     = 0x10000000
		DRAM bank   = 0x00000001
		-> start    = 0x40000000
		-> size     = 0x10000000

2.6.10.3、init_sequence总结
(1)都是板级硬件的初始化以及gd、gd->bd中的数据结构的初始化。譬如：
网卡初始化、机器码（gd->bd->bi_arch_number）、
内核传参DDR地址（gd->bd->bi_boot_params）、
Timer4初始化为10ms一次、
波特率设置（gd->bd->bi_baudrate和gd->baudrate）、
console第一阶段初始化（gd->have_console设置为1）、
打印uboot的启动信息、
打印cpu相关设置信息、
检查并打印当前开发板名字、
DDR配置信息初始化（gd->bd->bi_dram）、
打印DDR总容量。

总结回顾：本节课结束后已经到了start_armboot的第487行。下节课开始继续往下看。


2.6.11.start_armboot解析9
2.6.11.1、CFG_NO_FLASH
(1)虽然NandFlash和NorFlash都是Flash，但是一般NandFlash会简称为Nand而不是Flash，一般讲Flash都是指的Norflash。这里2行代码是Norflash相关的。
(2)flash_init执行的是开发板中对应的NorFlash的初始化、display_flash_config打印的也是NorFlash的配置信息（Flash:   8 MB就是这里打印出来的）。但是实际上X210中是没有Norflash的。所以着两行代码是可以去掉的（我也不知道为什么没去掉？猜测原因有可能是去掉着两行代码会导致别的地方工作不正常，需要花时间去移植调试，然后移植的人就懒得弄。实际上不去掉除了显示有8MB Flash实际没用之外也没有别的影响）

CONFIG_VFD和CONFIG_LCD是显示相关的，这个是uboot中自带的LCD显示的软件架构。但是实际上我们用LCD而没有使用uboot中设置的这套软件架构，我们自己在后面自己添加了一个LCD显示的部分。

2.6.11.2、mem_malloc_init
(1)mem_malloc_init函数用来初始化uboot的堆管理器。
(2)uboot中自己维护了一段堆内存，肯定自己就有一套代码来管理这个堆内存。有了这些东西uboot中你也可以malloc、free这套机制来申请内存和释放内存。我们在DDR内存中给堆预留了896KB的内存。

2.6.11.3、代码实践，去掉Flash看会不会出错。
结论：加上CONFIG_NOFLASH宏之后编译出错，说明代码移植的不好，那个文件的包含没有被这个宏控制。于是乎移植的人就直接放这没管。


2.6.12.start_armboot解析10
2.6.12.1、开发板独有初始化：mmc初始化
(1)从536到768行为开发板独有的初始化。意思是三星用一套uboot同时满足了好多个系列型号的开发板，然后在这里把不同开发板自己独有的一些初始化写到了这里。用#if条件编译配合CONFIG_xxx宏来选定特定的开发板。
(2)X210相关的配置在599行到632行。
(3)mmc_initialize看名字就应该是MMC相关的一些基础的初始化，其实就是用来初始化SoC内部的SD/MMC控制器的。函数在uboot/drivers/mmc/mmc.c里。
(4)uboot中对硬件的操作（譬如网卡、SD卡···）都是借用的linux内核中的驱动来实现的，uboot根目录底下有个drivers文件夹，这里面放的全都是从linux内核中移植过来的各种驱动源文件。
(5)mmc_initialize是具体硬件架构无关的一个MMC初始化函数，所有的使用了这套架构的代码都掉用这个函数来完成MMC的初始化。mmc_initialize中再调用board_mmc_init和cpu_mmc_init来完成具体的硬件的MMC控制器初始化工作。
(6)cpu_mmc_init在uboot/cpu/s5pc11x/cpu.c中，这里面又间接的调用了drivers/mmc/s3c_mmcxxx.c中的驱动代码来初始化硬件MMC控制器。这里面分层很多，分层的思想一定要有，否则完全就糊涂了。


2.6.13.start_armboot解析11
2.6.13.1、env_relocate
(1)env_relocate是环境变量的重定位，完成从SD卡中将环境变量读取到DDR中的任务。
(2)环境变量到底从哪里来？SD卡中有一些（8个）独立的扇区作为环境变量存储区域的。但是我们烧录/部署系统时，我们只是烧录了uboot分区、kernel分区和rootfs分区，根本不曾烧录env分区。所以当我们烧录完系统第一次启动时ENV分区是空的，本次启动uboot尝试去SD卡的ENV分区读取环境变量时失败（读取回来后进行CRC校验时失败），我们uboot选择从uboot内部代码中设置的一套默认的环境变量出发来使用（这就是默认环境变量）；这套默认的环境变量在本次运行时会被读取到DDR中的环境变量中，然后被写入（也可能是你saveenv时写入，也可能是uboot设计了第一次读取默认环境变量后就写入）SD卡的ENV分区。然后下次再次开机时uboot就会从SD卡的ENV分区读取环境变量到DDR中，这次读取就不会失败了。
(3)真正的从SD卡到DDR中重定位ENV的代码是在env_relocate_spec内部的movi_read_env完成的。


2.6.14.start_armboot解析12
2.6.14.1、IP地址、MAC地址的确定
(1)开发板的IP地址是在gd->bd中维护的，来源于环境变量ipaddr。getenv函数用来获取字符串格式的IP地址，然后用string_to_ip将字符串格式的IP地址转成字符串格式的点分十进制格式。
(2)IP地址由4个0-255之间的数字组成，因此一个IP地址在程序中最简单的存储方法就是一个unsigend int。但是人类容易看懂的并不是这种类型，而是点分十进制类型（192.168.1.2）。这两种类型可以互相转换。

2.6.14.2、devices_init
(1)devices_init看名字就是设备的初始化。这里的设备指的就是开发板上的硬件设备。放在这里初始化的设备都是驱动设备，这个函数本来就是从驱动框架中衍生出来的。uboot中很多设备的驱动是直接移植linux内核的（譬如网卡、SD卡），linux内核中的驱动都有相应的设备初始化函数。linux内核在启动过程中就有一个devices_init(名字不一定完全对，但是差不多)，作用就是集中执行各种硬件驱动的init函数。
(2)uboot的这个函数其实就是从linux内核中移植过来的，它的作用也是去执行所有的从linux内核中继承来的那些硬件驱动的初始化函数。

2.6.14.3、jumptable_init
(1)jumptable跳转表，本身是一个函数指针数组，里面记录了很多函数的函数名。看这阵势是要实现一个函数指针到具体函数的映射关系，将来通过跳转表中的函数指针就可以执行具体的函数。这个其实就是在用C语言实现面向对象编程。在linux内核中有很多这种技巧。
(2)通过分析发现跳转表只是被赋值从未被引用，因此跳转表在uboot中根本就没使用。


2.6.15.start_armboot解析13
2.6.15.1、console_init_r
(1)console_init_f是控制台的第一阶段初始化，console_init_r是第二阶段初始化。实际上第一阶段初始化并没有实质性工作，第二阶段初始化才进行了实质性工作。
(2)uboot中有很多同名函数，使用SI工具去索引时经常索引到不对的函数处（回忆下当时start.S中找lowlevel_init.S时，自动索引找到的是错误的，真正的反而根本没找到。）
(3)console_init_r就是console的纯软件架构方面的初始化（说白了就是去给console相关的数据结构中填充相应的值），所以属于纯软件配置类型的初始化。
(4)uboot的console实际上并没有干有意义的转化，它就是直接调用的串口通信的函数。所以用不用console实际并没有什么分别。（在linux内console就可以提供缓冲机制等不用console不能实现的东西）。

2.6.15.2、enable_interrupts
(1)看名字应该是中断初始化代码。这里指的是CPSR中总中断标志位的使能。
(2)因为我们uboot中没有使用中断，因此没有定义CONFIG_USE_IRQ宏，因此我们这里这个函数是个空壳子。
(3)uboot中经常出现一种情况就是根据一个宏是否定义了来条件编译决定是否调用一个函数内部的代码。uboot中有2种解决方案来处理这种情况：方案一：在调用函数处使用条件编译，然后函数体实际完全提供代码。方案二：在调用函数处直接调用，然后在函数体处提供2个函数体，一个是有实体的一个是空壳子，用宏定义条件编译来决定实际编译时编译哪个函数进去。

2.6.15.3、loadaddr、bootfile两个环境变量
(1)这两个环境变量都是内核启动有关的，在启动linux内核时会参考这两个环境变量的值。

2.6.15.4、board_late_init
(1)看名字这个函数就是开发板级别的一些初始化里比较晚的了，就是晚期初始化。所以晚期就是前面该初始化的都初始化过了，剩下的一些必须放在后面初始化的就在这里了。侧面说明了开发板级别的硬件软件初始化告一段落了。
(2)对于X210来说，这个函数是空的。


2.6.16.start_armboot解析14
2.6.16.1、eth_initialize
(1)看名字应该是网卡相关的初始化。这里不是SoC与网卡芯片连接时SoC这边的初始化，而是网卡芯片本身的一些初始化。
(2)对于X210（DM9000）来说，这个函数是空的。X210的网卡初始化在board_init函数中，网卡芯片的初始化在驱动中。

2.6.16.2、x210_preboot_init（LCD和logo显示）
(1)x210开发板在启动起来之前的一些初始化，以及LCD屏幕上的logo显示。

2.6.16.3、check menukey to update from sd
(1)uboot启动的最后阶段设计了一个自动更新的功能。就是：我们可以将要升级的镜像放到SD卡的固定目录中，然后开机时在uboot启动的最后阶段检查升级标志（是一个按键。按键中标志为"LEFT"的那个按键，这个按键如果按下则表示update mode，如果启动时未按下则表示boot mode）。如果进入update mode则uboot会自动从SD卡中读取镜像文件然后烧录到iNand中；如果进入boot mode则uboot不执行update，直接启动正常运行。
(2)这种机制能够帮助我们快速烧录系统，常用于量产时用SD卡进行系统烧录部署。

2.6.16.4、死循环
(1)解析器
(2)开机倒数自动执行
(3)命令补全


2.6.17.uboot启动2阶段总结
2.6.17.1、启动流程回顾、重点函数标出
(1)第二阶段主要是对开发板级别的硬件、软件数据结构进行初始化。
(2)
	init_sequence
		cpu_init	空的
		board_init	网卡、机器码、内存传参地址
			dm9000_pre_init			网卡
			gd->bd->bi_arch_number	机器码
			gd->bd->bi_boot_params	内存传参地址
		interrupt_init	定时器
		env_init
		init_baudrate	gd数据结构中波特率
		serial_init		空的
		console_init_f	空的
		display_banner	打印启动信息
		print_cpuinfo	打印CPU时钟设置信息
		checkboard		检验开发板名字
		dram_init		gd数据结构中DDR信息
		display_dram_config	打印DDR配置信息表
	mem_malloc_init		初始化uboot自己维护的堆管理器的内存
	mmc_initialize		inand/SD卡的SoC控制器和卡的初始化
	env_relocate		环境变量重定位
	gd->bd->bi_ip_addr	gd数据结构赋值
	gd->bd->bi_enetaddr	gd数据结构赋值
	devices_init		空的
	jumptable_init		不用关注的
	console_init_r		真正的控制台初始化
	enable_interrupts	空的
	loadaddr、bootfile 	环境变量读出初始化全局变量
	board_late_init		空的
	eth_initialize		空的
	x210_preboot_init	LCD初始化和显示logo
	check_menu_update_from_sd	检查自动更新
	main_loop			主循环

2.6.17.2、启动过程特征总结
(1)第一阶段为汇编阶段、第二阶段为C阶段
(2)第一阶段在SRAM中、第二阶段在DRAM中
(3)第一阶段注重SoC内部、第二阶段注重SoC外部Board内部

2.6.17.3、移植时的注意点
(1)x210_sd.h头文件中的宏定义
(2)特定硬件的初始化函数位置（譬如网卡）




















