#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char *argv[])
{
	int fd = -1;		// fd 就是file descriptor，文件描述符
	char buf[100] = {0};
	char writebuf[20] = "l love linux";
	int ret = -1;
	
	// 第一步：打开文件
	fd = open("a.txt", O_RDWR);
	//fd = open("a.txt", O_RDONLY);
	if (-1 == fd)		// 有时候也写成： (fd < 0)
	{
		//printf("\n");
		perror("文件打开错误");
		// return -1;
		_exit(-1);
	}
	else
	{
		printf("文件打开成功，fd = %d.\n", fd);
	}
	
	ret = lseek(fd, 3, SEEK_SET);
	printf("lseek, ret = %d.\n", ret);
	
	


#if 0	
	// 第二步：读写文件
	// 写文件
	ret = write(fd, writebuf, strlen(writebuf));
	if (ret < 0)
	{
		//printf("write失败.\n");
		perror("write失败");
		_exit(-1);
	}
	else
	{
		printf("write成功，写入了%d个字符\n", ret);
	}
#endif


#if 1
	// 读文件
	ret = read(fd, buf, 20);
	if (ret < 0)
	{
		printf("read失败\n");
		_exit(-1);
	}
	else
	{
		printf("实际读取了%d字节.\n", ret);
		printf("文件内容是：[%s].\n", buf);
	}
#endif	

	// 第三步：关闭文件
	close(fd);
	
	_exit(0);
}



















