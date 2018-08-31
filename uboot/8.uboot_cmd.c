
2.8.1.1、使用uboot命令
	(1)uboot启动后进入命令行环境下, 不断接收字符, 收到enter, 开始解析字符串, 然后执行 
		-- 包括收到backspace, 字符串的处理及回显处理

2.8.1.3、每个命令对应一个函数		// do_xxx() -- 命令执行函数

2.8.1.4、命令参数以argc、argv传给命令函数
(1)命令执行函数接收的参数列表中有argc和argv, 然后命令体系会把执行命令时的命令+参数
	// md 30000000 10	 -- argc = 3, argv[0]=md, argv[1]=30000000, argv[2]=10传递给命令执行函数

2.8.2.3、run_commond()
	(1)命令获取
	(2)命令解析		// parse_line()
	(3)查找命令		// find_cmd(argv[0])
	(4)执行命令		// 命令结构体中函数指针

	// uboot的命令体系机制(注册、存储、管理、索引)

2.8.3.1、可能的管理方式
	(1)数组	-- 结构体数组 // 结构体成员变量封装命令的所有信息
	(2)链表 -- 每个node对应一条命令

	// uboot使用自定义段的方式来存储管理命令

2.8.3.2、命令结构体cmd_tbl_t
	struct cmd_tbl_s	// 封装了命令所有的相关信息
	{
		char	*name;
		int		maxargs;	// 包括自己 -- error: version  111
		int		repeatable;
		int		(*cmd)(struct cmd_tbl_s *, int, int, char *[]); /* Implementation function */
		char	*usage;
		char	*help
	};		// warnging: 两个地方的';' 

	// 一条命令对应一个cmd_tbl_t实例

2.8.3.3、uboot实现命令管理
	(1) 填充一个cmd_tbl_t实例构成一条命令	// cmd_tbl_t实例附加自定义段属性(用户自定义段) -- for link

	(2) uboot重定位时将该段整体加载到ddr中	// 类似于结构体数组    -- warning: 区别于 default_environment[]

	(3) 段起始地址和结束地址		// 用于遍历段

2.8.4.1、U_BOOT_CMD宏

	#define Struct_Section  __attribute__ ((unused,section (".u_boot_cmd")))

	#define U_BOOT_CMD(name, maxargs, rep, cmd, usage, help) \
		cmd_tbl_t __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}	 // gcc扩展语法: ##name #name

	U_BOOT_CMD(
		version,	1,		1,	do_version,		// version not "version"
		"version - print monitor version\n",
		NULL
	);

	// uboot/common/command.h -- search： 宏一般在头文件中
	// gcc test.c -E -o test	-- 只预处理
	// ##name解决定义变量重名问题

2.8.4.2、find_cmd函数
	(1)find_cmd()从当前uboot的命令集中查找是否有某个命令, 如果找到则返回这个命令结构体的指针, 如果未找到返回NULL

	// cmd_tbl_t *cmdtp_tmp = &__u_uboot_cmd_start;
	// len = ((p = strchr(cmd, '.')) == NULL) ? strlen(cmd) : (p - cmd);	// -- md.b md.w md.l
	// cmdtp_tmp++ 	// 指针的++, 要考虑指针的数据类型

2.8.5.1、在已有的c文件中直接添加命令
	(1)在uboot/common/command.c中添加命令, 使用U_BOOT_CMD定义cmd_tbl_t实例, 提供do_xxx()对应命令执行函数

2.8.5.2、自建一个c文件并添加命令
	(1)在uboot/common目录下新建cmd_xxx.c
		// 添加相应头文件
		// uboot/common/Makefile添加cmd_xxx.o


// 编译配置 -- kernel中常用
	COBJS-y += main.o
	COBJS-$(CONFIG_CMD_BDI) += cmd_bdinfo.o

// uboot使用/*链接段属性*/的方式存放命令集	-- kernel中常用






