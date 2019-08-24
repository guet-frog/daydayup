
#if /* 3 - 程序结构 */、

	(1) 在类外定义函数 void Person::printInfo(void)
	
	(2) person: main.o person.o 	// 我们想编译出person app，它依赖于main.o person.o
			g++ -o $@ $^			// 如果有这两个文件，则输出$@(person)。$@ -- person $^ -- 所有的依赖
			
		main.o: main.cpp			// main.o依赖于main.cpp
		g++ -c -o $@ $< 			// 输出$@(main.o) $< -- 第一个依赖	-- -c
		 
		// person.cpp和main.c一样编写 -- 简化 使用通配符 -- %.o: %.cpp

	(3) 在.c .h文件中添加 namespace{ ... }		A::printVersion()	// 使用A命名空间的func()
#endif

#if /* - 4 引用 */

	(1) 引用相当于定义别名，定义引用同时进行初始化
		int &c = a;		// ok
#endif

#if /* - 5 构造函数*/
	
	(1) Person per("lisi", 18, "student");
	
	(2) Person *per = new Person("lisi", 18, "student");
		delete per;		// 或者程序运行结束自动释放
		
	(3) free -m		// 以M为单位 输出
	
	(4) 析构函数	// 多个构造函数对应一个析构函数 -- 无参
		函数执行完毕，Person自动销毁 -- ???
		
		void test_fun()
		{
			Person per("zhangsan", 16);	// 后执行析构函数
			
			Person *per1 = new Person("lisi", 18, "student");
			delete per1;	// 先执行析构函数
		}
		
		// new出来的对象 自动销毁时 不调用析构函数
		
	(5) c++提供默认的 构造函数 析构函数 拷贝函数	// Person per2(per);
		// 默认拷贝函数 两个成员

#endif