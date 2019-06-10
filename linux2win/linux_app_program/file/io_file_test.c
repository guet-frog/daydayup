#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define	READ_BYTE_NUM	    11
//#define FILE_POINT_OFFSET   READ_BYTE_NUN
#define FILE_POINT_OFFSET   11

void fun(void)
{
	printf("fun@myOpen exe error\n");
	//return;	// 程序继续运行，只有main函数返回，程序才终止
	_exit(-1);  // 程序挂掉
}

int main(void)
{
	int fd = -1;
	int ret = -1;
	int count = 0;
	
	char readFileBuff[READ_BYTE_NUM] = {0};
	const char *writeBuffTmp = "hello world";
	const char *writeBuffTmp1 = "wwwww qqqqq";
	
	// char a[5] = "hello";     // sizeof(a) = 5
	// char b[]  = "hello";     // sizeof(b) = 6
	// char *c   = "hello";     // sizeof(c) = 4

	//fd = open("test.txt", O_RDWR);				// const char *, 不加O_APPEND, 注意filePoint
	//fd = open("test.txt", O_APPEND);				// 默认是O_RDONLY
	//fd = open("test.txt", (O_RDWR | O_TRUNC));	// 丢弃以前内容 readCount = 0
	//fd = open("test.txt", (O_RDWR | O_APPEND | O_CREAT | O_EXCL));    // 文件存在则报错, 保留以前内容, write后面增加, 且无限制
    fd = open("test.txt", (O_RDWR | O_APPEND));	
	if (fd < 0)	// file descriptor
	{
		printf("fun@open() exe error\n");
		_exit(-1);
	}
	else
	{
        //perror("成功");                                 // success + \n
		printf("fun@open() exe success fd = %d\n", fd);
	}

    //memset(readFileBuff, 0, READ_BYTE_NUM);
    memset(readFileBuff, 0, sizeof(readFileBuff)); // 进程中内存err：stack smashing detected, Aborted (core dumped)

    count = read(fd, readFileBuff, READ_BYTE_NUM);
	if (count < 0)
	{
		printf("fun@read() exe error\n");
		_exit(-1);
	}
	else
	{
		printf("fun@read() exe success read_count = %d\n", count);
	}
	
	printf("read = %s\n", readFileBuff);
	
    ret = close(fd);
	if (ret < 0)
	{
		printf("fun@close() exe error\n");
		_exit(-1);
	}
	else
	{
		printf("fun@close() exe success\n");
	}	
	
	return 0;
}










