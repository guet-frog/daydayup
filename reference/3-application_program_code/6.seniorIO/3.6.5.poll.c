#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>



int main(void)
{
	// 读取鼠标
	int fd = -1, ret = -1;
	char buf[200];
	struct pollfd myfds[2] = {0};
	
	fd = open("/dev/input/mouse1", O_RDONLY);
	if (fd < 0)
	{
		perror("open:");
		return -1;
	}
	
	// 初始化我们的pollfd
	myfds[0].fd = 0;			// 键盘
	myfds[0].events = POLLIN;	// 等待读操作
	
	myfds[1].fd = fd;			// 鼠标
	myfds[1].events = POLLIN;	// 等待读操作

	ret = poll(myfds, fd+1, 10000);
	if (ret < 0)
	{
		perror("poll: ");
		return -1;
	}
	else if (ret == 0)
	{
		printf("超时了\n");
	}
	else
	{
		// 等到了一路IO，然后去监测到底是哪个IO到了，处理之
		if (myfds[0].events == myfds[0].revents)
		{
			// 这里处理键盘
			memset(buf, 0, sizeof(buf));
			read(0, buf, 5);
			printf("键盘读出的内容是：[%s].\n", buf);
		}
		
		if (myfds[1].events == myfds[1].revents)
		{
			// 这里处理鼠标
			memset(buf, 0, sizeof(buf));
			read(fd, buf, 50);
			printf("鼠标读出的内容是：[%s].\n", buf);
		}
	}

	return 0;
}




