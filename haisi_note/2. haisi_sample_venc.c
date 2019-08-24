
2.2、sample代码学习的关键
	(1) 基础概念，譬如图像采集原理、通道、绑定
	(2) 整个视频采集、内部传递、处理、编码输出、网络传输过程

2.2_3.图像像素格式深度理解1_2
	(2) 颜色的三个关键：亮度、色度、饱和度

2.2.2、rawRGB和图像采集过程
	(1) 图像采集的过程：光照在成像物体被反射->镜头汇聚->Sensor光电转换->ADC为rawRGB
	(2) sensor上每个像素只采集特定颜色的光的强度，因此sensor每个像素只能为R或G或B
	(3) rawRGB和RGB都是用来描述图像的，图像采集时RGB是由rawRGB计算而来的
	(4) 视频观看效果: 分辨率、pitch、观看距离、fp

2.4.1、RGB方式表示颜色
	(1) RGB有RGB565和RGB888，ARGB等多种子分类
	(2) RGB的本质：将色度分解为R、G、B三部分，然后记录下亮度数据
	(3) RGB的优势：方便数字化表达，广泛用于数字化彩色显示器，计算机编程等领域。
	(4) RGB的劣势：和传统的灰度图兼容不好，表达颜色的效率不高

2.4.2、YUV
	(1) YUV是一种色彩空间，Y表示亮度，U和V表示色度。只有Y就是黑白图像，再加
		上UV就是彩色图像了。YUV的一个好处就是让彩色系统和传统黑白系统很好的兼容。
	(2) YUV和RGB的相同点是：都是用来表达颜色的数学方法；不同点是：对颜色的描
		述思路和方法不同。RGB将一个颜色拆解为3个纯色的亮度组合，YUV将一个颜色
		分解为一个亮度和2个色度的组合。
	(3) RGB和YUV之间可以用数学方法互相换算，是个典型的浮点运算过程		// 考虑cpu计算效率
	(4) YUV和YCbCr几乎可以看做一个概念，详细的区分以后再去慢慢体会。
	(5) YUV分为packed和planar两种。具体参考：
		http://blog.csdn.net/sunnylgz/article/details/7580628
	(6) 有多种YUV相关的概念需要弄清楚

	YUV
	YUYV
	YUV422					// 4个像素占8个字节
	YUV420(YUV411)			// 4个像素占6个字节
	YUV422 planar(YUV422P)
	YUV420 Planar(YUV420P)
	YUV422 semi planar(YUV422SP)
	YUV420 semi Planar(YUV420SP)
	参考：http://blog.csdn.net/bingqingsuimeng/article/details/50716390 https://www.2cto.com/kf/201303/198023.html

// RGB和YUV相互转化，重点是对cpu的计算量。 不考虑转化公式只考虑计算量

2.7.2、VB相关的数据结构和API
	(1) VB_CONF_S
	(2) HI_MPI_VB_SetConf()
	(3) HI_MPI_VB_Init()

	// p28 -- pool相关图解
	// 计算video buff size -- u32vbSize 留一定余量
	// 一路视频流对应一个缓存pool，不同分辨率block size不同

2.8.2、MPP系统初始化详解
	(1) mpp_init() 主要实现VB相关，其他soc提供封装

2.11_12.VI部分详解
	(1) sensor接口：MIPI、LVDS、DC
	(2) WDR宽动态	// 同一幅图、既有光线强也有光线弱。需要局部曝光不同 -- 光照强的地方 拍照 曝光要短
					// 需要硬件sensor支持
	(3) isp就是image signal process，图像信号处理。	/* - isp硬件单元 被封装成一个线程 */
		// 如3A 自动曝光 自动对焦 自动白平衡
		// 也可以额外添加isp ic

	(4) HI3518E内部的ISP单元是隶属于VI模块的。VI模块就包含3大部分：
		<a> sensor接口相关
		<b> ISP
		<c> VI dev和channel

2.13.3、VPSS的Grp和Chn
	(1) VPSS的Grp
	(2) VPSS的Chn
	(3) VI的Chn（和Dev）

	// vpss对图像进行统一处理
	// 缩放 -- 720P VGA QVGA(VI -> VPASS(一进多出) -> VO)
	// 锐化 -- 图像变清晰
	// p369 -- vpass channel描述 -- 通道0不支持缩小
	// 接到扩展通道 二次缩小

	// 创建、开启group，将group与上一级VI的device的channel绑定
	// 绑定后，在VPASS级创建并打开几路channel，输出 分辨率不同 大小不同 未经编码的原始图像
	// 目前完成的功能：VI部分调用sensor获取图像，进过VI基本处理将图像传给VPASS group，VPASS处理过后通过channel输出

2.16.图像编码压缩基本原理
	参考：http://blog.csdn.net/newchenxf/article/details/51693753

2.17.MPP手册中图像编码部分解读
	参考：http://blog.csdn.net/u013354805/article/details/51988171

2.18.sample中venc模块源码解读

2.19.编码后的流文件输出和课程总结

	// 工具 -> 媒体信息		-- 分辨率、以解码格式
	// 裸流没有header

	// APTINA_AR0130 --	sensor可以支持到1080P，但是fps会降低

	// 摄像头130万 -- 摄像头最多能出多少个像素
	// 1280 * 720 = 921600		// 产生720P的至少需要100万的摄像头
	// 1920 * 1080 = 2073600

	// 图像进过镜头的汇聚，汇聚到一个焦点上 		-- sensor选取要有余量

