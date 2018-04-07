#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void create_daemon(void);


int main(void)
{
	create_daemon();
	
	
	while (1)
	{
		printf("I am running.\n");
		
		sleep(1);
	}
	
	return 0;
}


// 函数作用就是把调用该函数的进程变成一个守护进程
void create_daemon(void)
{
	pid_t pid = 0;
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(-1);
	}
	if (pid > 0)
	{
		exit(0);		// 父进程直接退出
	}
	
	// 执行到这里就是子进程
	
	// setsid将当前进程设置为一个新的会话期session，目的就是让当前进程
	// 脱离控制台。
	pid = setsid();
	if (pid < 0)
	{
		perror("setsid");
		exit(-1);
	}
	
	// 将当前进程工作目录设置为根目录
	chdir("/");
	
	// umask设置为0确保将来进程有最大的文件操作权限
	umask(0);
	
	// 关闭所有文件描述符
	// 先要获取当前系统中所允许打开的最大文件描述符数目
	int cnt = sysconf(_SC_OPEN_MAX);
	int i = 0;
	for (i=0; i<cnt; i++)
	{
		close(i);
	}
	
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

}




