#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>


#define FILE	"/var/aston_test_single"
void delete_file(void);

int main(void)
{
	// 程序执行之初，先去判断文件是否存在
	int fd = -1;
	fd = open(FILE, O_RDWR | O_TRUNC | O_CREAT | O_EXCL, 0664);
	if (fd < 0)
	{
		if (errno == EEXIST)
		{
			printf("进程已经存在，并不要重复执行\n");
			return -1;
		}
	}
	
	atexit(delete_file);			// 注册进程清理函数
	
	int i = 0;
	for (i=0; i<10; i++)
	{
		printf("I am running...%d\n", i);
		sleep(1);
	}

	return 0;
}


void delete_file(void)
{
	remove(FILE);
}
