#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>


int main(void)
{
	// 读取鼠标
	int fd = -1, ret = -1;
	char buf[200];
	fd_set myset;
	struct timeval tm;
	
	fd = open("/dev/input/mouse1", O_RDONLY);
	if (fd < 0)
	{
		perror("open:");
		return -1;
	}
	
	// 当前有2个fd，一共是fd一个是0
	// 处理myset
	FD_ZERO(&myset);
	FD_SET(fd, &myset);
	FD_SET(0, &myset);
	
	tm.tv_sec = 10;
	tm.tv_usec = 0;

	ret = select(fd+1, &myset, NULL, NULL, &tm);
	if (ret < 0)
	{
		perror("select: ");
		return -1;
	}
	else if (ret == 0)
	{
		printf("超时了\n");
	}
	else
	{
		// 等到了一路IO，然后去监测到底是哪个IO到了，处理之
		if (FD_ISSET(0, &myset))
		{
			// 这里处理键盘
			memset(buf, 0, sizeof(buf));
			read(0, buf, 5);
			printf("键盘读出的内容是：[%s].\n", buf);
		}
		
		if (FD_ISSET(fd, &myset))
		{
			// 这里处理鼠标
			memset(buf, 0, sizeof(buf));
			read(fd, buf, 50);
			printf("鼠标读出的内容是：[%s].\n", buf);
		}
	}

	return 0;
}













