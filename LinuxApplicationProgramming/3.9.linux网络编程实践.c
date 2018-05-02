

3.9.2.TCP协议的学习1
3.9.2.1、关于TCP理解的重点
(1)TCP协议工作在传输层，对上服务socket接口，对下调用IP层
(2)TCP协议面向连接，通信前必须先3次握手建立连接关系后才能开始通信。
(3)TCP协议提供可靠传输，不怕丢包、乱序等。
3.9.2.2、TCP如何保证可靠传输
(1)TCP在传输有效信息前要求通信双方必须先握手，建立连接才能通信
(2)TCP的接收方收到数据包后会ack给发送方，若发送方未收到ack会丢包重传
(3)TCP的有效数据内容会附带校验，以防止内容在传递过程中损坏
(4)TCP会根据网络带宽来自动调节适配速率（滑动窗口技术）
(5)发送方会给各分割报文编号，接收方会校验编号，一旦顺序错误即会重传。


3.9.3.TCP协议的学习2
3.9.3.1、TCP的三次握手
(1)建立连接需要三次握手
(2)建立连接的条件：服务器listen时客户端主动发起connect
3.9.3.2、TCP的四次握手
(3)关闭连接需要四次握手
(4)服务器或者客户端都可以主动发起关闭
注：这些握手协议已经封装在TCP协议内部，socket编程接口平时不用管
3.9.3.3、基于TCP通信的服务模式
(1)具有公网IP地址的服务器（或者使用动态IP地址映射技术）
(2)服务器端socket、bind、listen后处于监听状态
(3)客户端socket后，直接connect去发起连接。
(4)服务器收到并同意客户端接入后会建立TCP连接，然后双方开始收发数据，收发时是双向的，而且双方均可发起
(5)双方均可发起关闭连接
3.9.3.4、常见的使用了TCP协议的网络应用
(1)http、ftp
(2)QQ服务器
(3)mail服务器


3.9.4.socket编程接口介绍
	3.9.4.1、建立连接
		(1)socket	//socket函数类似于open, 打开一个网络连接, 成功则返回一个网络文件描述符(int类型)
					//以后操作这个网络连接都通过这个网络文件描述符
		(2)bind 	//绑定local ip地址
		(3)listen
		(4)connect
		(5)accept	返回值是一个fd, 表示已经和前来连接的客户端之间建立了一个TCP连接
					通过这个连接(new fd)来和客户端进行/*读写操作*/
			
			//注意: socket返回的fd叫做监听fd, 是用来监听客户端的, 不能用来和任何客户端进行读写 	\
					accept返回的fd叫做连接fd, 用来和连接那端的客户端程序进行读写				\
					-- tcp udp的socket返回值代表的含义不同

	3.9.4.3、发送和接收
		(1)send和write
		(2)recv和read
		
	3.9.4.4、辅助性函数
		(1)inet_addr, inet_aton, inet_ntoa		// inet_addr
		(2)inet_ntop、inet_pton					// net to point   warning: return const char *

	3.9.4.5、表示IP地址相关数据结构
		(1)都定义在 netinet/in.h

		(2)struct sockaddr，这个结构体是网络编程接口中用来表示一个IP地址的，注意这个IP地址是不区分IPv4和IPv6的（或者说是兼容IPv4和IPv6的）

		(3)typedef uint32_t in_addr_t;		网络内部用来表示IP地址的类型

		(4)struct in_addr				// struct in_addr && struct in6_addr
		  {
			in_addr_t s_addr;			// 封装为了某一层次的一致性
		  };							// struct in_addr addr = 0;
										// invalid initialize 初始化结构体类似于数组初始化
		(5)struct sockaddr_in			// sockaddr_in  sockaddr_in6
		  {
			__SOCKADDR_COMMON (sin_);
			in_port_t sin_port;
			struct in_addr sin_addr;	// warning: 分号

			unsigned char sin_zero[sizeof (struct sockaddr) - __SOCKADDR_COMMON_SIZE - sizeof (in_port_t) - sizeof (struct in_addr)]; // calculate pad size
		  };							// warning: 分号
		
		(6)struct sockaddr{}			// 实际编程中sockaddr使用sockaddr_in或者sockaddr_in6填充

3.9.5.IP地址格式转换函数实践
	3.9.5.1、inet_addr, inet_ntoa, inet_aton
	3.9.5.2、inet_pton, inet_ntop
		// in_addr_t inet_addr(const char *cp);		//
		// char *inet_ntoa(struct in_addr in);		// sockaddr_in.sin_addr


 // 客户端端口号可以自动分配

概念: 端口号，实质就是一个数字编号，用来在我们一台主机中（主机的操作系统中）唯一的标识一个能上网的进程。
	端口号和IP地址一起会被打包到当前进程发出或者接收到的每一个数据包中。
	每一个数据包将来在网络上传递的时候，内部都包含了发送方和接收方的信息（就是IP地址和端口号），
	所以IP地址和端口号这两个往往是打包在一起不分家的。

3.9.8.socket实践编程3
3.9.8.1、客户端发送&服务器接收
3.9.8.2、服务器发送&客户端接收
3.9.8.3、探讨：如何让服务器和客户端好好沟通
(1)客户端和服务器原则上都可以任意的发和收，但是实际上双方必须配合：
	client发的时候server就收，而server发的时候client就收
(2)必须了解到的一点：client和server之间的通信是异步的，这就是问题的根源
(3)解决方案：依靠应用层协议来解决。说白了就是我们server和client事先做好一系列的通信约定。


3.9.9.socket编程实践4
3.9.9.1、自定义应用层协议第一步：规定发送和接收方法
	(1)规定连接建立后由客户端主动向服务器发出1个请求数据包，然后服务器收到数据包后回复客户端一个回应数据包，这就是一个通信回合
	(2)整个连接的通信就是由N多个回合组成的。

3.9.9.2、自定义应用层协议第二步：定义数据包格式
3.9.9.3、常用应用层协议：http、ftp······
3.9.9.4、UDP简介











