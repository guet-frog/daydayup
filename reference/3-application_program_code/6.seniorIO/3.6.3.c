#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(void)
{
	// 读取鼠标
	int fd = -1;
	int flag = -1;
	char buf[200];
	int ret = -1;
	
	fd = open("/dev/input/mouse1", O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("open:");
		return -1;
	}
	
	// 把0号文件描述符（stdin）变成非阻塞式的
	flag = fcntl(0, F_GETFL);		// 先获取原来的flag
	flag |= O_NONBLOCK;				// 添加非阻塞属性
	fcntl(0, F_SETFL, flag);		// 更新flag
	// 这3步之后，0就变成了非阻塞式的了
	
	while (1)
	{
		// 读鼠标
		memset(buf, 0, sizeof(buf));
		//printf("before 鼠标 read.\n");
		ret = read(fd, buf, 50);
		if (ret > 0)
		{
			printf("鼠标读出的内容是：[%s].\n", buf);
		}
		
		// 读键盘
		memset(buf, 0, sizeof(buf));
		//printf("before 键盘 read.\n");
		ret = read(0, buf, 5);
		if (ret > 0)
		{
			printf("键盘读出的内容是：[%s].\n", buf);
		}
	}
	
	return 0;
}


/*
int main(void)
{
	// 读取鼠标
	int fd = -1;
	char buf[200];
	
	fd = open("/dev/input/mouse1", O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("open:");
		return -1;
	}
	
	memset(buf, 0, sizeof(buf));
	printf("before read.\n");
	read(fd, buf, 50);
	printf("读出的内容是：[%s].\n", buf);
	
	
	return 0;
}
*/

/*
int main(void)
{
	// 读取键盘
	// 键盘就是标准输入，stdin
	
	char buf[100];
	int flag = -1;
	
	// 把0号文件描述符（stdin）变成非阻塞式的
	flag = fcntl(0, F_GETFL);		// 先获取原来的flag
	flag |= O_NONBLOCK;				// 添加非阻塞属性
	fcntl(0, F_SETFL, flag);		// 更新flag
	// 这3步之后，0就变成了非阻塞式的了
	
	memset(buf, 0, sizeof(buf));
	printf("before read.\n");
	read(0, buf, 5);
	printf("读出的内容是：[%s].\n", buf);
	
	return 0;
}
*/