
// 2M(1920 * 1080)Pixel  3518E最大支持2000000像素

// 3518E 只有一个 VIU0		-- 获取sensor数据

// ov9712性能要低于ar0130
// sensor硬件上瑕疵 可以通过图像IQ调试弥补 -- 黑电平 曝光度 γ矫正参数

4.2.1、软件修改
	(1) ./load3518e -i -sensor ov9712 -osmem 32 -total 64
	(2) sample/mpp/Makefile.param

4.3.1、并口Sensor
	(1) OV9712和AR0130都是并口的			// 10bit 12bit
	(3) 并口传输的是CMOS电平信号，非差分信号，传输距离受限

4.3.2、LVDS
	(1) low voltage differential signal，低电压差分信号
	(2) 接口由1组差分clock和若干组差分信号线组成
	(3) LVDS主要用于视频传输的2个领域：camera和主控(视频采集)、LCD和主控(视频显示)
	(4) LVDS利用差分抗干扰能力，提升clock频率从而提升带宽，传输距离也更远
	(5) LVDS的数据线组数越多带宽越大、clock频率越高带宽越大（牺牲抗干扰和距离）
	(6) 并口和LVDS之间可以互转，但是需要专门的电平转换芯片（类似于232和485）

4.3.3、MIPI(MIPI-CSI2)
	(1) MIPI: mobile industry processor interface，移动工业处理器接口
	(2) MIPI接口由1组差分clock和1-4组差分信号线组成
	(4) MIPI的架构层次更分明，广泛应用在手机平板等领域中，可以认为MIPI是LVDS的升级版
	(5) MIPI的数据线组数越多带宽越大、clock频率越高带宽越大（牺牲抗干扰和距离）
	(6) MIPI和LVDS和并口之间均可以互相转换，但是需要专门的电平转换芯片

	// insert_ko()
	// insert_sns()

4.7.sensor驱动源码解析1
4.7.1、sensor驱动源码
	(2) component/isp/sensor/ar0130		// 编译 -- 自动更新lib下库文件
	(3) 底层i2c驱动在kernel中		// sensor为应用层驱动

	// 仅在应用层无法操作硬件
	// sensor驱动(应用层驱动) 基于 IIC驱动(内核底层驱动) 来操作硬件

4.7.2、sensor驱动的框架
	(1) mpp定义了一套sensor驱动的实现和封装
	(2) xxxx_cmos.c中定义回调和上层函数
	(3) xxxx_sensor_ctl.c中定义底层硬件相关的寄存器值配置函数
	(4) kernel中的I2C驱动提供i2c层面的物理层操作接口

4.8.sensor驱动源码解析2
4.8.1、xxxx_cmos.c中实现和注册回调
4.8.2、xxxx_sensor_ctl.c中配置sensor寄存器

	(1) s32Ret = HI_MPI_ISP_sensorRegCallBack(IspDev, AR0130_ID, &stIspRegister)	// IspDev

// HI3518E 中的 VI模块; VI模块 中的 ISP模块 -- isp(dsp单元)
// Sensor与VI模块中的isp对接，sensor中的相关操作(方法名)在isp框架中已经定义好

// mpp系统中已经定义好 所有sensor行为的接口
// sensor驱动来提供 sensor行为的具体方法

// 大部分sensor中自带isp模块

4.9.ISP_3A框架解读	// ISP_3A开发指南.pdf

	component/isp/firmware/algo		-- firmware基础算法

	以lib的方式，各个模块可以实现松耦合
	// 3a和sensor的关联 -- 如：自动曝光功能实现
	最终是通过控制sensor的曝光时间、曝光强度来实现自动曝光功能。最终的操作落在sensor的硬件操作
	但是自动曝光和sensor可以分开(松耦合)
	3a 算法中自动曝光相关，并不涉及硬件如何自动曝光。重点在于曝光的算法，
	如：根据当前环境光来计算曝光时间，这个算法和控制硬件曝光时间没有关联

	3a // 自动曝光算法
	sensor	// 自动曝光硬件控制

	// 松耦合后 设计自动白平衡的人 不关心所用的sensor

	// 如何实现差异化sensor适配

	// 3A是纯软件算法

	// 算法库更改，如果是.a 直接重新编译app。如果是.so 重新编译app，并部署.so到运行环境

	// PQ tool可以在pc端模拟图像IQ调试

4.10.sensor驱动编译实战
4.10.1、sensor的注册接口分析
4.10.2、黑电平
4.10.3、sensor驱动编译实战
	(1) 修改驱动源码，修改黑电平		// vi sensor/ar0130/ar0130_cmos.c -- HI_U32 cmos_get_isp_black_level()
	(3) 确认mpp中lib目录下的libsnsxxx.a/so已经被更新
	(4) 重新编译sample并运行查看效果

4.11.sensor驱动的寄存器操作
4.11.1、sensor的寄存器设置
	(1) sensor内部有若干寄存器，可以通过I2C接口来读写
	(2) 数据手册有对寄存器的基本说明

4.8.4、实战任务：修改sensor的flip和mirror寄存器查看效果
	(1) 查sensor数据手册的寄存器列表
	(2) 改sensor驱动代码
	(3) 重新编译isp
	(4) 确认sensor库已经更新到mpp中
	(5) 重新编译sample，运行测试
	
	// 修改黑电平
	
	// 0x3040 - picture flip -- 可以动态实时修改
	
	只修改链接库情况下 make

4.12.sensor驱动部分贯通总结
4.12.2、海思方案的sensor驱动相关体系关键点
	(1) 搞清楚sensor的本质：光电转换+AD+ISP+并口/MIPI/LVDS
	(2) ISP有多种实现：sensor内置、主SoC内置、外接专用ISP芯片
	(3) 3A 是为最终图像效果负责的，3A的实现有赖于镜头、sensor、isp等各部门协同工作
	(4) 海思的体系中把sensor和3A、ISP实现为：指针挂接注册的各自独立模块

4.12.3、扩展学习方向
	(1) sensor的各种参数
	(2) 其他几种sensor的驱动和对比实现
	(3) isp的firmware
	(4) 3A算法相关知识

