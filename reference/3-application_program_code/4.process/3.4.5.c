#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>



int main(void)
{
	// 首先打开一个文件
	int fd = -1;
	pid_t pid = -1;
	
	// fork创建子进程
	pid = fork();
	if (pid > 0)
	{
		// 父进程中
		fd = open("1.txt", O_RDWR | O_APPEND);
		if (fd < 0)
		{
			perror("open");
			return -1;
		}
		
		printf("parent.\n");
		write(fd, "hello", 5);
		sleep(1);
	}
	else if (pid == 0)
	{
		// 子进程
		fd = open("1.txt", O_RDWR | O_APPEND);
		if (fd < 0)
		{
			perror("open");
			return -1;
		}
		
		printf("child.\n");
		write(fd, "world", 5);
		sleep(1);
	}
	else
	{
		perror("fork");
		exit(-1);
	}
	close(fd);
	
	
	
	
	
	
	
	
	
/*
	// 首先打开一个文件
	int fd = -1;
	pid_t pid = -1;
	
	fd = open("1.txt", O_RDWR | O_TRUNC);
	if (fd < 0)
	{
		perror("open");
		return -1;
	}
	
	// fork创建子进程
	pid = fork();
	if (pid > 0)
	{
		// 父进程中
		printf("parent.\n");
		write(fd, "hello", 5);
		sleep(1);
	}
	else if (pid == 0)
	{
		// 子进程
		printf("child.\n");
		write(fd, "world", 5);
		sleep(1);
	}
	else
	{
		perror("fork");
		exit(-1);
	}
	close(fd);
*/	
	return 0;
}