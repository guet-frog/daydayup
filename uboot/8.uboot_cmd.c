
第一部分、章节目录
2.8.1.uboot命令体系基础
2.8.2.uboot命令解析和执行过程分析
2.8.3.uboot如何处理命令集1
2.8.4.uboot如何处理命令集2	
2.8.5.uboot中增加自定义命令	

第二部分、章节介绍
2.8.1.uboot命令体系基础
	本节讲述uboot的命令体系的基础知识，包括uboot命令的实现代码寻找，命令背后对应的执行函数等。
2.8.2.uboot命令解析和执行过程分析
	本节分析uboot中获取、解析和执行命令的过程，从代码角度来追溯一个命令执行的流程。
2.8.3.uboot如何处理命令集1
	本节讲解uboot的命令数据结构和使用链接段属性的方式将命令集中存放并查找的技巧，这种技巧在linux内核中同样有广泛使用。
2.8.4.uboot如何处理命令集2	
	本节通过分析find_cmd函数和version命令的实现函数do_version来进一步分析uboot命令集。
2.8.5.uboot中增加自定义命令	
	本节进行代码实践，动手向uboot中添加自定义命令，并且总结uboot中命令体系的优势就是便于命令的添加和扩展。

第三部分、随堂记录
2.8.1.uboot命令体系基础
2.8.1.1、使用uboot命令
(1)uboot启动后进入命令行环境下，在此输入命令按回车结束，uboot会收取这个命令然后解析，然后执行。
2.8.1.2、uboot命令体系实现代码在哪里
(1)uboot命令体系的实现代码在uboot/common/cmd_xxx.c中。有若干个.c文件和命令体系有关。（还有command.c  main.c也是和命令有关的）

2.8.1.3、每个命令对应一个函数
(1)每一个uboot的命令背后都对应一个函数。这就是uboot实现命令体系的一种思路和方法。这个东西和我们在裸机第十六部分shell中实现shell命令的方法是一样的。
(2)我们要找到每一个命令背后所对应的那个函数，而且要分析这个函数和这个命令是怎样对应起来的。

2.8.1.4、命令参数以argc&argv传给函数
(1)有些uboot的命令还支持传递参数。也就是说命令背后对应的函数接收的参数列表中有argc和argv，然后命令体系会把我们执行命令时的命令+参数（md 30000000 10）以argc（3）和argv（argv[0]=md, argv[1]=30000000 argv[2]=10）的方式传递给执行命令的函数。

举例分析，以help命令为例：
help命令背后对应的函数名叫：do_help。在uboot/common/command.c的236行。int do_help (cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])


2.8.2.uboot命令解析和执行过程分析
2.8.2.1、从main_loop说起
(1)uboot启动的第二阶段，在初始化了所有该初始化的东西后，进入了一个死循环，死循环的循环体就是main_loop。
(2)main_loop函数执行一遍，就是一个获取命令、解析命令、执行命令的过程。
(3)run_command函数就是用来执行命令的函数。

2.8.2.2、run_command函数详解
2.8.2.3、关键点分析
(1)控制台命令获取 
(2)命令解析。parse_line函数把"md 30000000 10"解析成argv[0]=md, argv[1]=30000000 argv[2]=10;
(3)命令集中查找命令。find_cmd(argv[0])函数去uboot的命令集合当中搜索有没有argv[0]这个命令，
(4)执行命令。最后用函数指针的方式调用执行了对应函数。

思考：关键点就在于find_cmd函数如何查找到这个命令是不是uboot的合法支持的命令？这取决于uboot的命令体系机制（uboot是如何完成命令的这一套设计的，命令如何去注册、存储、管理、索引。）。


2.8.3.uboot如何处理命令集1
2.8.3.1、可能的管理方式
(1)数组。结构体数组，数组中每一个结构体成员就是一个命令的所有信息。
(2)链表。链表的每个节点data段就是一个命令结构体，所有的命令都放在一条链表上。这样就解决了数组方式的不灵活。坏处是需要额外的内存开销，然后各种算法（遍历、插入、删除等）需要一定复杂度的代码执行。
(3)有第三种吗？uboot没有使用数组或者链表，而是使用了一种新的方式来实现这个功能。
2.8.3.2、命令结构体cmd_tbl_t
struct cmd_tbl_s {
	char		*name;		/* Command Name			*/
	int		maxargs;	/* maximum number of arguments	*/
	int		repeatable;	/* autorepeat allowed?		*/
					/* Implementation function	*/
	int		(*cmd)(struct cmd_tbl_s *, int, int, char *[]);
	char		*usage;		/* Usage message	(short)	*/
#ifdef	CFG_LONGHELP
	char		*help;		/* Help  message	(long)	*/
#endif
#ifdef CONFIG_AUTO_COMPLETE
	/* do auto completion on the arguments */
	int		(*complete)(int argc, char *argv[], char last_char, int maxv, char *cmdv[]);
#endif
};
typedef struct cmd_tbl_s	cmd_tbl_t;
(1)name：命令名称，字符串格式。
(2)maxargs：命令最多可以接收多少个参数
(3)repeatable：指示这个命令是否可重复执行。重复执行是uboot命令行的一种工作机制，就是直接按回车则执行上一条执行的命令。
(4)cmd：函数指针，命令对应的函数的函数指针，将来执行这个命令的函数时使用这个函数指针来调用。
(5)usage：命令的短帮助信息。对命令的简单描述。
(6)help：命令的长帮助信息。细节的帮助信息。
(7)complete：函数指针，指向这个命令的自动补全的函数。
总结：uboot的命令体系在工作时，一个命令对应一个cmd_tbl_t结构体的一个实例，然后uboot支持多少个命令，就需要多少个结构体实例。uboot的命令体系把这些结构体实例管理起来，当用户输入了一个命令时，uboot会去这些结构体实例中查找（查找方法和存储管理的方法有关）。如果找到则执行命令，如果未找到则提示命令未知。

2.8.3.3、uboot实现命令管理的思路
(1)填充1个结构体实例构成一个命令
(2)给命令结构体实例附加特定段属性（用户自定义段），链接时将带有该段属性的内容链接在一起排列（挨着的，不会夹杂其他东西，也不会丢掉一个带有这种段属性的，但是顺序是乱序的）。
(3)uboot重定位时将该段整体加载到DDR中。加载到DDR中的uboot镜像中带有特定段属性的这一段其实就是命令结构体的集合，有点像一个命令结构体数组。
(4)段起始地址和结束地址（链接地址、定义在u-boot.lds中）决定了这些命令集的开始和结束地址。


2.8.4.uboot如何处理命令集2
2.8.4.1、uboot命令定义具体实现分析
(1)U_BOOT_CMD宏基本分析
这个宏定义在uboot/common/command.h中。
U_BOOT_CMD(
	version,	1,		1,	do_version,
	"version - print monitor version\n",
	NULL
);
这个宏替换后变成：
cmd_tbl_t __u_boot_cmd_version __attribute__ ((unused,section (".u_boot_cmd"))) = {#name, maxargs, rep, cmd, usage, help}

总结：这个U_BOOT_CMD宏的理解，关键在于结构体变量的名字和段属性。名字使用##作为连字符，附加了用户自定义段属性，以保证链接时将这些数据结构链接在一起排布。
(2)链接脚本。

2.8.4.2、find_cmd函数详解
(1)find_cmd函数的任务是从当前uboot的命令集中查找是否有某个命令。如果找到则返回这个命令结构体的指针，如果未找到返回NULL。
(2)函数的实现思路很简单，如果不考虑命令带点的情况（md.b md.w这种）就更简单了。查找命令的思路其实就是for循环遍历数组的思路，不同的是数组的起始地址和结束地址是用地址值来给定的，数组中的元素个数是结构体变量类型。

2.8.4.3、U_BOOT_CMD宏详解
(1)这个宏其实就是定义了一个命令对应的结构体变量，这个变量名和宏的第一个参数有关，因此只要宏调用时传参的第一个参数不同则定义的结构体变量不会重名。

2.8.4.4、命令举例：version命令


2.8.5.uboot中增加自定义命令
2.8.5.1、在已有的c文件中直接添加命令
(1)在uboot/common/command.c中添加一个命令，叫：mycmd
(2)在已有的.c文件中添加命令比较简单，直接使用U_BOOT_CMD宏即可添加命令，给命令提供一个do_xxx的对应的函数这个命令就齐活了。
(3)添加完成后要重新编译工程（make distclean; make x210_sd_config; make），然后烧录新的uboot去运行即可体验新命令。
(4)还可以在函数中使用argc和argv来验证传参。

2.8.5.2、自建一个c文件并添加命令
(1)在uboot/common目录下新建一个命令文件，叫cmd_aston.c（对应的命令名就叫aston，对应的函数就叫do_aston函数），然后在c文件中添加命令对应的U_BOOT_CMD宏和函数。注意头文件包含不要漏掉。
(2)在uboot/common/Makefile中添加上aston.o，目的是让Make在编译时能否把cmd_aston.c编译链接进去。
(3)重新编译烧录。重新编译步骤是：make distclean; make x210_sd_config; make

2.8.5.3、体会：uboot命令体系的优点
(1)uboot的命令体系本身稍微复杂，但是他写好之后就不用动了。我们后面在移植uboot时也不会去动uboot的命令体系。我们最多就是向uboot中去添加命令，就像本节课所做的这样。
(2)向uboot中添加命令非常简单。
























