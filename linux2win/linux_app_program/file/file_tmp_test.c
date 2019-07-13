#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int ret = -1, fd = -1;
	int var = 98;
	
	fd = open("./test.txt", O_RDWR | O_TRUNC);
	if (open < 0)
	{
		perror("open()");
		exit(-1);
	}
	
	ret = write(fd, &var, 4);
	if (ret < 0)
	{
		perror("write()");
		exit(-1);
	}
	printf("ret = %d\n", ret);
	
	close(fd);
	
	return 0;
}