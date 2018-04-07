#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


int main(void)
{
	pid_t p1 = -1;
	
	p1 = fork();		// 返回2次
	
	if (p1 == 0)
	{
		// 这里一定是子进程
		
		// 先sleep一下让父进程先运行，先死
		sleep(1);
		
		printf("子进程, pid = %d.\n", getpid());		
		printf("hello world.\n");
		printf("子进程, 父进程ID = %d.\n", getppid());
	}
	
	if (p1 > 0)
	{
		// 这里一定是父进程
		printf("父进程, pid = %d.\n", getpid());
		printf("父进程, p1 = %d.\n", p1);
	}
	
	if (p1 < 0)
	{
		// 这里一定是fork出错了
	}
	
	// 在这里所做的操作
	//printf("hello world, pid = %d.\n", getpid());

	return 0;
}