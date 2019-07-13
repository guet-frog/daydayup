//5.2.4 18′
 -- /proc/sys/printk
 -- dmesg

 -- 编译模块的Makefile讲解

//5.2.5 1′
 -- uname -r
 -- /lib/modules/3.13.0-32-generic/bulid

//5.2.5 33′
 -- cat /proc/sys/kernel/printk
 -- #define KERNEL_INFO		6
 -- printk()打印级别在cmd中的效果

//5.2.6 1′
 -- 驱动使用模块这种机制来实现

//5.2.7 13′
 -- inline的两个作用		// 防止头文件中定义函数 链接时重复定义
							// 提高效率

 -- cat /proc/devices		// 读内存
 -- 主设备号(major)既是设备编号 也是设备管理表的下标

//5.2.10 1′
/dev/ *		// 每一个文件代表一个设备

//  6′
如何通过设备文件找到驱动			// -- file_operation[255] kernel驱动集合
// 主设备号 -- 文件类型属性
			-- 主设备号 + 次设备号

vi /dev/test 		ls -l /dev/test

//  5.2.11 17′
数据在应用层和驱动层的传递
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos)

write(fd, "helloworld", 100)

// file -- 驱动框架中用到
// ubuf -- 用户数据到内核数据的 复制

write(struct file *file, ...)		// file驱动框架中用到

2.
设备号的 一个作用 用来创建设备文件

5.3.5 3′
函数封装 -- 分层
inline  -- 效率 

5.3.7 1′
app调用驱动方法的整个流程

5.3.11
结构体变量首元素首地址 与 该结构体变量的地址的关系

5.3.12
io与内存统一编址		// x86不是 *(volatile unsigned int *)0x10000000

5.4.4 4′
susbsys_initcall(leds_init) 不用调用insmod
leds模块不能设置为M

7′
sysfs是 驱动 提供给 应用层 的一个接口
leds_class->dev_attrs = led_class_attrs

_ATTR()宏生成sys/class/xxx文件	//show() -- read();	store() -- write

5.4.5 1′
/sys/devices	//  当前系统注册的所有设备。有些设备不是通过class的方式，class文件夹下没有 

17′
make menuconfig的目录层次和源码目录层次相似

5.4.11 25′
数组传参 需要带上size  函数内部不能通过sizeof(数组名)获取长度

5.4.12 26′
tmp = __raw_readl(base + GPIOCON_OFF);
tmp &= ~(0xf << con_4bit_shift(offset));
tmp |= 0x1 << con_4bit_shift(offset);

io记录休眠前的状态

5.4.13 20′

由s3c_gpiolib_add() 到 gpiochip_add()

drivers/gpio/gpiolib.c   line1089
gpio_desc[id].chip = chip
将gpio_chip结构体的首地址，挂接在内核内部gpio_desc[]结构体数组中chip元素中

类似于内核用 数组 来管理字符设备驱动

驱动框架：关于硬件的相关操作并没有实现，只是留出接口(通过函数指针)

5.4.15  2′
应用层在sysfs中可以通过attribute获取到相关信息
attribute实现的相关代码

//  ----------------------------------------------------------------------
5.5.1 9′
总线负责将 设备和驱动 挂接起来

13′
kobject 基类相关概念
对象生命周期的自我管理机制 自我free

5.5.2 1′
从面向对象的角度 其他类继承了基类
从c语言角度	其他结构体 包含 了该结构体

绑定和包含的差异

5.5.3 9′
操作系统管理总线
总线再管理驱动和设备

// 总线 设备 驱动 三个概念
// 分析总线就是为了分析总线式如何管理下面的设备和驱动

总线分为两个分支：设备分支、驱动分支	// match() 设备和驱动匹配

struct bus_type
{
	struct device_attribute *dev_attrs;
	struct driver_attribut *drv_attrs;
	
	int (*match)(struct device *dev, struct device_driver *drv);
}

22′
device_register()注册设备相关信息到内核。如: 使用哪些gpio 使用哪一个中断号等其他一些系统资源

父类是子类的高度抽象

27′
probe()函数用于总线式驱动 -- 相当于通过insmod()所执行的函数来装载驱动。
如何判断某一个设备是否能使用该驱动，主要通过该驱动的probe函数
probe函数是整个驱动的入口

使用udev(热插拔功能)自动生成节点文件

// /sys/bus		-- 内核中肯定有一个bus子系统

5.5.6 2′
编写驱动包含两个部分：
驱动功能编写 -- 硬件相关操作
驱动加载 -- 框架相关操作(驱动的入口)

5.6.1 5′
make menuconfig中搜索相关关键字buzzer

misc设备都是字符设备 
当成字符设备注册到内核中，主设备号10固定 次设备号用来区分各个杂散类设备

9′
为什么不直接使用字符设备，而使用misc
udev需要class，因此设备必须 从属于一个类，以前实验是自己创建了一个类

dirname $0		获取当前路径

find -name "*busybox*"