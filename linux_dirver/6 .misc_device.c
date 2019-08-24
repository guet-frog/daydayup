
5.6.2.misc类设备介绍
	(2) 典型的字符设备 /sys/class/misc		// device_create()后没有/sys/devices/misc
	(5) misc是对原始的字符设备注册接口的一个类层次的封装，很多典型字符设备都可以
		归类到misc类中，使用misc驱动框架来管理
		// 内核框架(创建misc类、提供register接口)：misc.c
		// 驱动：x210-buzzer.c

		// bitmap机制

5.6.4.1、open函数分析			// 一般框架不会实现具体函数

5.6.4.3、内核互斥锁
	(1) 计数值为1的信号量 = 互斥锁

5.6.5.2、ioctl
	(1) 为什么需要ioctl（input output control，输入输出控制）
