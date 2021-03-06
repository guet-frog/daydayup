*******		   《项目驱动创新学习训练营》			  *******
*******  A0201-海思HI3518E方案视频编解码传输深度学习  *******
*******        第8季：海思平台OSD的实现	  			  *******

--------------------------------------------------------
本课程由朱老师物联网大讲堂推出并提供技术支持，更多技术问
题可以联系朱有鹏老师获取支持。版权所有，翻录必究
--------------------------------------------------------
第一部分、章节目录
8.1.海思平台OSD理论学习1
8.2.海思平台OSD理论学习2
8.3.OSD实验演示和代码框架分析
8.4.OSD代码实现分析1
8.5.OSD代码实现分析2-前景和背景透明
8.6.OSD代码实现分析3-RGN内容填充
8.7.OSD代码实现分析4-RGN内容填充
8.8.OSD代码实现分析5-动态刷新BMP
8.9.字库字符实现OSD
8.10.字库字符实现OSD源码解析1
8.11.字库字符实现OSD源码解析2
8.12.字库字符实现OSD源码解析3
8.13.字库字符实现OSD源码解析4


第二部分、章节介绍
8.1.海思平台OSD理论学习1
	本节讲解OSD相关的理论，主要以海思MPP手册为准。
8.2.海思平台OSD理论学习2
	本节接着讲解OSD的相关理论。
8.3.OSD实验演示和代码框架分析
	本节演示OSD部分实验，并且对demo的源码架构进行整体分析。
8.4.OSD代码实现分析1
	本节分析OSD的代码部分，主要是RGN的建立模块。
8.5.OSD代码实现分析2-前景和背景透明
	本节继续分析OSD源码部分，主要是前景和背景透明等模块。
8.6.OSD代码实现分析3-RGN内容填充
	本节接着分析OSD源码部分，主要是rgn的内容填充部分。
8.7.OSD代码实现分析4-RGN内容填充
	本节接着分析OSD源码部分，主要是rgn的位图填充部分。
8.8.OSD代码实现分析5-动态刷新BMP
	本节分析OSD的代码部分，主要是BMP图片的动态刷新功能的实现。
8.9.字库字符实现OSD
	本节开始讲解用字库方式实现OSD显示的代码模块。
8.10.字库字符实现OSD源码解析1
	本节开始讲解字库方式实现OSD的源码分析，主要是OSD信息解析部分的分析。
8.11.字库字符实现OSD源码解析2
	本节接着讲解字库方式实现OSD的源码分析，主要是time和title的刷新显示部分。
8.12.字库字符实现OSD源码解析3
	本节接着讲解字库方式实现OSD的源码分析，主要是字库生成BMP图片的部分。
8.13.字库字符实现OSD源码解析4
	本节接着讲解字库方式实现OSD的源码分析，主要是上层代码框架的实现和细节。
	

第三部分、随堂记录
8.1.海思平台OSD理论学习1
8.1.1、OSD概述
8.1.2、海思OSD的4种类型
8.1.3、4种OSD类型各自支持的模块和功能
8.1.4、海思OSD的几个重要概念
(1)区域层次
(2)位图填充
(3)区域公共属性
(4)通道显示属性
(5)区域反色
(6)区域QP保护

// OVERLAY只能绑定VENC模块 -- 区域叠加是在编码环节 -- VI采集、VPASS处理不能叠加
// 设备号 0 -- 3518EV200只有一个VENC模块，一个device多个channel		-- 3518E VENC只有一个channel

// 编码之前没有OSD，编码之后的码流有OSD

// 画布canvas -- framebuff


8.2.海思平台OSD理论学习2
8.2.1、海思平台OSD使用方法
(1)用户填充区域属性并创建区域
(2)将该区域指定到具体通道中(如 VENC)
(3)通过 HI_MPI_RGN_GetAttr、 HI_MPI_RGN_SetAttr 获取和设置区域属性
(4)通过 HI_MPI_RGN_SetBitMap(仅针对 Overlay)设置区域的位图信息
(5)通过 HI_MPI_RGN_GetDisplayAttr 和 HI_MPI_RGN_SetDisplayAttr 获取和设置区
域在某通道（如 VENC 通道）的通道显示属性。
(6)最后用户可以将该区域从通道中撤出（非必须操作），再销毁区域
8.2.2、海思平台OSD的API和关键数据结构


8.3.OSD实验演示和代码框架分析
8.3.1、OSD实验演示
8.3.2、OSD代码框架分析


8.4.OSD代码实现分析1
(1)函数调用关系
	SAMPLE_RGN_CreateVideoRegion
(2)RGN画布尺寸计算：以像素为单位。原始图像是bpp24的，每个像素3字节。而画布的图像是ARGB1555的，所以每个像素是2字节。
	所以画布每一行的像素数是图像宽度*3/2	
	// region_width = 480;	// 320 * 3 / 2
	// region_hight = 320; 	// 320 
	
(3)整个图像的坐标系是左上角是(0,0)点，宽度方向是x，高度方向是y


8.5.OSD代码实现分析2-前景和背景透明
8.5.1、函数调用层次更新
	SAMPLE_RGN_CreateVideoRegion
		SAMPLE_RGN_CreateOverlayForVenc
			HI_MPI_RGN_Create
			HI_MPI_RGN_AttachToChn
		SAMPLE_RGN_Add
			HI_MPI_RGN_GetAttr
			HI_MPI_RGN_GetCanvasInfo
			SAMPLE_RGN_UpdateCanvas
				SAMPLE_RGN_CreateSurfaceByCanvas
					SAMPLE_RGN_LoadCanvasEx
						SAMPLE_RGN_LoadBMPCanvas_Logo
							GetBmpInfo
							malloc
							fseek
							fread
							OSD_MAKECOLOR_U16				
			HI_MPI_RGN_UpdateCanvas
			
		SAMPLE_RGN_CreateOverlayForVenc
		SAMPLE_RGN_AddVideoTimestamp
8.5.2、RGN通道属性分析
8.5.3、通过实验来帮助理解代码和概念
(1)前景透明度	// FgAlpha

(2)背景透明度	// BgAlpha	// 包括画布背景 和 图片背景 -- 背景透明度设置为0 全透明

(3)画布背景色	// BgColor 	0 -- 黑		0xffffffff -- 白		画布背景色(显存的默认填充色)与图片背景色相同
8.5.4、总结
(1)所谓前景foreground，就是图片中显示的内容部分；所谓背景background，就是图片中没有内容的部分。
(2)前景和背景的透明度范围都是0-128，其中0代表全透明，128代表全不透明
(3)前景和背景透明度可以同时设置，各自起作用，互不影响。
(4)stRgnAttr.unAttr.stOverlay.u32BgColor是RGN的画布（canvas，等同于LCD显示时的显存fb）的背景颜色。也就是画布中没有被填充的部分默认显示的颜色。


8.6.OSD代码实现分析3-RGN内容填充
BMP图片中存储图像的像素顺序，和RGN的canvas里像素顺序是不同的。


8.7.OSD代码实现分析4-RGN内容填充


8.8.OSD代码实现分析5-动态刷新BMP
总结：左下角的就是单bmp图片OSD，静态的；右下角是多BMP图片组合，且动态刷新显示。


8.9.字库字符实现OSD
8.9.1、代码框架分析
8.9.2、原理讲解
HH_OSD_Init			初始化这一套
	HH_OSD_SetOsdPosDefault
	HH_OSD_GetOrg
	HH_OSD_GetColor
	HH_OSD_GetTitle
	HI_Create_Osd
		HI_OSD_Build
			HI_OSD_Parse_OsdTitle
			HI_OSD_Get_BmpSize
			HI_Create_Osd_Reg
				HI_MPI_RGN_Create
				HI_MPI_RGN_AttachToChn
			HI_OSD_Create_Bitmap
				HI_OSD_CreateBMP				// 用字库来生成bmp文件的
				HI_MPI_RGN_SetBitMap			// 真正设置osd显示的API
			HI_OSD_Set_Show
				HI_MPI_RGN_GetDisplayAttr
				HI_MPI_RGN_SetDisplayAttr
HH_OSD_All_Refresh
	HH_OSD_AllTime_Refresh
		HH_OSD_ChnTime_Refresh
			HH_OSD_ChName_Refresh
				HH_OSD_GetLogoHandle
				HH_OSD_GetTitle
				HI_OSD_Parse_OsdTitle
				HI_OSD_Get_BmpSize
				HI_OSD_Create_Bitmap
					HI_OSD_CreateBMP
					HI_MPI_RGN_SetBitMap
				HH_OSD_Show_Refresh
					HH_OSD_GetLogoHandle
					HH_OSD_GetShow
					HI_OSD_Set_Show
	HH_OSD_AllName_Refresh
总结：字库字符实现OSD的本质原理是：先由字库生成对应文字内容的bmp文件，然后将bmp文件显示在osd中。


8.10.字库字符实现OSD源码解析1


8.11.字库字符实现OSD源码解析2


8.12.字库字符实现OSD源码解析3


8.13.字库字符实现OSD源码解析4








