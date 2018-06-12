
3.9.3.TCP协议的学习2
		-- 建立连接的条件: 服务器listen时客户端主动发起connect
		-- 关闭连接需要四次握手, 服务器或者客户端都可以主动发起关闭
		-- 握手协议已经封装在TCP协议内部, socket编程接口平时不用管
		-- 客户端端口号可以自动分配

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
		(6)send和write
		(7)recv和read	// flag = 0

	3.9.4.4、辅助性函数
		(1)inet_addr, inet_aton, inet_ntoa			// inet_addr
		(2)inet_ntop、inet_pton						// net to point   warning: return const char *

		// in_addr_t inet_addr(const char *cp);		//
		// char *inet_ntoa(struct in_addr in);		// sockaddr_in.sin_addr

	3.9.4.5、表示IP地址相关数据结构
		(1)都定义在 netinet/in.h

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

3.9.8.3、探讨：如何让服务器和客户端好好沟通
	(1)客户端和服务器原则上都可以任意的发和收, 但是实际上双方必须配合
	(2)//client和server之间的通信是异步的, 这就是问题的根源
	(3)解决方案: 依靠应用层协议来解决, server和client事先做好一系列的通信约定












