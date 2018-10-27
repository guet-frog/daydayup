
2.9.1.1、环境变量的作用
	(1)不修改uboot的源代码，而是通过修改环境变量来影响uboot运行时的一些数据和特性
		譬如说通过修改bootdelay环境变量就可以更改系统开机自动启动时倒数的秒数

		// 环境变量与全局变量的区别: 两次复位上电, 环境变量具有相关性

2.9.1.2、环境变量的优先级
	(1)uboot代码当中有一个值，环境变量中也有一个值		// env优先
														// s = getenv("machid");  if (NULL = s){}	-- Bootm.c do_bootm_linux()

2.9.1.3、环境变量在uboot中工作方式
	(1)default_environment		// uboot/common/env_common.c
		里面内容就是很多个环境变量连续分布组成的, 每个环境变量最末端以'\0'结束
		// gcc提供的语法支持 多个字符串"" "" ""自动连接, "aaa"的'\0'编译器给省略掉, 所以手动添加'\0'
		// 全局变量, 重定位到ddr中

	(2)SD卡中环境变量分区, 在uboot的raw分区中, 刚烧录的系统中环境变量分区是空白的	// relocate后crc校验失败, 使用default_environment

2.9.2.1、printenv	// -- 环境变量在内存中的组织形式

2.9.3.1、setenv
	第1步：遍历DDR中环境变量的数组，找到原来就有的那个环境变量对应的地址。168-174行。
	第2步：擦除原来的环境变量，259-265行
	第3步：写入新的环境变量，266-273行。
	(3)本来setenv做完上面的就完了，但是还要考虑一些附加的问题。
	问题一：环境变量太多超出DDR中的字符数组，溢出的解决方法。
	问题二：有些环境变量如baudrate、ipaddr等，在gd中有对应的全局变量。
		这种环境变量在set更新的时候要同时去更新对应的全局变量，否则就会出现在本次运行中环境变量和全局变量不一致的情况。

	// -- do_setenv()	// 没有下划线的可以修改
		|
		| -- _do_setenv()	// 	一个下划线的系统自己使用
		|
		| -- __do_setenv() 	// 两个下划线是系统内部自己用
		|
		| -- ___do_setenv() // warning
	
2.9.4.1、saveenv
	void movi_write_env(ulong addr)
	{
		movi_write(raw_area_control.image[2].start_blk, 
			raw_area_control.image[2].used_blk, addr)
	}
	// param1: image[2] -- env分区; start_blk -- 起始扇区号
	// param2: used_blk = 32 -- x210_sd.h配置

2.9.5.1、getenv、getenv_r	//是否可重入





