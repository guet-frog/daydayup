
5.8.1.3、input子系统简介
	(2) input子系统分4个部分：应用层 + input event + input core + 硬件驱动

5.8.2.1、确定设备文件名
	(1) 应用层操作驱动有两种方法：/dev目录下的设备文件，/sys目录下的属性文件	// /dev/input/eventn

5.8.2.2、标准接口打开并读取文件
	(3) struct input_event				// #include <linux/input.h>

5.8.4.1、input子系统分为三层
	(1) 上层：	事件驱动层		// evdev.c、mousedev.c、joydev.c
	(2) 中间层：核心层			// input.c
	(3) 下层：	设备驱动层		// drivers/input/xxx

5.8.4.2、input类设备驱动开发方法
	(1) 事件驱动层、核心层不需要编写、
	(2) 调用核心层定义的接口，完成设备驱动层

5.8.6.输入核心层源码分析1
5.8.6.1、核心模块注册input_init()
	(1) class_register()			// 其他文件中实现device_register()
	(2) input_proc_init()
	(3) register_chrdev()

	// class_create()	class_register()

5.8.6.2、核心层为设备驱动层提供的接口函数	// 核心层为事件驱动层提供的接口函数
	(1) input_allocate_device()
	(2) input_set_capability()
	(3) input_register_device(struct input_dev *dev)

	// register -- 定义硬件相关结构体变量并填充，调用相应接口函数(框架实现)注册

5.8.7.输入核心层源码分析2
5.8.7.1、handler和device的匹配
 -- input_for_each_entry(handler, &input_handler_list, node)	// 遍历、绑定
  |
  | -- input_attach_handler(dev, handler);
	 |
	 | -- input_match_device()		// 匹配device和handler
	 |
	 | -- handler->connect(handler, dev, id)	// 匹配上后，挂接device和handler

//  handler属于最上层(输入事件驱动层)
//  handler作用：为应用层提供接口(/dev/input/eventX)访问驱动层

//  内核用链表来管理handler，遍历handler与device(usb、serial)匹配	\
	注册一个新设备时，新设备与4类handler进行匹配					\
	绑定后在应用层生成相应的设备文件，为应用层提供访问接口			\
	绑定后设备产生输入事件后，上报到相应handler						\
	handler将数据按照一定格式放入缓冲区，并唤醒read阻塞的app

// Handlers: Keyboard Handlers、Mouse Handlers、Joystick Handler、Event Handler(多数)

5.8.7.2、核心层为事件驱动层提供的接口函数
	(1) input_register_handler(&mousedev_handler)		// mousedev.c
	(2) input_register_handler(&evdev_handler)			// evdev.c

	static const struct input_device_id evdev_ids[] = 
	{
		{ .driver_info = 1, },		// match all devices
	}

5.8.8.事件驱动层源码分析
	(1) struct input_handler
		{
			void *private;			// for special data
			void (*event)(...);		// 处理底层上报的数据，生成struct input_event结构体变量。app使用
			void (*match)(...);		// 没有使用，使用核心层提供的match()
			int (*connect)(...);	// 当handler和device匹配后，调用connect
			
			const struct file_operations *fops;		// 映射app层的fops
		}

	(2) -- evdev_read()
		 |
		 | -- if (count < input_event_size())	// read(fd, &buf, sizeof(struct input_event));
		 |
		 | -- retval = wait_event_interruptible();		// block
		 |
		 | -- input_event_to_user();		// copy_to_user()

	(3) -- evdev_connect()
		 |
		 | -- device_initialize(&evdev->dev)
		 |
		 | -- device_add(&evdev->dev)		// device_register() -- /dev/input/eventxxx

	(3) -- evdev_event()
		 |
		 | -- struct input_event event;
		 |
		 | -- event.xxx = xxx; 	// event填充
		 |
		 | -- evdev_pass_event()
		    |
			| -- client->buffer[client->head++] = *event	// event放入缓冲区
			|
			| -- kill_fasync(&client->fasync, SIGIO, POLL_IN)	// 通知应用层

5.8.9.1、先找到bsp中按键驱动源码
	(1) cat /dev/input/event0		// 按下按键，判断是否有反应，判断驱动是否存在
	(2) /driver/input/keyboard		// 由Makefile判断哪些文件被编译，或直接去源码树下检查
	(3) cat /sys/input/eventx/name 	// 通过设备name属性文件，在源码中搜索相关文件

5.8.9.3、源码细节实现分析
	(1) gpio_request()
	(2) input_allocate_device()
	(3) input_register_device()
	(4) timer()

5.8.11.1、模板	// input_programming.txt

5.8.12.中断方式按键驱动实战2
// mach-s5pv210/include/mach/irqs.h		-- OS对中断的统一编号
// kernel/include/linux/interrupt.h
// proc/interrupts

// 应用层没有syc打印 -- 驱动中没有set_bit(EV_SYNC, input->evbit)

// timer.expires = jiffies + (HZ/100)
// 时间点 和 时间段
// jiffies 当前时间点
