
3.3.1.1、GMT时间	// old

3.3.1.2、UTC时间	// new
	// http://www.cnblogs.com/qiuyi21/archive/2008/03/04/1089456.html

3.3.2.1、jiffies的引入		// systick_count

3.3.2.3、linux中时间相关的系统调用
	(1) 常用的时间相关的API和C库函数有9个：
		time/ctime/localtime/gmtime/mktime/asctime/strftime/gettimeofday/settimeofday

3.3.3.1、time
	(1) 返回当前时间以秒为单位的距离1970-01-01 00:00:00 +0000(UTC)过去的秒数，其他几个函数的基础
	
3.3.3.3、gmtime和localtime
	(1) 返回struct tm类型的时间

3.3.3.2、ctime，asctime，strftime
	(1) 返回一个容易观察的字符串格式的当前时间，输入time_t类型
	(2) 返回一个容易观察的字符串格式的当前时间，输入struct tm类型
	(3) 返回自定义格式的时间

3.3.4.1、mktime
	(1) 从OS中读取时间时用不到mktime的，这个mktime是用来向操作系统设置时间时用的。

3.3.4.4、gettimeofday和settimeofday
	(1) 前面讲到的基于time函数的那个系列都是以秒为单位来获取时间的，没有比秒更精确的时间。
	(2) 有时候我们程序希望得到非常精确的时间（譬如以us为单位），这时候就只能通过gettimeofday来实现了

	// 总结：本质上最终都是从kernel中记录的jiffies中计算得到，不同的函数返回的时间的格式不同，精度不同

3.3.5.5、在linux系统中获取真正的随机数

3.3.6.2、proc虚拟文件系统的工作原理
	(5) 在内核中构建一个虚拟文件系统/proc，内核运行时将内核中一些关键的数据结构呈现在/proc目录中的一些特定文件中
		这样相当于将不可见的内核中的数据结构以可视化的方式呈现给内核的开发者
	(6) proc文件系统给了开发者一种调试内核的方法：通过实时的观察/proc/xxx文件，来观看内核中特定数据结构的值

3.3.6.3、常用proc中的文件介绍
	(1) /proc/cmdline
	(2) /proc/cpuinfo
	(3) /proc/devices
	(4) /proc/interrupts

3.3.7.3、扩展：sys文件系统
	(2) /proc中的文件只能读，/sys中的文件可以读写
		读/sys中的文件就是获取内核中数据结构的值，写入/sys中的文件就是设置内核中的数据结构的元素的值

	// 都是内核中数据结构的可视化接口，主要用来调试驱动

