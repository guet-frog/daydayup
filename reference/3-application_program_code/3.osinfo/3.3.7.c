#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char **argv)
{
	int fd = -1;
	char buf[512] = {0};
	
	if (argc != 2)
	{
		printf("usage: %s -v|-d\n", argv[0]);
		return -1;
	}
	
	if (!strcmp(argv[1], "-v"))
	{
		fd = open("/proc/version", O_RDONLY);
		if (fd < 0)
		{
			perror("open /proc/version");
			return -1;
		}
		read(fd, buf, sizeof(buf));
		printf("结果是：%s.\n", buf);
	}
	else if (!strcmp(argv[1], "-d"))
	{
		fd = open("/proc/devices", O_RDONLY);
		if (fd < 0)
		{
			perror("open /proc/devices");
			return -1;
		}
		read(fd, buf, sizeof(buf));
		printf("结果是：%s.\n", buf);
	}
	
	return 0;
}















