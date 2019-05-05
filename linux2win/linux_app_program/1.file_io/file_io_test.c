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

int curFilePointLocation(int fd)
{
    return lseek(fd, 0, SEEK_CUR);
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

#if 0 /* test repeat open file */
    int fd1 = -1;
    fd1 = open("test.txt", (O_RDWR | O_APPEND));
    if (fd1 < 0)
    {
        printf("repeat open file ok, fd1 = %d\n", fd1);
        _exit(-1);
    }
    else
    {
        //perror("repeat open file");
        printf("repeat open file success fd1 = %d\n", fd1);
    }
#endif /* test repeat open file */
	
	//myOpen("test1.txt", O_RDWR); // 测试 return && _exit()
	//fd1 = open("test3.txt", (O_RDWR | O_CREAT | O_EXCL), 0666);   // 没有文件则创建文件(并设置rom文件权限)open, 有文件则报错

#if 0 /* test read file */
    //memset(readFileBuff, 0, READ_BYTE_NUM);
    memset(readFileBuff, 0, sizeof(readFileBuff)); // 进程中内存err：stack smashing detected, Aborted (core dumped)
	
    //count = read(fd, readFileBuff, /*READ_BYTE_NUM*/ 20); // error：readFileBuff overflow(爆了)
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
	
	printf("read = %s\n", readFileBuff);    // %s
#endif /* test read file*/

#if 0 /* test lseek */
    ret = lseek(fd, FILE_POINT_OFFSET, SEEK_CUR);
    if (fd < 0)
    {
        perror("fun@lseek");
        _exit(-1);
    }
    else
    {
        printf("file:fd=%d current file point location is %d\n", fd, ret);
    }
#endif /* test lseek */

//-------------------------------------------------------------
#if 1 /* test write file */
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
#endif /* test write file */


// in this, we can use lseek fun or add param O_APPEND to avoid coverage (remove file point fd1) -- 2018-4-12

#if 0 /* test repeat read */
	memset(readFileBuff, 0, sizeof(readFileBuff));
	count = read(fd1, readFileBuff, READ_BYTE_NUM);
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
#endif /* test repeat read*/
//-------------------------------------------------------------	
    
    //printf("current file point location is %d\n", curFilePointLocation(fd));
    //printf("current file point location is %d\n", curFilePointLocation(fd1));

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










