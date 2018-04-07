#include <stdio.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
	int ret = -1;
	
	
	if (argc != 2)
	{
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}
	
	ret = chmod(argv[1], S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWOTH);
	if (ret < 0)
	{
		perror("chmod");
		return -1;
	}
	
	return 0;
}
















