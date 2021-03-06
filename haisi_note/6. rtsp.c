
6.3.H264的NAL单元详解1
	本节讲解SODB、RBSP、NALU等h264中重要概念，同时讲解常用的h264流分析工具软件，最后从宏观上讲了H264流的一些理解
6.4.H264的NAL单元详解2
	本节重点讲解H264的sequence、sps、pps、sei、NALU等，其重点是NALU的位定义和解析方法，这些是我们后续处理h264的rtsp传输的关键点。
6.5.H264的NAL单元详解3
	本节重点讲解sps和pps、sei的内部帧结构，以及h264的profile和level等重要概念。
6.6.H264的NAL单元详解4
	本节对h264的NAL部分进行总结，彻底打通前述概念，让大家对h264的整体码流内容和解析方法有本质的掌握。
6.7.rtsp传输源码分析1
	本节开始分析rtsp传输源码，主要是代码整体架构的分析。
6.8.rtsp传输源码分析2
	本节接着分析rtsp源码，主要是RtspServerListen相关的部分。
6.9.rtsp传输源码分析3
	本节接着分析rtsp源码，主要是主程序部分，以及和rtsp部分
6.10.rtsp传输源码分析4
	本节接着分析rtsp源码，主要是client和server之间的命令和响应，以及rtsp通信建立的过程，关键字的parser等。
6.11.rtsp传输实战分析
	本节从实战角度分析rtsp传输代码，用实验验证的方式帮助我们进一步理解rtsp传输代码的工作流程。
6.12.直接发送与环状buffer发送
	本节讲述2种rtp包发送策略，直接发送和环状buffer发送，这2种方式我们提供的源码中都做了演示。
6.13.rtsp分包发送h264的源码分析	
	本节讲解h264分包的rtp发送部分代码，核心是分包后用fu_indicator和fu_header替代nalu的部分，以及相关的一些标志位等。



6.2.H264编码原理和基本概念
6.2.1、h.264编码原理
	(1) 图像冗余信息：空间冗余、时间冗余
	(2) 视频编码关键点：压缩比、算法复杂度、还原度
	(3) H.264的2大组成部分：VCL和NAL
	
6.2.2、h.264编码相关的一些概念
	(1) 宏块 MB  macroblock
	(2) 片 slice
	(3) 帧 frame
	(4) I帧、B帧、P帧
	(5) 帧率 fps
	(6) 像素->宏块->片->帧->序列->码流

6.3.H264的NAL单元详解1
6.3.1、VCL和NAL的关系
	(1) VCL只关心编码部分，重点在于编码算法以及在特定硬件平台的实现，VCL输出的
		是编码后的纯视频流信息，没有任何冗余头信息
	(2) NAL关心的是VCL的输出纯视频流如何被表达和封包以利于网络传输
	(3) SODB：String Of Data Bits 
	(4) RBSP：Raw Byte Sequence Payload		
	(5) NALU：Network Abstraction Layer Units
	(6) 关系：
			SODB  + RBSP trailing bits    = RBSP
			NAL header(1 byte) + RBSP     = NALU
	(7) 总结：做编码器的人关心的是VCL部分；做视频传输和解码播放的人关心的是NAL部分

6.3.2、H.264视频流分析工具
	(1) 雷神作品：SpecialVH264.exe
	(2) 国外工具：Elecard StreamEye Tools
	(3) 二进制工具：winhex
	(4) 网络抓包工具：wireshark
	(5) 播放器：vlc

6.3.3、h264视频流总体分析
	(1) h264标准有多个版本，可能会有差异，具体差异不详
	(2) 网上看的资料有时讲法会有冲突，或者无法验证的差异
	(3) 我们的课程都是以海思平台为主、为准、为案例，不能保证其他平台也完全一样
	(4) 海思平台编码出来的H.264码流都是一个序列包含：1sps+1pps+1sei+1I帧+若干p帧

6.4.H264的NAL单元详解2
6.4.1、相关概念
	(1) 序列 sequence
	(2) 分隔符
	(3) sps
	(4) pps
	(5) sei
	(6) NALU

6.4.2、NALU详解
	https://blog.csdn.net/jefry_xdz/article/details/8461343

6.5_6.H264的NAL单元详解3_4
6.5.1、sps和pps详解
	https://www.cnblogs.com/wainiwann/p/7477794.html

6.5.2、H264的profile和level
	https://blog.csdn.net/xiaojun111111/article/details/52090185

6.5.3、sequence
	(1) 一段h.264的码流其实就是多个sequence组成的
	(2) 每个sequence均有固定结构：1sps+1pps+1sei+1I帧+若干p帧
	(3) sps和pps和sei描述该sequence的图像信息，这些信息有利于网络传输或解码
	(4) I帧是关键，丢了I帧整个sequence就废了，每个sequence有且只有1个I帧
	(5) p帧的个数等于fps-1
	(6) I帧越大则P帧可以越小，反之I帧越小则P帧会越大
	(7) I帧的大小取决于图像本身内容，和压缩算法的空间压缩部分
	(8) P帧的大小取决于图像变化的剧烈程度
	(9) CBR和VBR下P帧的大小策略会不同，CBR时P帧大小基本恒定，VBR时变化会比较剧烈

6.7.rtsp传输源码分析1
	// 服务器init -- 创建一个线程用来listen
	// 客户端connect -- 服务器accept  --  创建新线程来服务新连接
	
	// venc是在内部开启一个线程来进行编码
	// 编码完成放入VB的buff中				-- VB类似于一个队列
	// step6 -- SAMPLE_COMM_VENC_StartGetStream() -- 创建线程，取buff
	
	// line -- 1409
	// /* Set Venc Fd */
	// VencFd[i] = HI_MPI_VENC_GetFd(i)		// 获取内部编码线程 fd

6.8.rtsp传输源码分析2

6.9.rtsp传输源码分析3

6.10.rtsp传输源码分析4
	// 与ORTP实验不同 --   没有提供sdp信息，直接是裸流
	// sample提供了sdp信息

6.11.rtsp传输实战分析

6.12.直接发送与环状buffer发送

6.13.rtsp分包发送h264的源码分析

rtp_timestamp：
	https://blog.csdn.net/jasonhwang/article/details/7316128

fu_indicator：
	https://www.cnblogs.com/yjg2014/p/6144977.html


函数调用框架：
main
	RtspServer_init
		RtspServerListen
			RtspClientMsg
				ParseRequestString
		vdRTPSendThread
			VENC_Sent
	SAMPLE_VENC_720P_CLASSIC
		SAMPLE_COMM_VENC_Sentjin




ENC_Sent


RTSP_Session_Info


