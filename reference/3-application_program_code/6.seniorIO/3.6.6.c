#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>



int mousefd = -1;


// 绑定到SIGIO信号，在函数内处理异步通知事件
void func(int sig)
{
	char buf[200] = {0};
	
	if (sig != SIGIO)
		return;

	read(mousefd, buf, 50);
	printf("鼠标读出的内容是：[%s].\n", buf);
}

int main(void)
{
	// 读取鼠标
	char buf[200];
	int flag = -1;
	
	mousefd = open("/dev/input/mouse1", O_RDONLY);
	if (mousefd < 0)
	{
		perror("open:");
		return -1;
	}	
	// 把鼠标的文件描述符设置为可以接受异步IO
	flag = fcntl(mousefd, F_GETFL);
	flag |= O_ASYNC;
	fcntl(mousefd, F_SETFL, flag);
	// 把异步IO事件的接收进程设置为当前进程
	fcntl(mousefd, F_SETOWN, getpid());
	
	// 注册当前进程的SIGIO信号捕获函数
	signal(SIGIO, func);
	
	// 读键盘
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		//printf("before 键盘 read.\n");
		read(0, buf, 5);
		printf("键盘读出的内容是：[%s].\n", buf);
	}
		
	return 0;
}