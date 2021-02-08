2.2.2.1、编辑器、编译器、运行方法    //脚本的3种执行方法
	(1)运行方法:
		① ./xx.sh			// chomod +x xx.sh
		② source xx.sh    	// 类似make xxx
		③ bash xx.sh      bash是一个脚本程序解释器, 本质上是一个可执行程序(执行bash程序, 把xx.sh作为bash运行时的参数argv[1])

	//shell下用sed命令解决\r\n -- sed 命令具有替换功能

	(2)shell程序的第一行一般都是： #!/bin/sh   // 指定shell程序执行时被哪个解释器解释执行
		可以将第一行写为：#!/bin/bash来指定使用bash执行该脚本
		注意：在ubuntu上面默认使用的解释器sh其实不是bash, 而是dash

2.2.3.2、shell中的变量定义和引用  */变量前 {} not ()
	(4)变量引用, shell中引用一个变量使用$符号
	// shell弱类型语言, 定义变量时不需要指定数据类型	 //一般数据类型为: 数字 + 字符串
	// 在引用变量的时候, 最好加上{}

2.2.4.1、shell中调用linux命令
	(2)反引号括起来执行 // var=`pwd`  var=$(pwd)	-- Makefile: var=${shell pwd}

2.2.4.2、shell中的选择分支结构
	(2)典型if语言格式
		if [ 表达式 ]; then
			xxx
		else
			xxx
		fi          // shell 下没有大括号

	(3)if的典型应用
		判断文件是否存在    (-f)
		判断目录是否存在    (-d)
		判断字符串是否相等  ("str1" = "str2") */注意用一个等号而不是两个 -- warning: 空格(两处)
		判断数字是否相等    (-eq) (-gt) (-lt) (-ge) (-le)
		判断字符串是否为空  (-z)     //注意-z判断时如果变量没定义也是不成立(noDefineValue != null)

	(4)if判断式中使用“-o”表示逻辑或

	(5)逻辑与&& 逻辑或||	// 简写的if表达式相结合

2.2.5.1、for循环

2.2.5.2、while循环
	(2)格式注意: while后面的[]两边都有空格, []后面有分号(如果do放在一行的话), i++的写法中有两层括号

2.2.5.3、echo的创建和追加输入文件

2.2.6.2、调用shell程序的传参
	(1)C语言中可以通过main函数的argc和argv给程序传参
	(2)shell程序在调用时可以传参		
		shell: ./shell1.sh aa bb cc		// $# = 3
			$# = 3
			$0 = ./shell1.sh
			$1 = aa
			$2 = bb
			$3 = cc

		C语言: ./a.out aa bb cc
			*/argc = 4
			argv[0] = ./a.out
			argv[1] = aa
			argv[2] = bb
			argv[3] = cc
			
	// shell中的$# $1等内置变量可以被shift指令修改 -- $=减1，$2->$1

2.2.7.2、目标、依赖、命令  // target, prerequisite, command
	make xxx: 使用 命令 将 依赖 加工成 目标 的过程

2.2.7.3 通配符%和Makefile自动推导(规则)
	(1)%是Makefile中的通配符, 代表一个或多个字母, 也就是说%.o就代表所有以.o为结尾的文件
	(2)/*自动推导*/其实就是Makefile的规则
		当Makefile需要某一个目标时, 就会把这个目标去套规则说明
		一旦套上了某个规则说明, 则Makefile会试图寻找这个规则中的依赖
		//如果能找到则会执行这个规则用依赖生成目标

2.2.7.4、Makefile中定义和使用变量
	(1)Makefile中定义和使用变量, 和shell脚本中非常相似(都没有变量类型, 直接定义使用, 引用变量时用$var)

2.2.7.5、伪目标（.PHONY）
	(2)伪目标一般都没有依赖, 因为执行伪目标就是为了执行目标下面的命令
	(3).PHONY来声明伪目标的作用?

	// symbol: @ #

2.2.8.3、Makefile中几种变量赋值运算符
	(1) =	*/ $(var) var的值取决于最后一次赋值, 引用前or引用后
	(2):=	*/ $(var) var的值取决于引用前, 直接解析
	(3)?=	如果变量前面并没有赋值过则执行这条赋值，如果前面已经赋值过了则本行被忽略
			(所谓的没有赋值过其实就是这个变量没有被定义过)
	(4)+=  	用来给一个已经赋值的变量接续赋值，意思就是把这次的值加到原来的值的后面，有点类似于strcat
			(在shell makefile等文件中, 可以认为/*所有变量都是字符串*/, +=就相当于给字符串stcat接续内容)
			(+=续接的内容和原来的内容之间会自动加一个空格隔开)

2.2.8.4、Makefile的环境变量		// Makefile与shell的环境变量


2.2.9.1、Makefile中使用通配符
	(1)*		若干个任意字符
	(2)?		1 个任意字符
	(3)[]		将[]中的字符依次去和外面的结合匹配

	还有个%，也是通配符，表示任意多个字符，和*很相似，但是%一般只用于规则描述中，又叫做规则通配符
	关于通配符，Makefile还有一些wildcard等比较复杂的通配符用法

2.2.9.2、Makefile的自动变量
	(1)为什么使用自动变量。
		在有些情况下文件集合中文件非常多，描述的时候很麻烦
		所以我们Makefile就用一些特殊的符号来替代符合某种条件的文件集，这就形成了自动变量
	(2)自动变量的含义：预定义的特殊意义的符号。就类似于C语言编译器中预制的那些宏__FILE__一样
	(3)常见自动变量：
		$@		规则的目标文件名
		$<		规则的依赖文件名
		$^		依赖的文件集合

# 指定 error prerequisite   can not find xxx.o

gcc xxx.c 		//是不是shell命令, 不是shell命令, 这条命令是如何执行的
				//shell实现Linux控制台? Linux控制台除了执行shell命令, 还可以执行其他可执行程序

arm-linux-gcc 	//是不是shell命令

// test shell script return 1

x210_sd_config :	unconfig
	@$(MKCONFIG) $(@:_config=) arm s5pc11x x210 samsung s5pc110			 // $@
	@echo "TEXT_BASE = 0xc3e00000" > $(obj)board/samsung/x210/config.mk  // shell中 > 表示创建文件

# mkconfig x210_sd arm s5pc11x x210 samsung s5pc110




