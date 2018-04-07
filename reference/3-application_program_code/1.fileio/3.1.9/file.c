#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char *argv[])
{
	int fd1 = -1, fd2 = -1;		// fd 就是file descriptor，文件描述符
	char buf[100] = {0};
	char writebuf[20] = "l love linux";
	int ret = -1;
	
	// 第一步：打开文件
	fd1 = open("a.txt", O_RDWR);
	fd2 = open("a.txt", O_RDWR);
	
	//fd = open("a.txt", O_RDONLY);
	if ((-1 == fd1) || (fd2 == -1))		// 有时候也写成： (fd < 0)
	{
		//printf("\n");
		perror("文件打开错误");
		// return -1;
		_exit(-1);
	}
	else
	{
		printf("文件打开成功，fd1 = %d. fd2 = %d.\n", fd1, fd2);
	}
	


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
	while(1)
	{
		// 读文件
		memset(buf, 0, sizeof(buf));
		ret = read(fd1, buf, 2);
		if (ret < 0)
		{
			printf("read失败\n");
			_exit(-1);
		}
		else
		{
			//printf("实际读取了%d字节.\n", ret);
			printf("fd1：[%s].\n", buf);
		}
		
		sleep(1);
		
		// 读文件
		memset(buf, 0, sizeof(buf));
		ret = read(fd2, buf, 2);
		if (ret < 0)
		{
			printf("read失败\n");
			_exit(-1);
		}
		else
		{
			//printf("实际读取了%d字节.\n", ret);
			printf("fd2：[%s].\n", buf);
		}
		
	}

	
#endif	


	// 第三步：关闭文件
	close(fd1);
	close(fd2);
	
	_exit(0);
}













