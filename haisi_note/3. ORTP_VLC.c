
3.1.2、视频网络传输的2种方式
	(1) 基于下载：http or ftp
	(2) 基于实时：RTP/RTSP/RTCP

3.1.3、ORTP的介绍		// 基于RTP、RTCP协议实现
	(1) openRTP，用C实现的一个RTP库	// C++、Java实现
	(2) 实质是一个视频服务器，工作时客户端和服务器实时传递视频数据
	(3) 一般认为RTP工作在传输层，但是其实RTP比TCP/UDP高一个层次

3.2.1、准备源码		// https://github.com/dmonakhov/ortp

3.2.2、源码修改
	(1) 增加H.264的payload支持
		在src/avprofile.c中357行添加：
		rtp_profile_set_payload(profile,96,&payload_type_h264);

3.2.3、配置和编译、安装
	(1) 进入ortp目录执行./autogen.sh
	(2) error：./autogen.sh: line 44: libtoolize: command not found
	    解决：sudo apt-get install libtool*		// not libtoolize
	(2) error：libtoolize:   error: Please install GNU M4, or 'export M4=/path/to/gnu/m4'.
	    解决：sudo apt-get install m4
	(3) error：Automake - aclocal: command not found 
	    解决：sudo apt-get install automake

	(4) 继续执行./configure --prefix=/tmp/ortp --host=arm-hisiv300-linux

	(5) make && make install

3.2.4、到/tmp/ortp目录下查看移植好的库和头文件

3.3.1、在官方SDK的sample中添加rtp传输代码
	(1) venc/sample_venc.c				// channel num
	(2) common/sample_common_venc.c		// handle video stream

3.3.2、重新编译sample
	(1) 添加ortp头文件		// warning: 函数声明, error: 类型定义
	(2) 修改venc/Makefile		// -lortp -L /home/aston/src/ortp/ortp_lib/lib

3.3.3、开发板中部署并运行测试
	(1) 部署libortp.so到开发板中/usr/lib目录下
	(2) 检查开发板中原有配置是否正确，譬如sensor是否对应实际
	(3) 在nfs中运行新的sample程序
	(4) vlc中打开配置好的sdp文件

//  makefile -- -lpthread -lm -lortp 指定库  -L /tmp/ortp/lib 指定库路径 	-I 头文件
3.4_5.ORTP库的源码分析1_2
3.4.1、ORTP库概览
	(1) 库本身没有main，只提供功能函数，都在src目录下
	(2) 库的使用给了案例，有main，在src/tests目录下
	(3) 相关数据结构和头文件在include/ortp目录下
	(4) ortp实现了rtp和rtcp协议，前者负责传输，后者负责控制和同步协调

3.4.2、ORTP库的使用案例
	(1) src/tests/rtpsend.c
	(2) ortp_init及av_profile_init
	(3) ortp_scheduler_init和ORTP调度器：一个任务中完成多个会话的发送和接收，
		类似于select
	(4) rtp_session_new和rtp的会话管理

3.4.3、rtp的session
	(1) rtp通过会话来管理数据发送和接收，会话的本质是一个结构体，管理很多信息
	(2) 创建会话用rtp_session_new
	(3) rtp发送用rtp_session_send_with_ts
	(4) 底层真正干活的还是socket接口那一套，参考rtpsession_inet.c

3.4.4、ORTP的一些小细节
	(1) port.c中对OS的常用机制（任务创建和销毁、进程管理和信号量等）进行了封
		装，便于将ortp移植到不同平台中
	(2) utils.c中实现了一个双向链表
	(3) str_util.c中实现了一个队列管理
	(4) rtpparse.c和rtcpparse.c文件实现了解析收到的rtp数据包的部分
	(5) jitterctl.c中实现了jitter buffer来防抖。jitter buffer技术是ip 音视频通信里
		相对比较高级的主题，jitter buffer模块好坏通常是衡量一个voip客户端/服务器好
		坏的技术点之一，尤其是在网络抖动比较严重，如3g, wifi环境，数据包的rtt值不均
		衡往往会导致语音卡顿，丢字等现象，jitter buffer 模块通过缓存一段数据包，把
		数据包重排，并均匀的送给播放端，一个好的jitter buffer实现通长是动态调整缓存
		大小的，在网络延迟大，抖动严重时会动态增加缓存大小，在网络恢复时动态减小缓
		存大小以减少端到端的播放延迟。

3.4.6_7.RTP发送实验源码分析1_2
3.4.6.1、ortp库相应API
3.4.6.2、发送函数的重点讲解
3.4.6.3、可能的拓展方向
	(1) 裁剪sample到最简化
	(2) 修改一些参数做实验（譬如每包字节数、IP地址、端口号等）

3.4.8.VLC的sdp文件解析和课程总结
3.4.8.1、SDP文件格式		// 以sdp文件配置 打开一个网络串流
3.4.8.2、第三季总结

 -- rtp_session_init()
 
 -- rtp_session_set_scheduling_mode()
 
 -- rtp_session_set_blocking_mode()
 
 -- rtp_session_signal_connect()
 
 -- rtp_session_set_local_addr()

 -- rtp_session_set_remote_addr()
 
 -- rtp_session_set_send_payload_type()
 
 -- rtp_session_send_with_ts()
 
 -- rtp_session_recv_with_ts()
 
 -- rtp_session_destroy()
