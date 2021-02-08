
/** 变量类型、作用域 */
#if
	字符串
	数字		// 变量是数字类型，可以通过(())进行运算 -- shell无浮点数计算
	布尔
	a=1
	a="hello world"

	echo $a
	echo ${a}

	echo $ab
	echo ${a}b		// echo ${a}{b}

	echo $		// 通过tab自动补全 查看系统变量
	echo $PWD
	echo $USER
	echo $HOME
	echo $PATH

	array=(1 2 3 4 5)			// a=3 -> (3 2 3 4 5)	-- a=(1, 2, 3, 4, 5)
	echo ${array[*]}
	echo ${#array[*]}

	""特殊作用
	echo "hello ${array[*]} world"
	echo 'hello ${array[*]} world'

	echo "hello world\n"
	echo -e "hello world\n"
	echo -e "\n"

	((a=a+1))
	
	exit表示进程退出，shell自身为交互进程
	shell中执行命令，本质上创建进程 -- 用 $? 获取进程返回码
	$! 最近的一个后台运行的进程号 -- 方便kill用
	
	在bash中运行sh 或者 bash
	创建新的bash，不能继承父bash中的变量 -- 不能传递给子shell(进程)
	export a=hello		// PATH变量已经被export，子shell中可以使用

	bash file.sh	// 在新的shell执行 所有的变量和函数都会被释放掉
	./file.sh		// 脚本通过一个 子shell 执行，执行完成后 内部变量不能继续使用 -- 通过存文件传递

	source file.sh  // 在当前shell下 逐行执行，所有的变量 和 函数都可以被访问
	. file.sh		// source简写
	
	vi ~/.bash_profile	
	
	PATH="$HOME/bin:$PATH"
#endif

/** 字符串 */
	echo ${s:0:5}		// awk切片
	echo ${#s}			// 计算长度 -- ${#array[*]}

	echo ${s/testerhome/hogwarts}	替换字符串
	echo ${s/l/_}
	echo ${s//l/_}		// 贪婪匹配
	echo ${s/l*/_}
	echo ${s/*l/_}
	
	echo ${s#*l}		// "lo world"
	echo ${s##*l}		// "d"
	echo ${s%l*}
	echo ${s%%l*}

/** */
/** 判断 */
#if
	算术判断 -eq -ne -gt -ge -lt -le
	[ 2 -eq 2 ]		// 注意空格
	echo $?
	[ 3 -eq 2 ]
	echo $?

	((10>=8))		// (())除了进行算术运算，也可以算术比较

	字符串比较
	[ "$a" = "hello world" ]		// 精准匹配

	[ -z "$b" ]		// 判断为空
	[ -n "$b" ]		// nozero			// [ -n $b ]
	echo ${#a}	// 长度为0 判断为空

	-e file 	// 文件存在 exist
	-d file 	// 是目录
	-f file		// 是普通文件

	x=`ls`
	x=$(ls)		// 可以嵌套

	((a=a+2))	// ok
	$((a=a+2))	// error
	echo $((a=a+2))	// ok
	
	a=1; [ a -gt 0 ]; echo $?		// error
	a=1; [ $a -gt 0 ]; echo $?		// ok
	a=1; [ ((a=a+1)) -gt 0 ]; echo $? // error
	a=1; [ $((a=a+1)) -gt 0 ]; echo $? // ok
#endif

/** 逻辑控制 if、for、while */
#if
	if [ condition ]; then ...; elif [ condition ]; then ...; else ...; fi

	[ -f a.c ] && echo is file 		// if结构简写 -- 有不同点
	[ -f aa.c ] && echo is file || echo not file

	// 按次数循环
	for ((i=0;i<10;i++)); do echo $i; done

	array=(1 2 3 4 5)
	echo $array			// 1
	echo $array[*]		// 1[*]
	echo ${array[*]}	// 1 2 3 4 5		// x=1234 ${x}_aa

	// 迭代循环
	s="aa bb cc dd"
	echo $s
	s="aa bb cc dd"; for x in $s; do echo $x; done	// for x in "$s"效果不一样

	while [ $i -lt 5 ]; do echo $i; ((i=i+1)); done

	while read line; do echo $line; done < /tmp/tmp		// while ... done; 整体是一个命令 再  < /a.c
	i=0; while read line; do ((i=i+1)); if [ $((i%2)) -gt 0 ]; then echo $line >> tmp.txt; fi; done < ./a.c

	// if while 后面可以为 命令(read line)	-- 命令都有返回值
#endif

/** shell常用命令 */
#if
	cygwin

	git bash底层使用cygwin

	set获取本地所有函数定义、变量定义
	set | less

	read用来读取输入，读到 回车 停止

	echo xxx	// 自动追加换行
	printf "xxx"	// 不换行

	grep "s*"		// 查找，从输入中查找，如果 条件匹配输入 标记颜色 -- 需要 读 输入
	echo asdfsdf | grep "s*"	// grep不用从标准输入中读取		// grep s* -- error

	echo *		// 列出所有文件
	echo ?
	echo ???

	$0表示执行的程序，少用
	$1 $2 $3						// 当前脚本、函数的第几个参数
	$@，$*表示所有参数，不包括$0
	${#*} ${#@}表示参数个数，不包括$0

	curl http://www.baidu.com > index.html		// shell中用来发送http请求
	python -m CGIHTTPServer		// 默认开启一个8000端口，库，搭建一个小网站

	ls ddd > tmp 2>&1
	ls ddd 2>/dev/null

	set -x
	./file.sh	// 执行每一条命令，打印输出结果

	a = " "
	[ -f "$a" ]
	
	shell自动化
	交互程序实现非交互执行

	自动输入法  -- 很多命令支持从管道中读取

	ssh-copy-id  -- 免除远程服务器需要输密码，免密登录

	ps、top

	netstat 查看本地网络连接
	netstat -tlnp 查看本地开放的端口		:22 远程服务器连接 :25 邮件

	vmstat
#endif

/** grep、awk、sed */
#if
	grep 数据查找定位
	awk	 数据切片
	sed  数据修改

	curl http://testerhome.com	// https

	grep 支持正则表达式		-- 将输入完全读取，当成一个正则
	grep -E "Appium|Python" tmp.txt

	grep java tmp.txt | grep -v script

	curl http://www.baidu.com | grep -o 'http://[a-zA-z0-9\\-\\.]*'		// 匹配网址 -o只取出指定内容

	echo "123|456|789" | -F"|" '{print $1}'		// "{print $1}"
	-F 字段分隔符
	输出的任意一行，在awk看来是一条记录record，一条记录分为多个字段field

	在BEGIN指定相关数据，在END中打印相关信息

	awk 'BEGIN{print 1/3}'
#endif


单引号和双引号内的都是字符串，双引号内字符串可以被替换成变量，单引号就是原字符串输出
在执行过程中，shell会给程序添加单引号，结果单引号就很多了，其中有转义
如果要执行这个命令，可以把最后的${cp_cmd}修改为eval ${cp_cmd}，eval会把中间的单引号和双引号全部去掉





