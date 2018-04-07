#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(void)
{
	// 思路就是创建子进程，然后父子进程中分别进行读键盘和鼠标的工作
	int ret = -1;
	int fd = -1;
	char buf[200];
	
	ret = fork();
	if (ret == 0)
	{
		// 子进程
		fd = open("/dev/input/mouse1", O_RDONLY);
		if (fd < 0)
		{
			perror("open:");
			return -1;
		}
		
		while (1)
		{
			memset(buf, 0, sizeof(buf));
			printf("before read.\n");
			read(fd, buf, 50);
			printf("读出鼠标的内容是：[%s].\n", buf);
		}	
	}
	else if (ret > 0)
	{
		// 父进程
		while (1)
		{
			memset(buf, 0, sizeof(buf));
			printf("before read.\n");
			read(0, buf, 5);
			printf("读出键盘的内容是：[%s].\n", buf);			
		}
	}
	else
	{
		perror("fork:");
	}
	
	return 0;
}

