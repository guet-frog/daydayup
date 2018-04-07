#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define FILENAME	"1.txt"


int main(void)
{
	int fd1 = -1, fd2 = -1;
	
	fd1 = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd1 < 0)
	{
		perror("open");
		return -1;
	}
	printf("fd1 = %d.\n", fd1);
	
	//close(1);		// 1就是标准输出stdout
	
	
	// 复制文件描述符
	//fd2 = dup(fd1);		// fd2一定等于1，因为前面刚刚关闭了1，这句话就把
	// 1.txt文件和标准输出就绑定起来了，所以以后输出到标准输出的信息就
	// 可以到1.txt中查看了。
	
	fd2 = dup2(fd1, 16);
	printf("fd2 = %d.\n", fd2);
//	printf("this is for test");

	while (1)
	{
		write(fd1, "aa", 2);
		sleep(1);
		write(fd2, "bb", 2);
	}


	
	close(fd1);
	return -1;
}



















