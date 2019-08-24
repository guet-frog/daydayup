
3.7.1.3、进程技术的劣势
	(1) 进程间切换开销大
	(2) 进程间通信麻烦而且效率低

3.7.2.2、linux中的线程简介
	(1) 一种轻量级进程
	(2) 一个进程中可以有多个线程，线程是参与内核调度的最小单元
	(3) 同一进程的多个线程之间很容易高效率通信
	(4) 在多核心CPU(对称多处理器架构SMP)架构下效率最大化

// gcc xxx -lpthread

3.7.3.线程常见函数
3.7.3.1、线程创建与回收
	(1) pthread_create()
	(2) pthread_join()			主线程等待（阻塞）回收子线程	// pthread_exit()传参给pthread_join()
	(3) pthread_detach()		主线程分离子线程，分离后主线程不必再去回收子线程

// 线程要么回收，要么分离。否则需要等到进程终止才可以回收资源

3.7.3.2、线程取消
	(1) pthread_cancel()			一般都是主线程调用该函数去取消子线程
	(2) pthread_setcancelstate()	子线程设置是否允许被取消
	(3) pthread_setcanceltype()

3.7.3.3、线程函数退出相关
	(1) pthread_exit()与return退出
	(2) pthread_cleanup_push(func, args)	// 子线程被cancel后，会自动调用func() -- 释放锁
	(3) pthread_cleanup_pop()	// pthread_cleanup_pop(0) 取出func() 不执行

3.7.3.4、获取线程id
	(1) pthread_self()

3.7.4.3、信号量的介绍和使用
	// sem_t sem;
	// sem_init(&sem, 0, 0);	// 类似于cnt，初始化为0		-- sem_destroy(&sem);

	// sem_post(&sem);		// 发送sem

	// sem_wait(&sem);

3.7.6.1、互斥锁

	(2) 相关函数：pthread_mutex_init() pthread_mutex_destroy()
				  pthread_mutex_lock() pthread_mutex_unlock()
	(3) 互斥锁和信号量的关系：可以认为互斥锁是一种特殊的信号量
	(4) 互斥锁主要用来实现关键段保护

	// man 3 pthread_mutex_init -- 需要安装相应的man手册 sudo apt-get install manpages-posix-dev

3.7.7.线程同步之条件变量
		pthread_cond_init()		pthread_cond_destroy()
		pthread_cond_wait()		pthread_cond_signal/pthread_cond_broadcast()


// posix标准thread  -- pthread


