	
2.2.1.shell介绍
	shell是一个用C语言编写的程序;

	shell是用户使用OS的桥梁: Unix shell, Windows explorer图形界面shell;

	shell既是一种命令语言--交互式地解释和执行用户输入的命令; 又是一种程序设计语言--提供变量参数控制结构
	shell有两种执行命令的方式:
		(1)交互式(Interactive): 解释执行用户的命令, 用户输入一条命令, shell就解释执行一条 // command language
		(2)批处理(Batch)	  : 用户事先写好一个shell script, shell一次性执行完			  // program language

	我们常说有多少种shell, 其实就是说的是shell脚本解释器	// 常见shell脚本解释器bash, sh, csh, ksh

	编译型语言与解释型语言 // 解释器(interpreter)读取源代码(source code)并转换为目标代码(object code) 每次执行程序都多了编译的过程，因此效率有所下降  

2.2.2.动手写第一个shell
    2.2.2.1、编辑器、编译器、运行方法    //脚本的3种执行方法
        (1)编辑器: shell是要在linux系统下运行, 所以换行符必须是'\n'
        (2)编译器: shell是解释性语言, 直接编辑完就可以运行
        (3)运行方法:
                第一种：./xx.sh         类似于运行/*二进制可执行程序*/此时shell程序需要有可执行权限(chmod u+x xx.sh来添加可执行权限) // chomod +x xx.sh
                第二种：source xx.sh    source是linux的一个命令, 这个命令就是用来执行脚本程序, 这样运行不需要脚本具有可执行权限 // 类似于makefile的执行
                第三种：bash xx.sh      bash是一个脚本程序解释器, 本质上是一个可执行程序(执行bash程序, 把xx.sh作为bash运行时的参数argv[1])

    //  warning: ./test.sh 而不是 test.sh                                                      \
                运行其它二进制的程序也一样, 直接写test.sh, linux系统会去PATH里查询test.sh      \
                默认只有/bin, /sbin, /usr/bin，/usr/sbin等在PATH路径                           \
                current directory通常不在PATH路径, 所以写成test.sh是找不到命令                 
                ./test.sh       */ OS就在当前目录找该二进制程序
				
	//shell下用sed命令解决\r\n -- sed 命令具有替换功能
    2.2.2.2、helloworld程序和解释
        (1)shell程序的第一行一般都是： #!/bin/sh   // 指定shell程序执行时被哪个解释器解释执行
            可以将第一行写为：#!/bin/bash来指定使用bash执行该脚本
            注意：在ubuntu上面默认使用的解释器sh其实不是bash, 而是dash

2.2.3.shell编程学习1
    2.2.3.2、shell中的变量定义和引用  */变量前 {} not ()
        (4)变量引用, shell中引用一个变量使用$符号
        //shell弱类型语言, 定义变量时不需要指定数据类型	 //一般数据类型为: 数字 + 字符串

    2.2.3.3、shell中无引用、单引号和双引号的区别
        (1)shell中使用字符串可以不加双引号直接使用, 而且有空格时也可以,
			但是缺陷是不能输出"或者其他转义字符
        (3)单引号中：完全字面替换, 不能输出转义字符(不可包含单引号本身)
        (4)双引号中：$加变量名可以取变量的值
                     反引号仍表示命令替换
                     \$  \` \"  \\
        除以上情况之外, 在其它字符前面的\无特殊含义，只表示字面值。

2.2.4.shell编程学习2
    2.2.4.1、shell中调用linux命令
        (1)直接执行
        (2)反引号括起来执行 // var=`pwd`

    2.2.4.2、shell中的选择分支结构
        (2)典型if语言格式
            if [ 表达式 ]; then
                xxx
            else
                xxx
            fi          // shell 下没有大括号

        (3)if的典型应用
            判断文件是否存在    (-f) */注意[]里面前后都有空格, 不能省略。
            判断目录是否存在    (-d)
            判断字符串是否相等  ("str1" = "str2") */注意用一个等号而不是两个
            判断数字是否相等    (-eq) (-gt) (-lt) (-ge) (-le)	//gt:greater, than lt:less than, ge:greater or equal, le:less or equal
            判断字符串是否为空  (-z)     //注意-z判断时如果变量没定义也是不成立(noDefineValue != null)

        (4)if判断式中使用“-o”表示逻辑或

        (5)逻辑与&& 逻辑或|| 与 /*简写的*/if表达式相结合

2.2.5.shell中的循环结构
    2.2.5.1、for循环

    2.2.5.2、while循环
        (2)格式注意: while后面的[]两边都有空格, []后面有分号(如果do放在一行的话), i++的写法中有两层括号

    2.2.5.3、echo的创建和追加输入文件
        (1)在shell中可以直接使用echo指令新建一个文件, 并且将一些内容传入这个文件中 // echo `ll` > a.txt
        (2)还可以使用echo指令配合追加符号    // echo hellow world >> a.txt

2.2.6.shell中其他值得关注的知识点
    2.2.6.1、case语句
        (2)no need break

    2.2.6.2、调用shell程序的传参
        (1)C语言中可以通过main函数的argc和argv给程序传参
        (2)shell程序在调用时可以传参
            $#: 传参的个数(only param); $0、$1: param[0] param[1]

            C语言: ./a.out aa bb cc 	argc = 4, argv[0] = ./a.out, argv[1]是第一个有效参数···
            shell: source a.sh aa bb cc  $# = 3, $0是执行这个shell程序的解析程序的名字, 
												 $1是第一个有效参数的值，$2是第2个有效参数的值···

    2.2.6.3、while循环和case语言和传参结合
        (1)shell中break只用于循环跳出 //当while中内嵌case语句时, case中的break是跳出外层的while循环的, 不是用来跳出case语句的
        (2)shell中的$# $1等内置变量的值是可以被shift指令改变
            shift指令有点像左移运算符, 把shell程序的传参左移了一个移出去, 原来的$2变成了新的$1, 原来的$#减1

2.2.7.Makefile基础回顾 
    2.2.7.1、Makefile的作用和意义
        (1)project中大量文件需要管理, Makefile来做project管理, 方便编译链接过程
        (2)uboot和linux kernel本质上都是C语言的项目, 大量project file需要Makefile来管理

    2.2.7.2、目标、依赖、命令  // target、prerequisite、command
        //make xxx: 使用命令将依赖加工成目标的过程

    2.2.7.3 通配符%和Makefile自动推导(规则)
        (1)%是Makefile中的通配符, 代表一个或多个字母, 也就是说%.o就代表所有以.o为结尾的文件
        (2)/*自动推导*/其实就是Makefile的规则
            当Makefile需要某一个目标时, 他会把这个目标去套规则说明
            一旦套上了某个规则说明, 则Makefile会试图寻找这个规则中的依赖
            //如果能找到则会执行这个规则用依赖生成目标

    2.2.7.4、Makefile中定义和使用变量
        (1)Makefile中定义和使用变量, 和shell脚本中非常相似(都没有变量类型, 直接定义使用, 引用变量时用$var)

    2.2.7.5、伪目标（.PHONY）
        (2)伪目标一般都没有依赖, 因为执行伪目标就是为了执行目标下面的命令
        (3).PHONY来声明伪目标的作用?

        // makefile源文件没有可执行权限, 也可以使用make命令 -- 类似于source xxx.sh
		// symbol: @ #

2.2.8.Mafile补充学习1
	2.2.8.3、Makefile中几种变量赋值运算符
		(1)=	*/ $(var) var的值取决于最后一次赋值, 引用前or引用后
		(2):=	*/ $(var) var的值取决于引用前, 直接解析
		(3)?=	如果变量前面并没有赋值过则执行这条赋值，如果前面已经赋值过了则本行被忽略
				(所谓的没有赋值过其实就是这个变量没有被定义过)
		(4)+=  	用来给一个已经赋值的变量接续赋值，意思就是把这次的值加到原来的值的后面，有点类似于strcat
				(在shell makefile等文件中, 可以认为/*所有变量都是字符串*/, +=就相当于给字符串stcat接续内容)
				(+=续接的内容和原来的内容之间会自动加一个空格隔开)

		注意：Makefile中并不要求赋值运算符两边一定要有空格或者无空格，这一点比shell的格式要求要松一些

	2.2.8.4、Makefile的环境变量
		(1)makefile中用export导出的就是环境变量
			一般情况下要求环境变量名用大写, 普通变量名用小写
		(2)环境变量和普通变量不同
			可以这样理解: 环境变量类似于整个工程中所有Makefile之间可以共享的全局变量，
			而普通变量只是当前本Makefile中使用的局部变量。
			warning: 定义了一个环境变量会影响到工程中别的Makefile文件
		(3)Makefile中有一些环境变量
			可能是makefile本身自己定义的内部的环境变量或者是当前的执行环境提供的环境变量
			（譬如我们在make执行时给makefile传参。
			make CC=arm-linux-gcc，其实就是给当前Makefile传了一个环境变量CC，值是arm-linux-gcc。
			我们在make时给makefile传的环境变量值优先级最高的，可以覆盖makefile中的赋值）
			这就好像C语言中编译器预定义的宏__LINE__ __FUNCTION__等一样。

2.2.9.Makefile补充学习2
	2.2.9.1、Makefile中使用通配符
		(1)*		若干个任意字符
		(2)?		1个任意字符
		(3)[]		将[]中的字符依次去和外面的结合匹配

		还有个%，也是通配符，表示任意多个字符，和*很相似，但是%一般只用于规则描述中，又叫做规则通配符。
		关于通配符，Makefile还有一些wildcard等比较复杂的通配符用法

	2.2.9.2、Makefile的自动变量
		(1)为什么使用自动变量。
			在有些情况下文件集合中文件非常多，描述的时候很麻烦，
			所以我们Makefile就用一些特殊的符号来替代符合某种条件的文件集，这就形成了自动变量
		(2)自动变量的含义：预定义的特殊意义的符号。就类似于C语言编译器中预制的那些宏__FILE__一样。
		(3)常见自动变量：
			$@		规则的目标文件名
			$<		规则的依赖文件名
			$^		依赖的文件集合
			
	# 指定 error prerequisite   can not find xxx.o
	
	gcc xxx.c 		//是不是shell命令, 不是shell命令, 这条命令是如何执行的
	
	arm-linux-gcc 	//是不是shell命令
























