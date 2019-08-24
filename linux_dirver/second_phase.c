
#if	/* 12.1 字符设备驱动程序概念 */

	(1) 应用程序在调用open()，read()等时，实际通过执行swi指令(swi xxx)，这条汇编
		指令会引发一次异常，进入系统异常处理函数中(进入内核中)，在异常处理函数中
		通过判断xxx调用不同的处理函数

	(2) 根据打开的文件类型(文件属性、主设备号)来调用/*相应*/的open、read函数
#endif

#if /* 12.2.1 字符设备驱动led */

	(1) 定义file_operation如何通知内核 -- register_chrdev()
		register_chrdev()被谁调用(驱动的入口函数是哪一个) -- module_init()

	(2) 字符设备驱动通过数组来管理，major为下标

	(3) 自动创建设备文件 -- 应用程序的udev机制(mdev)
		利用系统信息(注册驱动时会在/sys目录下生成设备信息)，mdev会根据系统信息自
		动创建设备节点

	(4) dev_class = class_create();		// 创建类
		dev_device = class_device_create();		// 类下创建设备
	
	// --------------------------------------------------------
	(5) /etc/init.d/rcS
		echo /sbin/mdev > /proc/sys/kernel/hotplug
	// --------------------------------------------------------
#endif

#if /* 12.2.2 字符设备驱动led */

	(1) if (argc != 2)
		{
			printf("Usage: \n");
			printf("%s <on|off>\n", argv[0]);	// linux中<>表示参数不可省略
		}

	(2) int var = 1;
		write(fd, &var, 4);

		if (1 == var)		// if ('1' = var)
		{
			// ...
		}

	(3) // 编写驱动程序
		1，首先完成框架 -- file_operation相关注册
			-- file_operation
			-- 实现 使用到的 open read 函数
			-- 入口函数 -> 注册file_operation -> 宏修饰入口函数
			-- 出口函数 -> 注销file_operation -> 修饰
			-- 给sysfs提供信息 -> udev机制可以自动创建设备节点 -> 创建class，class下创建设备
			-- struct class *class； struct class_device *dev;
			-- 在入口创建的，在出口卸载

			-- make; insmod; lsmod; ls /dev/xxx -l		// 框架编写完毕

		2，对硬件的操作		// 与单片机相同
			-- 看原理图
			-- 看soc手册
			-- 编程		-- 虚拟地址 ioremap()

	(4)	static int s3c24xx_leds_open()
		{
			int minor = MINOR(inode->i_rdev);		// MINOR(inode->i_cdev)
		}

		static int s3c24xx_leds_read()
		{
			int minor = MINOR(filp->f_dentry->d_inode->i_rdev);
		}

	(5) // 使用资源及释放资源，申请资源的同时考虑资源何时释放

	(6) s3c_gpio_setpull(S5PV210_GPH0(3), S3C_GPIO_PULL_UP)	// 不需要ioremap -- 静态映射? 
		s3c_gpio_cfgpin();

	(7) 使用子设备号管理多个led
		for (minor = 1; minor < 4; minor++)
		{
			leds_class_devs[minor] = class_device_create(leds_class, NULL, MKDEV(LED_MAJOR, minor), NULL, "led%d", minor)
		}
#endif

#if /* 12.3 按键查询方式 */

	(1) 入口 -- 地址映射		// 在写代码 -- 入口函数中使用资源，同时在exit中释放资源
		open -- 配置引脚
		read -- 返回引脚状态
	
	(3) 读按键
		unsigned int regval;
		regval = *gpfdat;
		key_val[0] = (regval & (1 << 0)) ? 1 : 0;	/* - 赋值0或1 可以考虑使用三目运算符 */

	(4) read(..., size_t size) 参数判定 及 返回值
		 if (size != sizeof(key_vals))
			 return -EINVAL;

		 return size; 		// ok

	(5) /* 配置引脚为输入引脚 */
		*gpfcon &= ~((0x3 << (0*2)) | (0x3 << (0x3 << (2*2))));

		// s3c2410_gpio_getpin();  -- 读引脚

	(6) ./xxx $		// app在后台运行
		top			// 任务管理器
#endif

#if /* 12.4.1 linux异常处理结构 */

	(0) 单片机处理终端
		-- 按键按下
		-- cpu发生中断，调到异常向量入口执行
		-- b 函数
			-- 计算返回地址
			-- 保存被中断的现场
			-- 执行中断处理函数
			-- 恢复

	(1)	// p419
	(2) traps.c -- void __init early_trap_init(void)
		{
			__vectors_start		// entry_armv.S 是一些跳转指令  // 7′ 跳转指令(宏)的讲解
		}
		
		// 10′ 异常向量 如何转到 asm_do_irq
		
		trap_init()		// 构造异常向量(一些跳转指令)	-- b vector_irq + offset -- 因为vector_irq是链接地址，向量表被重新定位，所以要加上offset才能正确的跳转
#endif

#if /* 12.4.2 中断处理结构 */

	(1) do_asm_irq() // 中断时如何进行的 -- 分辨是哪个中断，调用处理函数，清中断
	
	(2) request_irq() 	// 中断号、处理函数、flag、name、dev_id
		-- 分配irqaction结构，初始化成员
		-- set_irq(irq, action)
			-- irq_desc[irq]		// 全局数组，以中断号为下标
		-- 设置引脚、使能中断
	
	(2) free_irq() // 中断号、dev_id
		-- 出链
		-- 判断是否需要禁止中断
#endif

#if /* 12.4.3 中断方式的按键驱动 */
	
	(1) request_irq() // 自动配置引脚为中断方式 -- irq 和 dev_id标记某一个irq，卸载用
	
	(2) .release = close;  // 关闭设备时，释放irq
	
	(3) // irq申请定义在open函数中，打开文件测试
		exec 5</dev/buttons			// 打开文件，文件描述符定位为5
		cat /proc/interrupts

		ps			// 查看当前shell pid为771
		ls -l /proc/771/fd
		exec 5<&-		// 关闭设备文件，（中断释放）

		lsmod
		used by 2		// thied_drv被两个使用
		exec 5<$- 	// 关闭使用	-- used by 0
		rmmod		// 再卸载模块

// --------------------------------------------------------------------
// 上面完成按键中断框架编写，测试中断框架OK
// --------------------------------------------------------------------

	(4) 重新编写中断服务函数，通过中断号来判断哪一个按键被按下
		(a) switch (irq)
			{
				case xxx:
			}
			
		(b) struct pin_desc
			{
			
			}

	(5)	底层实现休眠
		static DECLARE_WAIT_QUEUE_HEAD(button_waitq);		// 定义结构体，并初始化

		wait_event_interruptible(button_waitq, ev_press);		// 休眠 -- 进程挂载某个队列下

		wake_up_interruptible(&button_waitq);	// 唤醒 -- 把挂在队列(button_waitq)的进程唤醒

	(6) 安装驱动前，卸载老驱动
		rmmod third_drv
		rmmod: third_drv: resource temporarily unavailable
		ps
		kill -9 787
		rmmod third_drv

		./thirddrvtest &	// 在后台运行
		ps
		stat -> S 	// 该进程处于sleep状态

		34′ 处理一个内核崩溃
#endif

#if /* 12.5 poll机制 */

	(1) 应用层的poll() 对应 file_operation中的poll()
		app: 	-- poll()
		kernel: -- sys_poll()
					-- do_sys_poll(..., timeout_jiffies)
						-- poll_initwait()
						-- do_poll(..., timeout)
							-- for (;;)
								{
									for (; pfd != pfd_end; pfd++)	// 查询多个文件(多个驱动程序)
									{
										if (do_pollfd())	// mask = file->f_op->poll(); return mask;
										{
											count++;
											pt = NULL;
										}
								}
								
								// break条件：count非0, 超时, 有信号在等待处理
								if (count || !*timeout || signal_pending(current))
									break;
								
								// 休眠__timeout
								__timeout = schedule_timeout(__timeout);
								
		驱动的 -- poll()
				-- poll_wait(file, &button_waitq, wait)
					-- p->qproc(filp, wait_address, p)	// __pollwait(filp, &button_waitq, p)
					// 把当前进程挂到button_waitq队列中，当有按键按下才可以唤醒该进程
					// -- 中断方式是直接通过read函数，把进程挂在队列中

		// 驱动实现的poll
		static unsigned forth_drv_poll(struct file *file, poll_table *wait)
		{
			int mask = 0;
			
			poll_wait(file, &button_waitq, wait);	// 不会立即休眠只是添加到xxx队列中
			
			if (flag)
			{
				mask |= POLLIN | POLLRDNORM;		// 执行count++
			}
		}
		
		__timeout = schedule_timeout(__timeout)
		
	(2) int poll(struct pollfd *fds, nfds_t nfds, int timeout)
		
		a) 定义变量 struct pollfd fds[1];		// poll可以查询多个fd
					fds[0].fd = fd;		// fd[0]
					fds[0].events = POLLIN; // 有数据等待被读取
		
		poll(fds, 1, 5000)
#endif

#if /* 异步通知 */	// 2019-7-21 22:10:30重新听

	(1) // 按键操作
		应用程序主动去read：查询, 中断, poll(select)
		驱动主动触发app：异步通知(signal实现)
	
	(2) 进程间发信号  kill -9 pid
	
	(3) 注册signal与irq类似
		signal(SIGUSR1, signal_func);
		
		kill -USR1 pid
		kill -10 pid
		
		要点：a> 注册信号处理函数(与注册中断服务函数类似)
			  b> 谁发信号
			  c> 发给谁
			  d> 怎么发
			  
			  a> 应用程序，注册信号处理函数
			  b> 驱动发
			  c> 发给app	-- app要告诉驱动pid
			  d> 内核调用kill_fasyn()
#endif

#if /* 同步互斥阻塞 */
	
	(1) 目的：同时刻只能有一个app打开 /dev/buttons	 -- open中做一个标记
#endif

#if /* 1 - 定时器防抖动 */

	(1) 定时器：超时时间、处理函数
		// struct timer_list
		// init_timer()
		// del_timer()
	
	(2) jiffies -- 全局变量，每隔10ms参数系统时钟中断，累加
		一秒 = HZ	10ms = HZ/100
		cat /proc/interrupts -- systick值累加
#endif

#if /* 2 - 输入子系统 3 - 编写驱动程序 */
	
	(1) 按键中断程序弊端：
	    不能用在现成的应用程序中，如QT。
		-- 不可能打开指定的设备文件，可能打开的是其他设备文件，或者直接scanf()获取按键输入
	
	(2) input子系统框架		// input.c -- 核心层
							// handler层
							// 设备层
							
	(3) static struct input_handler *input_table[8]
			-> input_register_handler(struct input_handler *handler) // -- not handle -- Evdev.c Keyboard.c Mousedev.c等调用

					-> input_table[handler->minor >> 5] = handler;	// 放入数组

					-> list_add_tail(&handler->node, &input_handler_list); // 放入 handler 链表

					-> list_for_each_entry(dev, &input_dev_list, node)		// 用handler来遍历dev链表
							input_attach_handler(dev, handler);

			-> int input_register_device(struct input_dev *dev) // -- 大量调用

					-> list_add_tail(&dev->node, &input_dev_list);	// 放入 dev 链表

					-> 	list_for_each_entry(handler, &input_handler_list, node) // 这是一个for循环，将每一个input_handler，都调用一次input_attach_handler()
							input_attach_handler(dev, handler);

	(4) // 如果app调用read(), 则执行input_table->new_fops->read()
			-> evdev_open() // -- input_table[]
				-> evdev_fops->open()
					-> read() // -- 休眠
						-- 谁来唤醒 --
						-> irq_handler()
							-> input_event()		// 之前 中断中 直接唤醒 进程
								-> dev找到 dev->h->list -- h_list找到handle
								    handle找到handler -- 调用handler->event()
								-> event()中记录到buff中，然后发信号 或 唤醒app

	(5) 两种按键驱动，硬件操作永远是相同的
	    不需要自己编写open、write。evdev.c中已经构造，只要完成硬件相关操作
	
	(6) -- input_event()
		 | 
		 | -- input_pass_event()
		    |
			| -- list_for_each_entry_rcu(handle, &dev->h_list, d_node)
			   |
			   | -- if (handle->open)
				  |
				  | -- handler->event(handle, type, code, value)
	
	(7) 装载之前首先 ls -l /dev/event*
					 insmod
					 ls -l /dev/event*


	// 为什么分配到的主设备号13 次设备号64
		之前在input.c中
		class_register()  	// 创建了类
		register_chrdev()   // 注册了file_operation
		但是没有在类下创建设备
		
	// 什么时候创建设备
		注册设备input_register_device()
		从注册的handler中取出比较 -- evdev.c keyboard.c 
		如果支持就调用connect函数建立联系
		
		
		判断evdev.c能否支持buttons_dev
		支持
		在connect函数中
		devt = MKDEV()
		
		
		cat /dev/tty1					// tty_io.c驱动来访问 keyboard.c
		-- 没有添加event_sync()
		-- qt占用
		
		
		hexdump /dev/event1				// 测试input子系统驱动，常使用hexdump
		
 		不确定内核是否支持另外一个handler /dev/tty1  -- 确认keyboard.c(handler)有没有编译 -- 直接到目录下看是否有.o文件
		ls -l /dev/tty1 -l
		
		
		// 修改测试OK
		cat /dev/tty1
		ls
		exec 0</dev/tty1  // 标准输入改为tty1
		
		// cat /dev/tty1 在串口上等待输入，执行我们串口输入的程序时shell
		// 标准输入以前是由串口输入
		// exec 0</dev/tty1  把串口输入改为由键盘输入
		
		// 根据我们输入删除显示的是 -sh程序		-- 文件接收我们的输入和删除 根据文件显示
		  ls -l /proc/770/fd
		
		标准输入 -- 从哪里得到数据		// exec 0</dev/tty1
		标准输入 -- 
		
		重复输入 EV_REP -- 启动定时器
		
		
		// 将整个输入抽象为两层，底层分为左右两边
		// 核心层和事件驱动层保持稳定不变 -- 应用层的接口固定，即使硬件修改，应用层不用改变
		
		对所有输入事件的抽象
		struct input_event
		{
			struct timeval time;
			__u16 type;
			__u16 code;
			__u32 value;
		}
#endif

#if /* 4 - 总线驱动模型 */
	(1) 匹配后自动调用probe() remove()
	
	(2) 驱动编译链接OK。insmod报错 -- 添加相关头文件解决 44′
	
#endif

#if /* 5 - lcd驱动程序分析 */
	(1) 分层思想：抽出一些共性的东西
	
	(2) fb_open() -- registered_fd[]

	// fbmem.c input.c 都是抽象出来的共性。open() write()都由一个接口进入(主设备号一定)
	// 应用层的接口从而定死。如：ioctl传参命令类型固定
	// 具体硬件操作都需要 xxx数组 -- 数组 = register_xxx
	
	// fbmem.c是抽象的驱动框架，并不能完成对硬件的操作。需要底层提供驱动，来 提供 注册fb_info
	
	// 以前写驱动一个文件搞定，现在kernel提供框架，借用框架必须按照框架要求提供相关操作
	
	(3) register_xxx -- 类下创建设备 -- 有设备才有必要创建设备节点
	
	(4) probe() -- 获取硬件相关信息 -- malloc(fb_info) -- 设置 -- 硬件相关操作 -- register_framebuffer()
	
	(5) 如何知道从一个像素到另一个像素  -- VCLK信号
		如何知道从一行到另一行			-- HSYNC信号
										-- VFRAME帧同步信号
										-- VDEN像素使能信号(黑框)
	(6) 硬件操作							
		不同lcd的VCLK不同，查手册设置
		分配显存，设置显存地址到lcdc
		像素字节数设置
		配置引脚
#endif

#if /* 7 - lcd驱动编写 */
	
	首先先写入口函数
	static int lcd_init(void)
	{
		//1. 分配fb_info结构体
		s5pv210_lcd = framebuffer_alloc(); // 结构体的大小固定，分配函数仍需要提供size？ -- kzmalloc 分配并清零
		
		//2. 设置
		//2.1 设置固定参数
		//2.2 设置可变参数
		//2.3 设置操作函数
		//2.4 其他的设置
		
		//3. 硬件相关的设置
		//3.1 配置gpio
		//3.2 根据lcd手册设置lcdc，如vclk
		//3.3 分配显存(framebuff)，告诉地址到lcdc
		
		//4. 注册
		
		固定参数中 -- 
		
		可变参数 -- 虚拟分辨率，显示器的物理分辨率一定，电脑仍可以设置的分辨率
		
		ioremap()  //至少按照page映射 1024
		
		dmsg // 查看启动信息
		
		vckl -- 100ns // 只能慢，不能快
	}
	
	// ------------------------------------------------
	echo hello > /dev/tty1		// hello
	cat xxx > /dev/fb0			// 花屏
	
	vi /etc/inittab
	s3c2410_serial0::askfirst:-/bin/sh	// 启动一个shell程序，从串口0得到输入，输出到串口0
	tty1::askfirst:-/bin/sh		// dev/tty1 输入时对应键盘，输出对应lcd  -- ls -l /proc/768/fd
	
	tty1 -- fbcon.c		// 最终映射到framebuff
	      |
		  | -- fbcon_rotate()
		     |
			 | -- registered_fb[]		// 获取 fb_info
			 
	// fb_info中有显存，显示文字时该驱动会自动获取字模，然后映射到lcd显存中
	
	采用平台总线设备模型，将lcd驱动拆分为两个部分，一部分比较稳定的代码保持不动
	一部分硬件相关的代码 -- struct platform_device s3c_device_lcd = {}
	调用s3c24xx_fb_set_platdata()设置平台私有数据 -- 寄存器设置值
		
#endif

#if /* - 触摸屏驱动框架 */
	
	(1) 采用input子系统实现
		input_dev = input_allocate_device()		// 分配 struct input_dev
		
		设置能够产生 哪些类 事件		// 产生按键事件
		设置能够产生 按键类 哪些按键
		
		注册
		
		硬件相关的操作	// 如注册中断
		
		有事件发生时上报事件		// touch_timer_fire() -- input_report_abs() -- input_event();
		
	(2) 触摸屏实际是adc转换器
	
	(3) cp -rfd		// 递归 强制 链接文件保持链接文件
	
	(4) /dev/input/event0	// 触摸屏 和 lcd 不是同一个设备
	    /dev/input/fb0		// lcd
		
	(5) 装载驱动出现segment fault		// kernel重新配置过，驱动要重新编译一下
	
	(6) 删除注释符，以及前面的空格
#endif

#if /* 14 - USB驱动程序介绍 */

	(1) 总线设备驱动模型只不过提供了一种框架，probe()中可以做任何操作
	
	(2) 将鼠标模拟成键盘 -- input子系统
		a> 分配input_dev
		b> 设置
		c> 注册
		d> 硬件操作
			--> 对于按键驱动: 注册中断，在中断中读引脚，确定按下松开确定按键值
			--> 对于触摸屏驱动：设置adc控制器
			--> 对于usb驱动：使用usb总线驱动提供的读写函数来手法数据
			
			-- // usb驱动和其他以前驱动的不同 -- 硬件操作不同
			
	(3) 如何编写usb 设备 驱动
		a> 分配/设置 usb_driver结构体	// -- 面向对象的思想
			.id_table
			.probe
			.disconnect
		b> 注册
		
		// usb主机控制器 不断查询usb设备 如果有数据 控制器向cpu产生中断
#endif

#if /* 19 - 块设备驱动程序框架 */

	(0) /*块设备*/相较于/*字符设备*/ 不能直接提供读写函数
		a> 把"读写"队列放入队列，先不执行
		b> 优化后在执行
		
	(1) 应用层read() write()对文件的操作，映射为对扇区的硬件读写操作 -- 文件系统：文件读写转化为扇区读写
		// 新硬盘 U盘 首先需要格式化
		
	(2) 块设备的读写被文件系统转化为底层扇区的读写 调用统一接口ll_rw_block()	// -- fs/buffer.c
		ll_rw_block()
			-- 把"读写"放入队列
			-- 调用队列的处理函数(优化/调顺序/合并)
		
	(3) 《linux内核源代码情景分析》
		
	(4) 怎么写块设备驱动程序
		a. 分配gendisk结构体: alloc_disk()		// 面向对象思想，分配 填充 注册 结构体
		b. 设置
		b1. 分配/设置队列: request_queue_t		// 队列提供读写能力
			blk_init_queue()
		b2. 设置gendisk其他信息					// 提供属性：如容量、厂商
		c. 注册: add_disk()
#endif

#if /* 20 - 内存模拟磁盘 */

	/* 1. 分配一个gendisk结构体 */
	static struct gendisk *ramblock_disk = alloc_disk()		// 形参minors(分区个数) -- ls /dev/sd*
	
	/* 2. 设置 */
	/* 2.1 分配/设置队列: 提供读写能力 */
	static struct request_queue_t ramblock_queue = blk_init_queue(requeset_fn_proc *rfn, spinlock_t *lock)	// rfn -- 队列的处理函数
											// 调用默认函数将请求放入队列，调用rfn来进行处理
	ramblock_disk->queue	= ramblock_queue
	
	/* 2.2 设置其他属性 */
	ramblock_disk->major = register_blkdev()	// 相对于register_chrdev() 没有提供file_operation
												// cat /proc/devices
	set_capacity(ramblock_disk, size / 512)		// 设置容量 单位/扇区。在fs中扇区 = 512字节
	
	/* 3. 注册 */
	add_disk(ramblock_disk)
	
	
	// request_fn_proc *rfn
	static void do_ramblock_request()
	{
		while ((req = elv_next_request(q)) != NULL)
		{
			/* - 硬件操作相关 */
			/* - 取出请求，实现磁盘读写 */
			
			// 内存模拟 -- memcpy()实现对硬件操作(读写内存)
			
			/* - 数据传输三要素：源、目的、长度 */
			/* 源/目的: */
			unsigned long offset = req->sector << 9;	// * 512
			
			/* 目的/源: */
			// req->buffer
			
			/* 长度 */
			unsigned long len = req->current_nr_sectors * 512;
			
			if (rq_data_dir(req) == READ)
			{
				memcpy(req->buffer, ramblock_buf + offset, len);
			}
			else
			{
				memcpy(ramblock_buf + offset, req->buffer, len);
			}
			
			end_request(req, 1);
		}
	}
	
	// ------------------------------------
	装载
	insmod ramblock.ko
	ramblock:unknow partition table		// -- 分配的内存全部清零，无分区表
	
	格式化 -- mkdosfs /dev/ramblock
	
	挂接 -- mount /dev/ramblock		/tmp/
	
	读写文件 -- vi a.test
	
	umount /tmp/
	
	cat /dev/ramblock  > /mnt/ramblock.bin		// 整个 磁盘 拷贝到 /mnt/ramblock.bin 文件 -- 相当于磁盘映像
	sudo mount -o loop ramblock.bin 	/mnt 	// -o loop将普通文件当做块设备挂接
	
	// --------------------------------------
	static int r_cnt;
	printf("%d\n", ++r_cnt);
	
	cp /etc/xxx /tmp
	read	// -- 先打印
	write	// -- 再打印，块设备读写操作先放入队列没有立即执行，等了一会才打印出来
	
	cp /etc/xxx /tmp
	sync				// 系统调用 同步
	
	// --------------------------------------
	fdisk
	// unknow value cylinders -- 柱面数不知道		// 磁头(多少面) 柱面(多少环) 扇区
	
	.getgeo = xxx;		// 容量 = heads * cylinders * sectors * 512;
	
	insmod xxx
	ls /dev/ramblock		// 当前有几个ramblock -- 次设备号 = 0 表示当前整个ramblock
	fdisk /dev/ramblock		// 圆柱的外环
	
	fdisk n p w				// 分区表；磁盘的第一个扇区
	
	可以对两个分区 分别挂接 分别格式化 分别操作
	
	// ---------------------------------------
	块设备驱动框架，主要2.1 分配、设置队列，提供读写函数
	以后对磁盘的格式化、对文件的读写，由fs转化为对扇区的读写
	调用ll_rw_block() 将读写放入队列，调用队列内的函数进行处理 -- /*提供处理队列的函数*/
	
#endif

#if /* 22 - NANDFLASH驱动框架 */
	
	ls -l /dev/mtd* -l 		// 既有字符设备又有块设备 -- 通过fs的方式，只适用于块设备
	
	----------------------------
	APP: open, read write
	----------------------------
	VFS: sys_open, sys_read					-- 根据文件类型：普通文件通过fs找到块设备驱动
	----------------------------
			|  FS: vfat, yaffs2, jiffs2
			|  ----------------------------
	字符	|  				  块设备        -- 知道怎么优化，
	设备	|  				----------------  
	驱动	|  块设备驱动     nand协议层	-- 知道 发什么 来识别、读写、擦除		// 只要是nand都必须遵从nand协议
			|  				-----------------
			|  			  	  硬件相关代码  -- 知道 怎样发 命令/地址，读写数据
	----------------------------
				硬件
	----------------------------
	
	// mtd设备：flash
	
	举例：
	nand协议层 -- nand_scan(): 确定可以 发出90命令 发出0地址，然后读出第一个数据是厂家ID，第二个数据是设备ID
							   协议层知道发出90命令，但是不知道如何发出，需要硬件相关层提供函数(发命令、发地址的收发函数)
							   把除最底层的差别，其他抽象实现
							   我们只需要完成硬件相关操作 --  实现nand_chip结构体中函数
							   -- 分配				// struct nand_chip{}
							   -- 设置
							   -- 硬件相关操作
							   -- 使用	(not 注册)	// nand_scan()/add_mtd_partition() 扫描完成后添加分区
								
	(1) 由启动信息查找nand相关文件
	
	s3c2410_nand_init_chip();	// 硬件相关操作不关心
	
	nand_scan()		// 谁实现？ -- // drivers/nand/nand_base.c
								   // 通用的
								   // 所有nand都需要支持 选中 发命令 读id。必须响应
	
	-- nand_scan()
	 |
	 | -- nand_scan_ident()
	    |
		| -- nand_get_flash_type()
	
	// 如果kernel中有同名的平台dev，则会调用driver的probe函数
	
	// 21′ add_mtd_device()
	
	// nand协议层: 构造 struct mtd_info{.read; .write; .erase};
	// 硬件相关:   构造 struct nand_chip{};
	
	// 分层概念: 硬件相关层负责如何发命令、发地址、发数据			\
				 nand协议层负责如何擦除、读写(发什么命令来读写)
#endif

#if /* 23 - NANDFLASH编写代码 */
	
	/* 1. 分配一个nand_chip结构体 */
	
	/* 2. 设置  --  设置是为了使用，不知道如何设置，先看看是如何使用 -- nand_scan() */
	
	/* 3. 硬件相关的设置 */
	
	/* 4. 使用: nand_scan() */
	
	/* 5. add_mtd_partition */
	
	
	// 内核外设默认时钟都是关闭
	// 可以ioremap控制CLKCON寄存器
	// 也可以info->clk = clk_get()	-- clk_enable(info->clk);
#endif

#if /* 25 - NANDFLASH驱动测试*/

	// 5. add_mtd_partition()	-- 如果不需要分区，只有一个区，可以直接调用add_mtd_device()
	
	(1) 在启动信息中搜索默认分区 "bootloader"
		common_smdk.c -- static struct mtd_partition smdk_default_nand_part[] = {};
		MTDPART_OFS_APPEND 	-- 紧跟着上一个分区
		MTDPART_SIZ_FULL	-- 剩余空间大小
		
	(2) 取出内核nand驱动，重新编译，加载内核。设置nfs启动
	
	(3) ls /dev/mtd*
	    出现四个分区 四对字符设备驱动
		
	(4) mount /dev/mtdblock3 /mnt
		cd mnt	// 原rootfs文件都在
		
	// 因为nand之前烧写过文件系统，可以直接挂接
	// 如果没有，需要先格式化，再挂接，再写东西 -- flash 类似于 U盘
	
	(5) 格式化需要用到第三方工具 -- mtd-utils
		tar xjf mtd-utils
		cd utils
		修改Makefile -- CORSS_COMPILE = 
		cp flash_erase flash_eraseall  /bin/
		
		flash_eraseall /dev/mtd3		// flash_erase擦除一个扇区，操作字符设备mtd3
		擦除之后就是 yaffs文件系统
		mount -t yaffs /dev/mtdblock3 /mnt
		cd /mnt/
		
		
		// 格式化: flash_eraseall -j /dev/mtd1	-- 格式化使用字符设备
		// mount -t jffs2 /dev/mtdblock1 /mnt	-- mount使用块设备
		
		// 流程：格式化 挂接 创建文件 读写文件
#endif

#if /* 26 - NORFLASH驱动框架 */
	
	(1) norFlash可以像内存一样访问
		两个板子之间内存差别：地址(基地址)、位宽
		
	(2) 对于mtd层，不关心是nandflash或者是norflash，只关心提供的mtd_info结构体
		.erase .read实现不同
		
	(3) 接口和内存一样，有地址线 数据线，可以像内存一样读 但是不能像内存一样写
	    norflash写要发出特定命令 -- 某个地址写入特定的值，称之为命令
		
		错位 or 一个地址对应2字节 data16bit	
#endif

#if /* 29 - 虚拟网卡驱动程序 */
	
	// 字符设备 块设备都有设备节点
	// open /dev/xxx
	// mount /dev/xxx
	
	(1) 收发功能：将上层提供的数据发出；收到数据组包提交到上层 -- 收发能力
	
	(2) 上层将需要发送的信息封装在sk_buff结构体中，调用hard_start_xmit()
	    底层将收到的数据封装到sk_buff，调用netif_rx()
	
	/* - 分配net_device结构体 */
	
	/* - 设置 */
	/* - 提供发包函数 -- hard_start_xmit() */
	
	/* - 提供收包的功能 -- 中断中调用netif_rx()上报数据 */
	
	/* - 硬件相关操作 */
	
	/* - 注册 */
	register_netdev()
	
	
	// 找不到 struct net_device_stats {}
	cd kernel/include/
	grep "net_device_stats" * -nwR
#endif

#if /* 31 - dm9000驱动 */

	// 修改基地址、位宽、中断引脚等  --  两块板子之间移植，找出相异性，修改
	
	// memory controller接口负责发信号到网卡芯片 -- cpu发出读写操作到mem接口，由mem接口发出信号
	// 地址信号，读写信号。时序肯定有要求 -- 如读写脉冲时序要求
	// 如何设置，才能发出满足时间要求的信号 -- uboot已经设置OK
	
	// 总线位宽 & 等待控制寄存器
	
	// 可以首先都设置最大值，-- 后面提高访问速度再优化

	(1) 网卡接口和内存接口一样，直接读某个内存地址即可	// dm9000接口和内存相似
	
	(2) io_base的确定
		dm9000片选引脚接nGS4
		当cpu发出的地址数据是0x20000000 - 0x28000000, nGS4自动使能

#endif

#if /* 37 - i2c驱动框架 */

	(1) 由裸机代码修改为驱动:
		硬件读写函数中的寄存器操作 修改为 ioremap
		while()等待操作 修改为 休眠 -- 中断中数据传输完成唤醒
		中断 修改为 irq_requeset()
		
	(2) 内核已经将i2c相关处理实现，我们只需要完成设备相关的操作
		类似于usb总线，总线那一套已经处理完i2c协议相关，只需要完成设备相关层
		总线知道如何收发数据，但是不知道数据的含义，我们编写知道数据的上一层
		
		// i2c驱动框架
		APP: open, read, write
		--------------------------------------
		驱 drv_open, drv_read, drv_write		// i2c设备驱动: 知道数据的含义
        动 -----------------------------------
		层 i2c总线驱动程序						// i2c总线驱动: 识别，提供读写函数(知道如何收发)	
		--------------------------------------
		硬件: AT24C02
		--------------------------------------
		
		i2c总线驱动程序对应裸板中的中断函数 -- 发出各种信号，怎么发出起始信号，怎么发出数据，怎么发出回应
		kernel自带的i2c总线驱动程序 -- i2c-s3c2410.c
		
	(3) adapter.algo = xxx
	
		设置中断，启动传输(配置寄存器)
		一旦启动传输，就会产生一系列中断
	
	(4) 没有找到algo.smbus_xfer赋值，首先判断是否在其他地方有默认值设置
	
	(5) i2c_transfer()		// adap_algo->master_xfer();
		
		// 总线是 i2c_bus_type

	// -------------------------------------------------------------------
	static uint32_t force_addr[] = {ANY_I2C_BUS, 0x60, I2C_CLIENT_END};
	uint32_t **i2c_forces = &force_addr;
	
	static uint32_t force_addr[] = {};
	static uint32_t *forces[] = {force_addr, NULL};
	uint32_t **i2c_forces = forces;
	
	
	// -------------------------------------------------------------------
	对于写i2c设备驱动，核心是构造 struct i2c_driver{.attach_adapter = }
	
	注册driver时，遍历adapter链表，通过各个adapt访问driver中的attach_adapter()函数，从而调用i2c_probe()函数
	
	i2c_probe(adapter, addr, func)使用i2c总线的相关函数，发出地址信号，判断是否有该设备存在，如果存在则执行func()
	
	func() 分配设置i2c_client结构体 -- i2c_attach_client(at24cxx_client);
	
	裸机直接配置寄存器发数据，现在使用i2c总线驱动程序提供的收发函数
	-- i2c_transfer(at24cxx_client->adapter, )		// 调用适配器 -- i2c总线
	 |
	 | -- adap->algo->master_xfer()	// 适配器 算法 里面的 函数 发起传输
	    |
		| -- s3c4xx_i2c_xfer()	// i2c-s3c2410.c
		   |
		   | -- s3c24xx_i2c_doxfer()	// 配置寄存器，发起传输，使能中断
	
#endif

#if /* 74 - 3.4.2内核下i2c驱动 */

	(1) i2c读设备流程
		start + dev_addr + w + ACK		// 写
		data_addr + ACK
		start + dev_addr + r + ACK 		// 再发读
		data + ACK
	
	(2) 分层
		------------------------------------------
		APP：open、read、write
		------------------------------------------
		at24c08_open、at24c08_read					// 设备驱动层
		------------------------------------------
		总线设备驱动程序提供 统一的 i2c操作函数		// 核心层 -- 一个i2c控制器下可以接多个i2c设备，这些设备都需要发出start信号、地址信号、ack信号 -- 共性
		------------------------------------------
		各类soc i2c控制器的硬件操作					// 适配器
		------------------------------------------
		
		//  如何将三层结合到一起 -- 总线 - 设备 - 驱动模型  -- 方便管理设备驱动中比较稳定的代码
			内核中构造了很多种总线，包括虚拟的
			.match
			设备链表
			驱动链表
		
		总线提供match()，判断左右两边dev、driver是否匹配
		
		i2c_bus_type
		.match
		i2c_driver链表
		i2c_client链表
#endif

#if /* 73 - 3.4.2内核i2c编写代码 */

// 构造i2c_client
方法1：
	static struct i2c_board_info xxx_i2c_devs[] =
	{
		{
		I2C_BOARD_INFO("24c08", 0x50),		// name addr
		.platform_data = &at24c08,
		},
	}
	
	-- i2c_register_board_info(0, mini2440_i2c_devs, ARRAY_SIZE(mini2440_i2c_devs));	// 0 -- 适配器0
	 |
	 | -- list_add_tail(&devinfo->list, &__i2c_board_list); // 放入链表，链表什么时候使用?

	-- i2c_scan_static_board_info()		// 使用链表
	 |
	 | -- 对链表中每一个成员，执行i2c_new_device() 	// 通过i2c_board_info信息，构造client，放入左边链表  -- 右边是i2c_add_driver()
	 
	 // 这种方法不适合，编写可动态加载的程序 ???
	 
方法2：
	-- i2c_new_device(struct i2c_adapter *adap, struct i2c_board_info const *info)
	
	SMBUS是i2c的一小部分	// smbus is a subset from the i2c protocol
	
方法3：
	// static DEVICE_ATTR(new_device, ...)		-- i2c-core.c
	
	echo wm8976abc 0x33 > /sys/class/i2c-adapter/i2c-2/new_device
	echo 0x34 > /sys/class/i2c-adapter/i2c-2/delete_device

方法4：
	// 以上3种方法 都需要指定 adapter -- i2c控制器
	
	// request_irq() 如果是共享中断最后一个参数不能省略
	
	
	cat  /proc/interrupt  //  -- 判断确实发生了中断

#endif


#if /* 字符设备概念纠正之另一种写法 */

	2.4内核 字符设备驱动由MAJOR唯一标识，最多只能有255个
	register_chrdev() // (主, 0) ~ (主, 255)都对应一个xxx_fops;
	
	register_chrdev_region() // 主设备号 + 次设备号 共同标识一个设备
	
	// D -> 不可中断的等待
#endif

#if /* 字符设备概念纠正之RTC驱动分析 */

	date命令		// not data
	
	date无法查看时间：
	首先查看内核中是否有rtc相关驱动源码，为何没有执行？
	看驱动源码首先从入口开始

#endif

#if /* 58 - printk原理 */
	(1) printk() 需要调用硬件操作函数 串口 或 lcd		// console = ttySAC0 or console = tty1
	
	(2) __setup("console=", console_setup)
			-- console_setup()
			 |
			 | -- add_preferred_console(name, idx, options)	// 记录准备使用"ttySAC0"控制台
			 
			// 如何找到对应的硬件操作函数  -- name
			a> 串口		// struct xxx{.name = xxx; .fun = xxx;}
			b> lcd

	(3) register_console()		// driver/serial/s3c2410.c

	(4) -- printk()
		 |
		 | -- vprintk()
		    |
			| -- vscnprintf()	/* Emit the output into the temporary buffer */
			|
			| -- con->write(con, &LOG_BUF(start), end - start);		// 调用硬件写函数
		
	
	// 一部分输出到串口，一部分存在缓冲区
	比如printk("abc")会得到"<4>abc", 再写入log_buf。dmesg打印LOG_BUF数组中数据
	
	dmesg命令肯定也是打开某个文件 --  ls -l /proc/kmsg
	/proc -- 虚拟文件系统 -- 启动系统时 mount -a -- all指定是 /etc/fstab 中定义的所有文件系统
	cat /proc/kmsg		-- 再次打开，文件索引到末尾  -- cat对应内核.read，对应copy_to_user()
	
	cat /prco/mymsg		-- 出来很多数据
						-- cat系统调用  -- open xxx文件，while { read()如果有返回 } -- 一直读
	
	如果没有装载mymsg驱动 	// 没有提供myprintk实现
	先装载了first_drv.ko	// unknow symbol myprintk
	
	
	echo "8 4 1 7" /proc/sys/kernel/printk
	
	set bootargs "loglevel=0"
	
#endif 

#if /* 61 - oops信息 */

	vi System.map			// 确定内核地址空间

	cat /proc/kallsyms > /kallsyms.txt		// 打印太慢了
	
	
	cat /proc/interrupts		30:   xxx(中断次数，不断变化)  s3c S3C2410 Timer Tick
	
	static struct irqaction s3c2410_timer_irq =		/* 中断都有该结构体 */
	{
		.name    = "S3C2410 Timer Tick",
		.flags   = 
		.handler =
	}
	
	makefile -- 搜索 prefix
	make install prefix-$pwd/tmp	// $PWD/tmp
	sudo chmod +x /bin/arm-linux-gdb
	
	编译gdb_server报错 -- PTRACE_GETSIGINFO未定义
	echo $PATH -- 交叉编译工具链中找 -- 在工具链根目录下搜索该宏 grep
#endif



