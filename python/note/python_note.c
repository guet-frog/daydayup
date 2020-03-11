
#if linux基础
	   参数   选项
    ls /bin   -h		// 不一样的命令选项不同 "-"
	
    通配符'?'			// 正则表达式
    ls 1?3.txt
    ls 1[12345]3.txt
    ls 1[1-5]3.txt
	
    重定向
    ls > xxx.txt
    ls >> xxx.txt
    cat 1.txt 2.txt >> xxx.txt		// 文件合并
	
    ls /bin -l | more		// f(前) b
	
    ls;ls -alh
	
    ctrl + c 	取消输入的命令
	
    tree
	
    ls -s xxx.txt xxx_softlink.txt		// 源文件 目标软链接
    ls -l 查看文件硬链接数
	
    grep "ntfs"	xxx.txt
    grep -n "ntfs" xxx.txt
    grep -v "ntfs" xxx.txt	// 反向查找
    grep "^ntfs" xxx.txt	// 以ntfs开头查找
    grep "ntfs$" xxx.txt	// 以ntfs结尾查找
	
    find / -name "*name*"
	
    tar -cvf test.tar *.py		// 打包
    tar -xvf test.tar
	
    tar -zcvf xxx.tar.gz *.py	// 压缩
    tar -zxvf xxx.tar.gz
	
    tar -jcvf xxx.tar.bz2 *.py
    tar -jxvf xxx.tar.bz2
	
    cal、data
	
    ssh python@192.168.1.1			// windows Xshell
    who		// 查看有哪些用户登录
	
    vi /etc/apt/sources.list
    sudo apt-get update
	
	// 重定向及管道查找
	ifconfig | grep '192'
	
	ssh xxx@192.168.1.1
	
	外网访问本地局域网服务器，借助其他软件如：花生壳。通常使用外网服务器如：阿里云		// 原来的虚拟机添加一些功能
	
	who命令 查看当前用户登录信息
	
	crtl + A / ctrl + E
	
	cat /etc/group
	AAA:x:1001:		// AAA用户组组名
	
	chmod 777 test_dir -R		//
	
	网站显示信息，网站后台服务程序一般两种：/Apache /Nginx
#endif

#if python基础知识
	人工智能、爬虫、web服务器

	ipython支持linux命令，支持自动补全
	ipython3

	当语句以冒号:结尾时，缩进的语句视为代码块

	没有do while、switch、++ --
// ------------------------------------------------
// 输入输出
// ------------------------------------------------
	python3将输入作为字符串		a = 'abc'				a = 1+4 	# a = '1+4'
	python2将输入作为代码执行	a = abc #未定义abc		a = 1+4 	# a = 5

	python2中使用raw_input()

	print("%d %d %d"%(name,age,addr))	//不需要',' 多一个%
	print(100 + 200 + 300)
	print('100 + 200 =', 100 + 200)
	print(name)
    
    help(print)
    print(value, ..., sep='', end='\n', file= sys.stdout, flush=False)	// 缺省参数放后面
    print("", end = "")		// end是print的默认参数
    
// ------------------------------------------------
// 运算符
// ------------------------------------------------
	"="*50	// 表示 乘
			// 在形参中 *表示特殊值		-- *args表示元组 **args表示字典
			// 在实参中 *表示拆包
			// 复制
			
    [1, 2] + [3, 4] = [1, 2, 3, 4]		// 合并(字符串、列表、元组 -- not字典)
	
    and or not
	
	执行加法，python会自动检测类型 -- 组成字符串的两种方式
    
// ------------------------------------------------
// 条件判断、循环
// ------------------------------------------------
	if x:
		xxx			// 只要x是非零数值、非空字符串、非空list等，就判断为True，否则为False
		
	list(range(5))	// [0, 1, 2, 3, 4]
	
	import random
    rand_num = random.randint(0, 2)
    
    for 临时变量 in xxx:
	
    for else 和 break
    
    for遍历比较方便
    while需要处理长度、下标
	
	break continue这两个语句都 必须配合if语句使用
#endif

#if 字符串、列表、元组、字典  
// ----------------------------------------------
//  字符串
// ----------------------------------------------
	python3字符串采用Unicode编码

    切片(字符串、列表、元组)
    切片的语法：[起始:结束:步长]    // 左闭右开
    
    name[len(name)-1]
    name[-1]
    name[-2]
    
    name[2:5]
    name[2:-3]	// 取第二个元素到倒数第三个元素
    name[2:]	// 取第二个元素到最后
    
    name[2:-1:2]	// 取第二个元素到倒数第一个元素，每2个取一个
    
    name[]
    name[::-1]		// 倒序输出
    print(name)		// 正序输出，不切片
    
    字符串的常见操作
        find、rfind
        index、rindex
        count				// 统计出现次数
        replace				// 字符串是不可变类型，replace替换后，原字符串没有改变
        split				// split(" ")	-- 按照空格切割，切割后空格去掉
        startswith          
        endswith			// 判断文件类型
		upper、lower		// 用户输入统一转化
        ljust、rstrip       // 左对齐，长度不变后面补空格; 删除右边的空格
        partition           // 仅仅分开
        splitlines          // 按照换行分割 成 字符串列表
		isdigit				// int(a) 先判断a是纯数字，再进行强制类型转化
        join				// 几个字符串连接成一个字符串
		
		// 字符串 -> bytes
		   byte1 = str1.encode("utf-8")
		   byte1 = bytes(str1, "utf-8")
		   
		// bytes -> 字符串
		   str1 = byte1.decode("utf-8")
		   str1 = str(byte1, "utf-8")
			
        
// ----------------------------------------------
//  列表
// ----------------------------------------------
    列表可以存储不同类型元素，列表一般用来存储相同类型元素
	list = []
	list = [[], [], []]		// 定义列表可以不指定大小 -- 定义tuple时，元素需要确定
	
    增:
		list.append(para)	// 添加到最后，作为一个元素添加
		list.insert(index, para)	// 添加到任意位置

		list.extend(list)	// 合并两个元组，将一个集合中的元素添加到列表中 -- 与append效果不同

    删:
		pop()		// 删除最后一个
		remove()	// 根据 内容 删除第一个
		del list[0]	// 根据 下标 删除
        
    改:
		list[0] = new value

    查:
		if "xxx" in names:
			print()
		if "xxx" not in names:
			print()
		
		a.index("xxx", 1, 4)        // 左闭右开，返回下标
		a.count("xxx")

	sort()、reverse()将列表按特定序列重新排列
	
	range(1, 3)，range(10)
	a = [i for i in range(1, 18)]		// 列表生成式
	a = [11 for i in range(1, 18)]
	
	c = [i for i in range(10) if i % 2 == 0]

// ----------------------------------------------
//	元组
// ----------------------------------------------
	元组与列表类似，元组的元素不能修改(也不能删除其中元素)		// tuple的每个元素，指向永远不变
	元组类似于const
	
	tuple.index()
	tuple.count()
	
	元祖只有一个元素时，(a,)

// ----------------------------------------------
//	字典
// ----------------------------------------------
	存储多个数据，方便定位元素，多用于需要高速查找的环境	// dict是用空间来换取时间的一种方法
	infor = {键:值, 键:值}
    
	增、改:
		info["new_key"] = "new_value"
	
	删:
		d.pop["name"]
		
		del info["name"]
		del info
		info.clear()		// 清空字典
	
	查:
		根据 键 访问 值，访问不存在的键会报错，info.get()访问不存在的键，返回为None
		age = info.get("age", 18)	// 如果没有，则返回默认值18 -- 返回None时python交互环境不显示结果
		
		"name" in a		// True
	
	字典常用操作:
		len()、keys()、values()、items()、has_key()
		
// ----------------------------------------------
// 	集合
// ----------------------------------------------
	不存在相同元素
	
	a = [11, 22, 11, 22]
	f = set(a)
	b = list(f)		// 列表去重

// ----------------------------------------------
// 遍历
// ----------------------------------------------
	字符串遍历
		for tmp in str:
	
	列表遍历
		for tmp in list:
	
	元组遍历
		for tmp in tuple:
	
	字典遍历
		遍历字典的key
			for key in dict.keys():					// keys
		
		遍历字典的value
			for value in dict.values():				// values
		
		遍历字典的项
			for item in dict.items():				// items
		
		遍历字典的key-value
			for key, value in dict.items():
#endif

#if 函数
	函数名其实就是指向函数对象的引用，把函数名赋值给一个变量，给函数起一个"别名"

	return a, b, c  // 本质利用了元组 return (a, b, c)

// ----------------------------------------------
// 	变量的作用域
// ----------------------------------------------
	def modify_wendu():
		global wendu
		wendu = 33
	
	// 列表、字典可以不用添加global直接使用 -- 可变类型变量
	
	全局变量做形参时，需要注意是否是可变类型
	
// ----------------------------------------------
// 缺省参数、不定长参数
// ----------------------------------------------
	test(a, b = 22, c = 33)		// 缺省参数只能放在后面 -- 简化函数的调用
	
	def	func(a, b, *args):		// 形参变量名: args
		sum = 0
		for n in number:		// 可变参数在函数调用时自动组装一个tuple
			sum = sum + n
		return sum
	
	def func(a, b, *args, **kwargs):
		xxx
	
	A = (44, 55, 66)
	B = {"name":"laowang", "age":18}
	func(11, 22, *A, **B)	/* - 拆包，进程线程中使用 */

	a = (11, 22)
	c,d = a    				// 拆包

// ----------------------------------------------
//	引用、引用传参
// ----------------------------------------------
	python中赋值语句都是 引用 -- 不可变类型不可以修改
	
	a = [1, 2, 3]
	b = a
	a.append(4)
	
	id(a)

	python中函数参数是引用传递(不是值传递)
	对于不可变类型，因变量不能修改，所以运算不会影响到变量自身
	而对于可变类型来说，函数体中的运算有可能会更改传入的参数变量
// ----------------------------------------------
// 可变类型和不可变类型
// ----------------------------------------------
	不可变类型:
		数值类型(int、long、bool、float)
		字符串		// a = "hello"	a[0] = 'w' -- error
		元组
	
	可变类型:
		列表
		字典
	
	// 不可变类型才能作为 字典的key -- unhashable type: 'list'	(把任意字符串转化成 绝对不相同且长度一样的值)
	// 字典的存储方式 -- 将key进行hash运算得到内存地址

// ----------------------------------------------
// 	递归
// ----------------------------------------------
	一个函数在内部调用自己本身 -- 递归函数
	5! = 5 * 4!
	4! = 4 * 3!
	3! = 3 * 2!
	
// ----------------------------------------------
//	匿名函数
// ----------------------------------------------
	def test(a, b, func)
		result = func(a, b)
		print(result)
	
	test(11, 22, lambda x,y:x+y)
	
	eval("a + b")
#endif

#if 文件
// ----------------------------------------------
// 	文件常用操作方法
// ----------------------------------------------
	'r'		// = 默认不加参数
	'w'
	'r+'
	'w+'
	
	fd.readlines()			// 读取文件所有内容，按照列表方式存储
	fd.readline()
	
	fd.read(num) 
	fd.read()				// 读所有
	
	fd.tell()				// 获取当前读写文件
	fd.seek(offset, from)	// 0:文件开头，1:当前位置，2:文件末尾	-- fd.seek(-3, 2) 离文件末尾3字节
	
	os.rename(old_name, new_name)
	os.remove(doc_name)
	
	f.write(srt(list))		// 参数是str类型 -- 带[]
	global card_info
	card_info = eval(f.read())
	
// ----------------------------------------------
//	文件夹相关操作
// ----------------------------------------------
	os.mkdir(dir_name)
	os.rmdir(dir_name)
	os.getcwd()				// 获取当前目录
	os.chdir(new_dir)		// 修改默认目录
	os.listdir()			// ls信息
	
	#os.chdir(os.getcwd() + "\\" + dir_name)
	#os.chdir(os.getcwd() + "/" + dir_name)
	#os.chdir(os.getcwd() + "\" + dir_name)      # os.chdir(os.getcwd() + "\" + dir_name)

// ----------------------------------------------
//	重命名文件名、拷贝文件
// ----------------------------------------------
	pos = old_file_name.rfind(".")
	new_file_name = old_file_name[0:pos] + "[复件]" + old_file_name[pos:]
	
	while True:
		content = f_read.read(1024)
		
		if 0 == len(content):
			break
		
		f_write.write(content)
		
	像open()函数返回的这种有个read()方法的对象，在Python中统称为file-like Object。除了file外，还可以是内存的字节流，网络流，自定义流等等。file-like Object不要求从特定类继承，只要写个read()方法就行。
	StringIO就是在内存中创建的file-like Object，常用作临时缓冲。
#endif

#if 正则表达式
	描述某种规则的表达式(应具有某种通用性) -- regular expresstion
	
	ret = re.match(正则表达式, 要匹配的字符串)	// 返回匹配对象，否则返回None(不是空字符串"" -- 空字符串可能也是某种正则表达式)
	
	obj.group()		// 返回字符串的匹配部分
	
	ret = re.match("itcast", "itcast_heima")	// "itcast"虽然是正则表达式，但不具备通用性
	
// ----------------------------------------------
//	通用表示字符
// ----------------------------------------------
	re.match("...", "ab")	// 匹配3个字符，非'\n' -- 通常用于占位符

	re.match("\d", "1aa")	// 字符串中有数字，-- 开头有数字
	re.match("\D", "a11")	// 匹配非数字
	
	re.match("\s", "\na")	// 匹配不可见字符
	re.match("\S", "a\n")
	
	re.match("\w", "aaa")	// _ a-z A-Z 0-9 -- 如校验变量命名规则
	re.match("\W", "aaa")
	
	re.match("[^1-8]", "111")	// 对方括号里面数字取反
	\d == [0-9]
	\D == [^0-9]
	\w == [a-zA-Z0-9_]

	re.match("\d*", "abc")	// 匹配，"\d*"一组，'\d'表示字符集合 '*'表示数量
	
	s = r"\nabc"			// raw
	
	// match()方法，从左向右，如果第一个不匹配就立即结束
	
	r = re.findall(r"\w+</h1>", s)
	len(r)
#endif

#if 面向对象
	
	__xxx__()称之为魔法方法，有特殊功能			// __init(self, x, y)__， __str(self)__
	
	类对象		// 类也是一个对象
	实例对象
	
	__new__() 创建对象，返回对象的引用
	__init__() 初始化						// 等效 构造方法
	
	__str__()				// 
	__call_()				// 重写call方法，对象可以直接作为函数指针调用
    
// ----------------------------------------------
//  类属性、实例属性
// ----------------------------------------------
    class A(object):
        num = 100       // 类属性
        
        def __init__(self):
            super().父类方法名()       // python3支持
            super(子类名, self).父类方法名()
            父类名.父类方法()                  // 3种方式
            
            self.xxx = 100      // 实例属性
            A.num = 200         // 类属性修改
        
        @classmethod            // 类方法
        def test(cls):
            cls.num = 200
        
        @staticmethod           // 静态方法
        def test1():
            pass
    
    // 类方法、静态方法的作用
    
    a = A()
    b = a
    del a       // 不会调用 __del__()
    del b       // 此时调用 __del__()
    
// ---------------------------------------------------------
//	异常
// ---------------------------------------------------------
	try:
		open("xxx.txt")
		print("AAAA")
	except NameError:
		print("BBBB")
	except FileNotfoundError:
		print("CCCC")
	except Exception as ret:
		return(ret)
    finally:
        一定执行        // f.close()
	
	异常产生自身没有处理，则向调用者传，最终传递给python解释器，程序挂掉
	
	自定义异常 -- 自定义异常类，创建对象则抛出异常
#endif

#if 补充内容

// -------------------------------------------------------
// 	模块(.py文件)，包(带有__init__.py文件的文件夹)
// -------------------------------------------------------
	os.__file__		// /usr/lib/python3.5/os.py		-- py文件 即是 模块
	
	pip是python管理模块的工具
	
	import sendmsg					// sendmsg.test()
	from sendmsg import test()		// 直接调用test()
	
	后导入的会覆盖先导入的方法
	
	import time as tt				// 某些模块名称很长
	
	先在当前路径搜索该模块，再到安装路径搜索
	
	导入一个模块时，将所有模块内的代码执行一遍
	
	print(__name__)

// ----------------------------------------------
	import xxx
	
	class xxx(object):
		xxx
	
	def func:
		pass
	
	def main:
		pass
	
	if __name == "__main__":
		main()		// 模块测试代码
// ----------------------------------------------

	__all__ = ["test"]		// 管理全局的方法、类、变量		-- from xxx import *
							// 放入__init__.py文件
							// __all__写在.py文件 和 __init__.py文件的不同作用
	
	包: 在一个文件夹下面有多个.py文件，有一个__init__.py文件(python2强制要求)
	
	python使用包的方式不同， -- 对应的导入包的方式也不同
	
// 模块的发布与安装

	import sys
	sys.argv			// 给程序传参
    
    // 当模块不在当前目录，不再默认目录
    import sys
    sys.path.append("../")
    
    // 重新导入模块
    form imp import *       // 导入reload功能
    reload(xxx模块)
	
#endif

#if 深拷贝和浅拷贝
    ==      // 判断值是否相等
    is      // 判断引用
    
    a = [11, 22, 33]
    b = a                   // 浅拷贝  id(a) == id(b)
    
    import copy
    c = copy.deepcopy(a)    // 深拷贝  id(a) != id(c) -- 将列表中的 引用 ， 解引用 拷贝

    bin()   // 转为2进制
    hex()
    oct()
    int()

#endif

#if 迭代器、生成器、闭包、装饰器
// ----------------------------------------------
//  迭代器
// ----------------------------------------------
    可迭代对象：
    生成器一定是迭代器
    
    列表
    
// ----------------------------------------------
//  作用域
// ----------------------------------------------
    dir(__builtin__)    // python内嵌

// ----------------------------------------------
//  动态特性
// ----------------------------------------------
    python动态特性，定义对象后，给对象动态添加属性      // 与对象绑定，类没有修改
                                给类添加属性            // 对象.xxx 实例属性中没有，则到类属性中查找
    
                                给对象添加方法：
                                    p1.eat() = types.MethodType(eat, p1)    // import types
    
    程序发生一些适应性变化
    
    定义类后，可以动态添加属性方法
    __slots__ = ("name", "age")         // 限制属性范围
    
// ----------------------------------------------
//  生成器
// ----------------------------------------------
    保存生成值的算法，每次调用返回生成的值

    a = [x for x in range(10)]      // 列表生成式
    
    b = (x for x in range(10))      // 生成器
    
    //------------------------------------------
    // 使用生成器
    a.__next__()
    
    next(b)
    
    for tmp in b:                   // 生成器可以用于for循环
        print(tmp)
    
    函数中yield，调用该函数返回一个对象    // 生成器对象，该函数称之为生成器
        -- 调用该函数不执行，返回一个对象
        -- 调用next执行，在yield处停止
        -- 以异常的方式，通知不能再继续生成
    // ----------------------------------------
    
    yield可以实现多任务(协程)            // 多任务三种实现：进程、线程、协程
    
// ----------------------------------------------
//  闭包    
// ----------------------------------------------    
    外面函数 返回的是 里面函数 的引用
    
// ----------------------------------------------
// 装饰器
// ----------------------------------------------
    def w1(func)
        def inner():
            print("---新功能---")
            func()
        return inner		// 返回里面函数的引用 not return inner()
        
    def f1():
        print("AAAA")
        
    f1 = w1(f1)
    f1()                // 接口不变、功能新增
    
// ---------------------------------------------
    #f1 = w1(f1)        // 闭包写法。装饰器的原理实现
    
    @w1                 // 装饰器写法  -- 前面定义函数不会执行。这句话相当于执行f1 = w1(f1)
    @w2                 // 先装饰(替换)w2
    def f1():
        print("AAAA")
    
    f1()                // 在调用f1()时，装饰器已经执行完毕
	
	
	@Test				// 装饰器功能执行完毕，test此时保存类Test返回的对象
	def test():
		print("------")
            
	test()				// 类__call__()方法已经重写。对象test() -> 执行__call__()方法
#endif

#if 进程、线程				// 1. target=xxx	2. 继承、重写run方法
	进程与程序之间的区别
	
	父进程可以fork()多个子进程，父进程需要记录子进程的pid
	
	用fork创建的子进程，子进程未退出，主进程可以退出
	用Process()类创建的子进程，子进程未退出，主进程不可以退出 -- 等所有的子进程退出后再退出
	
	p1.join()	// 该子进程结束后，再继续往下运行 -- 通常用于进程间的同步
	p2.join()	// join可以设置超时时间
	
	// ---------------------------------------------
	// 创建子进程方法1 -- 实例化Process实例，传参执行函数			-- 底层都是fork()实现
	from multiprocessing import Process
	import os
	
	def subprocess(name):
		print("AAAA")
	
	if  __name__ == '__main__':
		print("BBBB")
		p = Process(target=subprocess)	// 创建子进程对象 -- 子进程函数名
										// target=subprocess 整体为一个参数，等号两边不要加空格
		print("子进程准备执行")
		p.start()
		p.join()	// 阻塞
		print("子进程执行结束")
		
	// ---------------------------------------------
	// 创建子进程方法2 -- 继承Process类，重写run()方法
	
	// ---------------------------------------------
	// 创建子进程方法3 -- 进程池Pool(需要创建多个进程时)
	
	// 主进程结束，进程池释放 -- 主进程需要等待

// ----------------------------------------------
//	线程
// ----------------------------------------------
	// 创建线程方式1
	from threading import Thread
	
	def test():
		pass
	
	t = Thread(target=test)
	t.start()
	
	// 创建线程方式2
	import threading		// from threading import Thread
	import time
	
	class MyThread(threading.Thread):		// 继承Thread类
		def run(self):						// 重写run方法
			pass
	
	t = MyThread()
	t.start()								// 自动调用run方法
	
	多线程全局变量的优劣 -- 线程不安全
	
// ----------------------------------------------
//	进程VS线程 -- p46
// ----------------------------------------------

// ----------------------------------------------
// 同步、互斥锁
// ----------------------------------------------
	mutex = threading.Lock()	// 创建锁
	mutex.acquire([blocking])	// 获取锁
	mutex.release()				// 释放锁

	help(Lock.release)
	
	由于可以存在多个锁，不同的线程持有不同的锁，可能会造成死锁
	
	from multiprocessing import Queue
	form queue import Queue

// ----------------------------------------------
// ThreadLocal
// ----------------------------------------------

#endif

#if 网络编程
	IP地址 127.0.0.1 可以代表本机ip地址，http://127.0.0.1 可以测试本机中配置的web服务器
	// 0.0.0.0
	127.0.0.1 是一个环回地址。并不表示“本机”。0.0.0.0 才是真正表示“本网络中的本机”
	在实际应用中，一般服务端绑定端口的时候可以选择绑定到 0.0.0.0, 这样服务访问方就可以通过本地多个ip地址访问服务
	比如一台服务器，一个外网地址A，一个内网地址B，如果绑定的端口指定了0.0.0.0, 那么通过内网地址或外网地址都可以访问我的应用。但是如果我只绑定了内网地址，那么通过外网地址就不能访问。所以如果绑定0.0.0.0,也有一定安全隐患，对于只需要内网访问的服务，可以只绑定内网地址
	
	from socket import *
	fd = socket(AF_INET, SOCK_DGRAM)
	
	fd.sendto(data, addr)				// tcp connected后也可以调用sendto()
	data, addr = fd.recvfrom(1024)		// 注意返回值 -- tcp_client.accept()

	// ----------------------------------------------
	// udp发送接收数据流程
	// ----------------------------------------------
		udp客户端				udp服务器
		socket()				socket()
		   ↓					   ↓
		   ↓					 bind()			// listen() ???
		   ↓					   ↓
		   ↓					recvfrom()
		   ↓					   ↓
		   ↓					 ...(阻塞)
		sendto()      --->         ↓
		   ↓					处理请求
		recvfrom()				   ↓
		   ↓					   ↓
		 ...(阻塞)	  <---		sendto()
		   ↓
		 close()
		
	// ----------------------------------------------
	// 下载
	// ----------------------------------------------
		怎样完成下载
			(1) 创建一个空文件
			(2) 向里面写数据		// 迅雷临时文件
			(3) 关闭

		从服务器下载文件
			(1) f = open("test.jpg", "bw")		// bw -- 文件的完整性? -- "\n"写入时的问题

		tftp协议：
			读写请求:

	// ----------------------------------------------
	// struct、pack、unpack
	// ----------------------------------------------
		pc一般为小端模式，IBM大型服务器一般为大端模式
		
		网络字节序，大端发送
		udp.port = 69
		
	// ----------------------------------------------
	// tcp
	// ----------------------------------------------
		// SOCK_DGRAM -> SOCK_STREAM
		
		// 服务器多个套接字，客户端只有一个套接字
		
		connect发送syn J到服务器
		服务器accept解除阻塞，发送syc K，ack J+1 到客户端
		客户端收到后，connect返回，发送ack K + 1 到服务器
		服务器收到后，accept返回
		
		// 0 == len(fd.read())		另一端已经发送FIN -- 应用层调用了 fd.close()
		
		web网站并发量大，无需频繁操作一般用短连接。数据库的连接一般用长连接
		
	// ---------------------------------
	// tcp服务器建立
	// ---------------------------------
		(1) socket创建套接字				// socket默认创建主动套接字
		(2) bind绑定ip和port
		(3) listen将套接字转为被动连接		// 主动套接字转为被动套接字
		(4) accept等待客户端的连接，返回一个新的套接字
		(5) recv/send接收发数据		// send -- sendto
	
	// 万维网购买域名，在github搭建好H1HO博客，将域名绑到这个服务器，将来访问域名直接访问博客(不需要买服务器)
	
	// ----------------------------------
	// tcp10种状态
	// ----------------------------------
	// ---------------------------------------- connection
		client						server
		
									listen
		syn_sent   	   syn-->		syn_recv
		
		established    <--syn+ack
		
						ack-->		established
		
		// ---------------------------------------- close
		
		fin_wait1		fin-->		close_wait
		
		fin_wait2		<--ack
		
		time_wait		<--fin		last_ack
		
		time_wait		ack-->		closed		// 如果这包ack丢掉后，需要再2MSL时间之内重发fin包
		// 2MSL
		closed
		
		// 服务器先结束(ctrl C)，服务器先发fin包，最后再发ack包 -- 需要等2-4分钟
		// 在此期间，服务器套接字不会被释放，重启服务器时会，绑定失败(同一个端口)
		// 客户端不需要绑定特定端口，每次重新分配

		// 360wifi -- ttl-1		判断ttl<128 过滤

		// 127.0.0.1	// 本地服务器

		// python使用原始套接字组包

		// 先在github把网站搭建好，再迁移到服务器上

	// ----------------------------------------
	// tcp服务器实现
	// ----------------------------------------
		socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)	// 目的：不必等待2MSL时间结束，再使用此端口
		// 服务器调用四次挥手的第一次，服务器需要等待2MSL，在此时间段，该端口不允许使用 -- 防止服务器意外关闭，重启不能使用该端口
		// 客户端使用每次随机端口
		// 得到一个socket实例，修改实例一些参数，通过setsockopt方法
		// SOL_SOCKET		SOL : socket option level，设置到socket级别 -- 这次设置到什么级别 (如：SOL_IP, SOL_TCP, SOL_UDP, SOL_LINGER)
		// SO_REUSEADDR  1 -- 设置REUSEADDR选项为1
		
	select相较于使用accept效率提高 -- select底层将可接收的数据的套接字提供到上层
 	
	io多路复用 -- 单进程 实现多路io访问
	// 将套接字设为非阻塞，accept时如果没有客户端连接，产生异常
		server_socket.setblocking(False)
		while True:
			try:
				new_client_info = server_socket.accept()
			except Exception as result:
				pass
			else:
				print("new client")
				new_client_info[0].setblocking(False)		// tcp客户端recv()取消阻塞 -- udp recvfrom()
	
	// ----------------------------------------
	协程、greenlet可以根据需求指定何时切换，切换指定任务
	
	tonador框架使用gevent实现多任务(协程)
#endif

#if web服务器
	http协议用于从万维网(www)服务器传输超文本到本地浏览器的传送协议
	通过使用网页浏览器、网络爬虫或者其它的工具，客户端发起一个HTTP请求到服务器上指定端口（默认端口为80）
	在用户代理和源服务器中间可能存在多个“中间层”，比如代理服务器、网关或者隧道（tunnel）
	
	xml中记录了文本的样式标签 -- word2007 docx(2003 doc)以后，普通文本添加格式的解决方式 -- 用xml
	
	超文本标记语言 -- 既有文本又有样式
	同一个文本既包含内容又包含样式 -- 打开时，经过浏览器渲染
	
	浏览器最核心工作 解析字符串 -- 解析字符串算法		// ie firefox opera safari edge		-- ie 678910版本所支持不同
	浏览器中JS解析器
	Google JSV8引擎(类似于python2 python3解析器版本)，提升JS运行速度
	抽离V8引擎，单独运行JS脚本，脱离浏览器
	
	HTML语言 标签标记了哪些浏览器版本可以支持
	
	改变浏览器的默认行为 -- <h1 style-"color:red;">hello world</h1>
	把这些样式单独提出来，放入一个文件，xxx.css
	让浏览器提供特定的行为，不再是简单的展示 -- JavaScript
	
	element		// 当前页面的样式 -- 如百度图片显示的位置
	console		// js交互工具 -- 类似python交互 -- 查看大公司网站后端实现源码
	source		// 加载当前页面时，所涉及到的资源
	network		// 访问页面时所有网络相关操作
				// Headers/Request Headers -- HTTP报文，请求服务器报文
				// HTTP请求方式 -- 规定行为
				   GET		获取数据
				   POST		修改数据
				   PUT		保存数据
				   DELETE	删除数据
				   OPTION	询问服务器的某种支持特性
				   HEAD		返回报文头
	起始行 -- GET / HTTP/1.1				// 提取请求方式 请求路径。 请求协议
	请求头 -- 类似于python字典的组织方式
	/r/n
	请求体
	
	服务器报文恢复解析
	起始行 -- 200 OK		HTTP状态码			// 500 服务器代码问题，前端无关  -- 404 NOT FOUND
	响应头 -- 键值对形式，以 冒号 分隔
	/r/n
	响应体
	<html>
	</html>
	
	// 请求头、响应头报文，文本形式(大字符串)，通过\r\n分隔，
	
	//输入网址，拿到页面的流程
	客户端发送时，形成请求报文
	服务器接收后，解析字符串，执行特性行为，形成响应报文(字符串)
	客户端收端服务器的响应，将响应体解析(Response) -- 浏览器渲染HTML源文件
	
	静态服务器
	
	https -> http  // 添加安全传输
	
	前端到后端传输数据
	放在请求体 		如登录用户名 密码，头像信息等
	放入请求头		如百度框写入信息 在URL中体现，/ ? 		-- 标准的get方式没有请求体，通过 ? 传输数据，网址从第一个?开始所有都是数据
					无论get post put都可以用URL传输数据，-- URL不仅仅代表资源，? 后都是数据 -- 与本次请求相关的参数
					查询字符串Query String: ?key1=value1&key2=value2
	
	http 无状态		// 问题：登录淘宝后  才可以使用淘宝的购物车 -- 框架使用别的方式解决
	
	http提出针对 超文本传输、网页传输。  -- 不仅仅网页，APP可以使用http协议传输
	
	服务器开发，框架jangle，tonador。
	服务器角度开发：主要针对于 send() 和 receive() 之间，对用户数据的处理，形成用户需要的数据，可能是网页，也可能是某种特定格式的数据，返回给客户端
	客户端角度开发：爬虫(类似于浏览器的，通讯客户端)
	
	服务器执行脚本，实现动态网站
	
	爬虫(spider)
	百度可以搜索到很多网站
	百度搜索相关关键字，相关关键字的资源全部被列举出来
	-- 百度在不停爬取整个互联网 -- 访问过本地网站并记录关键字和链接地址
	-- 搜索引擎公司原理，爬虫 + 搜索算法
	
	
	爬虫 访问 sina.com
	服务器响应请求，index.html  页面资源(带有格式信息)
	拿到页面资源，解析页面信息(字符串)，提取出所有的超链接，搜索链接信息，、
	另一方面记录拿到的页面资源，将链接与页面对应起来(通过分析关键字，与页面的关系)，通过算法分析页面信息中除标签之外的关键字记录
	将链接与关键字存入数据库，保存映射关系(每访问一个页面，提取页面特征值和链接地址)，最终将互联网大部分网站记录下来
	
	爬虫程序并不知道有多少网站，主动去访问所有网站(通过种子链接)，挖掘所有资源，爬遍所有资源 -- 访问时有策略问题
	
// -----------------------------------------
// 	多进程实现web服务器
// -----------------------------------------
	client_socket, client_addr = server_socket.accept()		// 返回值
	p = Process(target=handle_client, args=(client_socket,))
	p.start()
	client_socket.close()		// 关闭父进程的client_socket
	
	client_socket.send(bytes(str, "utf-8"))		// python3必须要转成byte类型，python2不需要
	client_socket.close()		// http短连接需求
	
	print("%s %s connected", %client_address)		// 元组拆包
	
	try:
		file = open(file_name, "rb")	// 考虑到可能打开图片
	except IOError:
		print("file is not exist")
	else:
		file_data = file.read()
		file.close()
		
	// Vagrant  -- 只装虚拟机内核
	
	// 浏览器会预测用户操作，发出请求
	// 浏览没有收到正确响应，会自动再次请求
	// 浏览器会尝试请求logo信息 -- favicon
	
	// 一个页面呈现各种各样的信息，是通过多次请求得到，主页HTML只是框架，包含为本信息，图片资源等信息需要再次请求
	
	// ctrl + C -- 向程序发送 kill -9 信号
	// ctrl + z -- 将程序转为后台运行 -- bg命令查看当前终端的后台程序 -- 将后台程序转为前台运行fg
	
	
#if database
	// -------------------------------------
	目的存储数据，优化读写(数据库算法核心)，大量数据快速查找
	
	关系型数据库，共用E-R设计模型 -- 核心实现代码不同，存取方式算法等，但是数据结构设计都是基于E-R
	
	
	SQLite(轻量级) 移动端使用多，手机、平板、智能家电
	和普通硬盘读取文本文件区别 -- 结构化存储优化，读写速度优化 (提供检索能力(不需要按行读取分析))
#endif
	
#endif

#if 其他
	if num == 1:
		pass		// 只用写一次
	
	backup.data		// 防止普通用户修改
	
	exit()			// 结束程序
    
    a = [11, 22, 33, 44, 55]        // 列表在循环时删除需要注意
    for i in a:
        if i == 33 or i == 44
            a.remove(i)             // 创建一个新列表记录待删除元素
	
    if random.randint(1, 100) == 78     // 延时效果
    
    // _x: 私有化属性和方法，from xxx import * 进制导入。     \
                             import xxx，xxx._x可以使用
	
    property对函数进行简单封装
	
	python一切皆对象
	
	类型实际是一个类
	
	type第二个功能，创建一个新类
	
	// ---------------------------------------
	def __getattribute()__(self, obj):	// 判断条件进行处理，否则扔个父类处理	-- 不可以调用self.xxx
		if obj == "aaa"
			return "bbb"
		else:
			return object.__getattribute__(self, obj)
	
	a.getNum()			// 'A' object has no attribute 'getNum'
	实例对象调用方法：先调用属性，得到函数的引用，再调用函数
	
	// -------------------------------------
	// 集合 -- 不存在相同元素
	b = set(a)
	a = list(b)		// 去重
	
	// 常见模块和扩展模块(进阶3) -- python用途介绍
    
	// ------------------------------------
	// python调试工具pdb
	python -m pdb test1.py
	
	python GIL 多线程 使用一个核
	计算密集型使用多进程
	io密集型使用多线程、协程
	
	pip2安装到python2，pip3安装到python3
	
	作为启动文件，__name__ == "__main__"
	作为包，__name__ == 包名
	
#if xxx1
	《树莓派编程：从Python入门》
	
	vs code
	
	#!/usr/bin/env python3		// chmod a+x hello.py
	
	浮点数也就是小数，之所以称为浮点数
	整数和浮点数在计算机内部存储方式不同，整数运算永远是精确的(包括除法)，而浮点数运算则可能会有四舍五入的误差
	
	静态语言在定义变量时必须指定变量类型，如果赋值的时候类型不匹配，就会报错。
	
	ASCII、Unicode、UTF-8		-- utf-8 without BOM
	
	由于Python的字符串类型是str，在内存中以Unicode表示，一个字符对应若干个字节。如果要在网络上传输，或者保存到磁盘上，就需要把str变为以字节为单位的bytes。

	tuple = ("a", "b", ["A", "B"])		// tuple的每个元素，指向永不变
	tuple[2][0] = "X"
	tuple[2][1] = "Y"
	
	尽量少用break continue，逻辑分叉多。这两个关键字通常配合if使用
	
	list的查找效率问题。dict消耗更多内存，用在高速查找场合
	
	in关键字判断是否存在
	
	set和dict类似，也是一组key的集合，但不存储value。由于key不能重复，所以，在set中，没有重复的key
	set和dict的唯一区别仅在于没有存储对应的value -- key为不可变对象(最常用的key为str)
	
	要始终牢记的是，a是变量，而'abc'才是字符串对象！有些时候，我们经常说，对象a的内容是'abc'，但其实是指，a本身是一个变量，它指向的对象的内容才是'abc'
	
	为什么要设计str、None这样的不变对象呢？因为不变对象一旦创建，对象内部的数据就不能修改，这样就减少了由于修改数据导致的错误。此外，由于对象不变，多任务环境下同时读取对象不需要加锁，同时读一点问题都没有。我们在编写程序时，如果可以设计一个不变对象，那就尽量设计成不变对象
	
	数据类型检查，内置函数isinstance()		// isinstance(x, (int, float))
	
	函数参数：	位置参数
				默认参数 -- 放在必选参数后。调用时可以不设置、按位置设置、指定名称设置 -- 定义默认参数必须指向不可变对象 (举例)
				可变参数 -- 传入参数的个数是可变的(自动组装成一个tuple)
				关键字参数 -- 传入多个含参数名的参数(自动组装成一个dict)
				命名关键字参数 -- 限制关键字参数的名字
				
	给定一个list或tuple，可以通过for循环来遍历这个list或tuple，这种遍历称之为迭代(iteration)
	判断一个对象是否是可迭代对象 -- from collection import Iterable
	
	列表生成式 -- python内置用来创建list的生成式
	a = [x for x in range(10)]		// list
	list(range(10))
	
	a = [0 for x in range(10)]
	list(0 for x in range(10))
	// -----------------------
	a = [range(10)]
	// -----------------------
	b = (0 for x in range(10))		// generator
	// -----------------------
	
	[d for d in os.listdir(".")]
	
	[i.lower() for i in a if isinstance(i, str)]	// 所有的字符串 转 小写
	
	通过列表生成式，可以直接创建一个列表，受到内存限制，列表容量有限
	不必创建完整的list，在循环的过程中推算出后续的元素，这种一边循环一边计算的机制称为生成器 -- generator
	
	创建generator的方法：(1) (2)
	
	generator保存的是算法，每次调用next(g)，计算出g的下一个元素值，一般使用for循环遍历 -- generator是可迭代对象
	
	// -------------------------------------------------------------
	// 接收一个函数、一个序列(iterable)
	// filter()函数返回iterator，是一个惰性序列，要强迫filter()完成计算结果，需要用list()并返回list
	
	map(func, [1, 2, 3])		// <map object at 0x00000132814378B0>
	list(map(func, [1, 2, 3]))	// list
	
	list(map(str, [1, 2, 3]))
	
	reduce(f, [x1, x2, x3, x4]) = f(f(f(x1, x2), x3), x4)	// 把结果和序列剩余元素继续运算
#endif

#endif

#if tmp
threading.current_thread().name

对象没有使用__slots__修饰，可以任意添加属性

前端 阿贾克斯(页面局部刷新) 使用到异步		// 如网络中经常使用

python的多线程由于GIL原因是伪多线程
python中多核cpu，多进程效率高于多线程

#endif




