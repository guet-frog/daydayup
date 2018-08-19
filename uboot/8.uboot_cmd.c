
2.8.3.uboot如何处理命令集1
	本节讲解 uboot的命令数据结构 和使用 链接段属性 的方式将命令集中存放并查找的技巧，
	这种技巧在linux内核中同样有广泛使用。
2.8.4.uboot如何处理命令集2	
	本节通过分析find_cmd函数和version命令的实现函数do_version来进一步分析uboot命令集

2.8.1.1、使用uboot命令
	(1)uboot启动后进入命令行环境下, 不断接收字符, 收到enter, 开始解析字符串, 然后执行 
		-- 包括收到backspace, 字符串的处理及回显处理
		
2.8.1.3、每个命令对应一个函数		// do_xxx();

2.8.1.4、命令参数以argc&argv传给函数
(1)命令背后对应的函数接收的参数列表中有argc和argv
	然后命令体系会把执行命令时的 命令+参数
	（md 30000000 10）以argc（3）和argv（argv[0]=md, argv[1]=30000000 argv[2]=10）的方式传递给执行命令的函数

2.8.2.2、run_command函数详解
2.8.2.3、关键点分析		// run_command()
(1)控制台命令获取
(2)命令解析。parse_line函数把"md 30000000 10"解析成argv[0]=md, argv[1]=30000000 argv[2]=10;
(3)命令集中查找命令。find_cmd(argv[0])函数去uboot的命令集合当中搜索有没有argv[0]这个命令，
(4)执行命令。最后用函数指针的方式调用执行了对应函数。

	// 这取决于uboot的命令体系机制（uboot是如何完成命令的这一套设计的，命令如何去注册、存储、管理、索引。）。

2.8.3.uboot如何处理命令集1
2.8.3.1、可能的管理方式
(1)数组。结构体数组，数组中每一个结构体成员就是一个命令的所有信息。
(2)链表。链表的每个节点data段就是一个命令结构体，所有的命令都放在一条链表上。这样就解决了数组方式的不灵活。坏处是需要额外的内存开销，然后各种算法（遍历、插入、删除等）需要一定复杂度的代码执行。
(3)有第三种吗？uboot没有使用数组或者链表，而是使用了一种新的方式来实现这个功能。

2.8.3.2、命令结构体cmd_tbl_t	
	struct cmd_tbl_s	// 封装了命令所有的相关信息
	{
		char	*name;
		int		maxargs;	// 包括自己 -- error: version  111
		int		repeatable;
		int		(*cmd)(struct cmd_tbl_s *, int, int, char *[]); /* Implementation function	*/
		char		*usage;
	#ifdef	CFG_LONGHELP
		char		*help;
	#endif
	#ifdef CONFIG_AUTO_COMPLETE
		int		(*complete)(int argc, char *argv[], char last_char, int maxv, char *cmdv[]);
	#endif
	};		// warnging: 两个地方的';' 
typedef struct cmd_tbl_s	cmd_tbl_t;

总结：uboot的命令体系在工作时，一个命令对应一个cmd_tbl_t结构体的一个实例，
然后uboot支持多少个命令，就需要多少个结构体实例。
uboot的命令体系把这些结构体实例管理起来，
当用户输入了一个命令时，uboot会去这些结构体实例中查找（查找方法和存储管理的方法有关）。如果找到则执行命令，如果未找到则提示命令未知。

2.8.3.3、uboot实现命令管理的思路
(1)填充1个结构体实例构成一个命令
(2)给命令结构体实例附加特定段属性（用户自定义段），链接时将带有该段属性的内容链接在一起排列（挨着的，不会夹杂其他东西，也不会丢掉一个带有这种段属性的，但是顺序是乱序的）。
(3)uboot重定位时将该段整体加载到DDR中。加载到DDR中的uboot镜像中带有特定段属性的这一段其实就是命令结构体的集合，有点像一个命令结构体数组。
(4)段起始地址和结束地址（链接地址、定义在u-boot.lds中）决定了这些命令集的开始和结束地址。

2.8.4.uboot如何处理命令集2
2.8.4.1、uboot命令定义具体实现分析
(1)U_BOOT_CMD宏基本分析		// search： 宏一般在头文件中
这个宏定义在uboot/common/command.h中。
U_BOOT_CMD(
	version,	1,		1,	do_version,		// version or "version"	 -- #name
	"version - print monitor version\n",
	NULL
);

// gcc test.c -E -o test	-- 只预处理
这个宏替换后变成：
cmd_tbl_t __u_boot_cmd_version __attribute__ ((unused,section (".u_boot_cmd"))) = {#name, maxargs, rep, cmd, usage, help}

总结：这个U_BOOT_CMD宏的理解，关键在于结构体变量的名字和段属性。
	名字使用##作为连字符(gcc扩展语法)，	// 宏: 来定义全局变量, 并且解决名称冲突问题
	附加了用户自定义段属性，以保证链接时将这些数据结构链接在一起排布
(2)链接脚本。

2.8.4.2、find_cmd函数详解
(1)find_cmd函数的任务是从当前uboot的命令集中查找是否有某个命令。如果找到则返回这个命令结构体的指针，如果未找到返回NULL。
(2)函数的实现思路很简单，如果不考虑命令带点的情况（md.b md.w这种）就更简单了。
	查找命令的思路其实就是for循环遍历数组的思路，不同的是数组的起始地址和结束地址是用地址值来给定的，数组中的元素个数是结构体变量类型。

	// cmd_tbl_t *cmdtp_tmp = &__u_uboot_cmd_start;
	// len = ((p = strchr(cmd, '.')) == NULL) ? strlen(cmd) : (p - cmd);	// -- md.b md.w md.l
	// cmdtp_tmp++ 	// 指针的++, 要考虑指针的数据类型
	
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
(2)在uboot/common/Makefile中添加上aston.o，目的是让Make在编译时能否把cmd_aston.c编译链接进去。	// uboot/common/Makefile
(3)重新编译烧录。重新编译步骤是：make distclean; make x210_sd_config; make

2.8.5.3、体会：uboot命令体系的优点
(1)uboot的命令体系本身稍微复杂，但是他写好之后就不用动了。我们后面在移植uboot时也不会去动uboot的命令体系。我们最多就是向uboot中去添加命令，就像本节课所做的这样。
(2)向uboot中添加命令非常简单。


// 编译配置 -- kernel中常用技巧

COBJS-y += main.o
COBJS-$(CONFIG_CMD_BDI) += cmd_bdinfo.o








