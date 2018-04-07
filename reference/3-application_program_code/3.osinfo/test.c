#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
	int fd = 0;
	char buf[200] = {0};
	
	fd = open("/proc/version", O_RDONLY);
	
	read(fd, buf, 200);
	printf("version:%s.\n", buf);

	return 0;
}