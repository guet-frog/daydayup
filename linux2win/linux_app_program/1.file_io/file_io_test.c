#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdint.h>
#include <string.h>

#define	READ_BYTE_NUM	20

void myOpen(const char *path, int flag, ...)
{
	int fd = 0;
	fd = open(path, flag);
	if (fd < 0)
	{
		printf("fun@myOpen exe error\n");
		//return;	// 程序继续运行，只有main函数返回，程序才终止
		_exit(-1);  // 程序挂掉
	}
	
}


int main(void)
{
	
	int fd = -1;
	int ret = -1;
	int count = 0;
	
	char readFileBuff[READ_BYTE_NUM] = {0};
	const char *writeBuffTmp = "hello world";
	
	//char a[5] = "hello";		// 字符串定义
	//char a[]  = "hello";
	//char *a   = "hello";
	//printf("sizeof(writeBuffTmp) = %d\n", sizeof(writeBuffTmp));	// 4
	//printf("strlen(writeBuffTmp) = %d\n", strlen(writeBuffTmp));	// 11
	
	//fd = open("test.txt", O_RDWR);				// const char *, 有限制
	//fd = open("test.txt", (O_RDWR | O_TRUNC));	// 丢弃以前内容 readCount = 0
	//fd = open("test.txt", (O_RDWR | O_APPEND));	// 保留以前内容, write后面增加, 且无限制
	//fd = open("test.txt", O_APPEND);
	if (fd < 0)	// file descriptor
	{
		printf("fun@open() exe error\n");
		_exit(-1);
	}
	else
	{
		printf("fun@open() exe success fd = %d\n", fd);
	}
	
	//myOpen("test1.txt", O_RDWR); // 测试 return && _exit()

	memset(readFileBuff, 0, READ_BYTE_NUM);
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

//-------------------------------------------------------------
#if 1
	count = write(fd, writeBuffTmp, strlen(writeBuffTmp));
	if (count < 0)
	{
		printf("fun@write() exe error\n");
		_exit(-1);
	}
	else
	{
		printf("fun@write() exe success write_count = %d\n", count);
	}
#endif

	memset(readFileBuff, 0, READ_BYTE_NUM);
	count = read(fd, readFileBuff, READ_BYTE_NUM);
	if (count < 0)
	{
		printf("fun@read_2() exe error\n");
		_exit(-1);
	}
	else
	{
		printf("fun@read_2() exe success read_count = %d\n", count);
	}

	printf("readFileBuff = %s\n", readFileBuff);
//-------------------------------------------------------------	
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










