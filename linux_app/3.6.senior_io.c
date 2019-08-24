
3.6.1.常见的阻塞
	 // wait()、pause()、sleep(); read或write某些文件时
	(2) 如何实现非阻塞IO访问：O_NONBLOCK/*打开同时设置*/和fcntl/*打开之后设置*/

3.6.2.阻塞式IO的困境 	// 多路io
	// cat /dev/input/mouse0

3.6.3.并发式IO的解决方案
	(1) 非阻塞式IO
	(2) 多路复用IO
	(3) 异步通知(异步IO)

3.6.4.IO多路复用原理
	(3) select()、poll()
	(4) 外部阻塞式，内部非阻塞式 自动 轮询多路阻塞式IO

	// nfds = MAX_FD + 1	// 监听范围

3.6.6.异步IO
	(1) 异步IO: 操作系统用软件实现的一套中断响应系统		// 类似于中断
	(2) 当前进程注册一个异步IO事件，使用signal注册一个信号SIGIO处理函数，
		当异步事件发生后当前进程会收到一个SIGIO信号从而执行绑定的处理函数去处理这个异步事件

3.6.6.2、涉及的函数
	(1) fcntl（F_GETFL、F_SETFL、O_ASYNC、F_SETOWN）

		flag = fcntl(mousefd, F_GETFL);
		//flag |= O_NONBLOCK;
		flag |= O_ASYNC;				// 把文件描述符设置为可 接收异步IO
		fcntl(mousefd, F_SETFL, flag);
		fcntl(mousefd, F_SETOWN, getpid());	// 把异步IO事件的接收进程设置为当前进程

		signal(SIGIO, func);

		// 异步io由内核驱动实现，驱动中实现异步io相关代码

3.6.7.存储映射IO
3.6.7.1、mmap函数
3.6.7.2、LCD显示和IPC之共享内存
3.6.7.3、存储映射IO的特点
	(1) 共享而不是复制，减少内存操作

// 显存和lcd建立关联，操作lcd等效为操作内存 -- 裸机可以直接操作物理内存

// 内核lcd驱动开辟显存，显存在内核中
// 内容在应用层中


// 多进程和多线程实现io并发



