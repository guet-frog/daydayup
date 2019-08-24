
// 2.1
(1) 测试printk()打印级别设置，在控制终端中打印信息
(2) 最简单的linux模块编写，通过insmod、lsmod、rmmod测试
(3) 编译模块，需要内核源码树，需要用到kernel的Makefile，以及相关Makefile分析

// 2.2
(1) 模块安装时，添加设备注册函数register_chrdev()
(2) app操作设备文件的read() write()，映射到file_operation结构体read() write() -- 设备号
(3) app层和kernel的信息交互copy_to_user()、copy_from_user()
(4) app.c read() write()函数测试
(5) mknod创建设备文件

// ---------------------------------------------
	填充file_operation结构体，read、write实例化		// copy_to_user()
	实例化并填充cdev			// struct cdev *cdev = cdev_alloc()
	主次设备号 和 file_operation 绑定	// cdev_add()
	app访问 该主次设备号 相对应的设备文件
// ---------------------------------------------

// 2.3
(1) fops-write()中添加led写操作
(2) led采用静态映射的方法 -- 裸机操控硬件和OS下操控硬件的异同

// 2.4
(1) 注册字符设备新接口测试alloc_chrdev_region()
(2) 自动创建设备文件

// ------------------------------------------------
	class_create()
	device_create()
// ------------------------------------------------


// 2.5
(1) 动态映射io

// 4.1
(1) 采用led驱动框架编写led驱动

// ------------------------------------------------
	配置kernel，添加drivers/leds/led_class.c文件	// 创建sys/class/leds
													// 通过leds_class->dev_attrs = led_class_attrs创建相关文件夹和文件(类似于设备文件)
													// 访问设备文件不需要通过主次设备号。通过attribute
	实例化并填充 struct led_classdev *led_cdev
	app直接访问指定目录下的指定文件
// ------------------------------------------------

// 4.2
(1) 采用writel()读写io
(2) 调用gpiolib框架相关函数

// 4.3
(1) 测试模块集成到kernel中自启动 -- 测试修改error label

// 4.4
(1) 修改为platform平台总线




