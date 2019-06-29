
// 2.1
(1) 测试printk()打印级别设置，在控制终端中打印信息
(2) 最简单的linux模块编写，通过insmod、lsmod、rmmod测试
(3) 编译模块需要内核源码树，需要用到kernel的Makefile，以及相关Makefile分析

// 2.2
(1) 模块安装时，添加设备注册函数register_chrdev()
(2) app操作设备文件的read() write()，映射到file_operation结构体read() write() -- 设备号
(3) app层和kernel的信息交互copy_to_user()、copy_from_user()
(4) app.c read() write()函数测试
(5) mknod创建设备文件

// 2.3
(1) fops-write()中添加led写操作
(2) led采用静态映射的方法 -- 裸机操控硬件和OS下操控硬件的异同

// 2.4
(1) 注册字符设备新接口测试alloc_chrdev_region()
(2) 自动创建设备文件

// 2.5
(1) 动态映射io