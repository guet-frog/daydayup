#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define NAME "1.txt"

int main(void)
{
	int ret = -1;
	struct stat buf;
	
	memset(&buf, 0, sizeof(buf));		// memset后buf中全是0
	ret = stat(NAME, &buf);				// stat后buf中有内容了
	if (ret < 0)
	{
		perror("stat");
		exit(-1);
	}
	// 成功获取了stat结构体，从中可以得到各种属性信息了
	printf("inode = %d.\n", buf.st_ino);
	printf("size = %d bytes.\n", buf.st_size);
	printf("st_blksize = %d.\n", buf.st_blksize);
	
	return 0;
}



















