
3.5.1.什么是信号
	(1) 信号是 内容受限的 异步通信机制；	// 信号是异步的，类似于软件中断

3.5.1.2、信号由谁发出
	(1) 用户在终端按下按键
	(2) 硬件异常后由操作系统内核发出信号
	(3) 用户使用kill命令向其他进程发出信号
	(4) 某种软件条件满足后也会发出信号，如alarm闹钟时间到会产生SIGALARM信号，
		向一个读端已经关闭的管道write时会产生SIGPIPE信号

3.5.1.3、信号由谁处理、如何处理
	(1) 忽略信号		/* man手册忽略 Ctrl+c */
	(2) 捕获信号（信号绑定了一个函数）
	(3) 默认处理（当前进程没有明显的管这个信号，默认：忽略或终止进程）

3.5.2.常见信号介绍
	(1) SIGINT			2		Ctrl+C时OS送给前台进程组中每个进程
	(2) SIGABRT			6		调用abort函数，进程异常终止
	(3) SIGPOLL	SIGIO	8		指示一个异步IO事件，在高级IO中提及
	(4) SIGKILL			9		杀死进程的终极办法	/* 有权限要求, 无法被忽略 */
	(5) SIGSEGV			11		无效存储访问时OS发出该信号
	(6) SIGPIPE			13		涉及管道和socket
	(7) SIGALARM		14		涉及alarm函数的实现
	(8) SIGTERM			15		kill命令发送的OS默认终止信号
	(9) SIGCHLD			17		子进程终止或停止时OS向其父进程发此信号
	(10) SIGUSR1		10		用户自定义信号，作用和意义由应用自己定义
		 SIGUSR2		12

	// /usr/include/signal.h
	// /usr/include/i386-linux-gnu/bits/signum.h

3.5.3.2、用signal函数处理SIGINT信号
	(1) 默认处理，忽略处理，捕获处理
	(2) signal()绑定一个捕获函数，信号发生后自动执行该函数并传参信号编号

	int main(void)
	{
		signal(SIGINT, func);		// SIGINT与func绑定，func中形参sig是否多余
									// 主动捕获方法 覆盖 默认方法
		return 0;
	}

	void func(int sig)
	{
		if (SIGINT != sig)		// for double check
		{
			return;
		}

		// 独立中断和共享中断(多个信号绑定同一个服务函数)
	}
	
	// signal(SIGINT, SIG_IGN);		// 忽略信号 -- SIGKILL不可忽略

3.5.3.4、sigaction函数介绍		// 传参NULL，可以获取旧捕获函数
	// signal()绑定捕获函数时不同版本有差异

3.5.4.1、alarm函数
	// 内核只会给每一个进程维护一个alarm时钟，重复调用alarm()会覆盖

3.5.4.2、pause函数
	// alarm信号可以唤醒pause

3.5.4.3、使用alarm和pause来模拟sleep


