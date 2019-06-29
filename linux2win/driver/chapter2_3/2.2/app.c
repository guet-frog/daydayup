#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BYTE_CNT	11

static char g_read_buff[BYTE_CNT] = {0};



int main(void)
{
	int fd = -1, ret = -1;
	
	fd = open("/dev/led_test", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}
	printf("app -- open /dev/led_test file success\n");
	
	ret = write(fd, "hello world", BYTE_CNT);
	if (ret < 0)
	{
		perror("write");
		exit(-1);
	}
	else
	{
		printf("app -- write %d bytes\n", ret);		// ret = 0
	}
	
	ret = read(fd, g_read_buff, BYTE_CNT);
	if (ret < 0)
	{
		perror("read");
		exit(-1);
	}
	else
	{
		printf("app -- read %d bytes\n", ret);
		printf("app -- g_read_buff = %s\n", g_read_buff);	// ret = 0
	}
	
	sleep(3);
	
	ret = close(fd);
	if (ret < 0)
	{
		perror("close");
		exit(-1);
	}
	printf("app -- close /dev/led_test file success\n");
	
	return 0;
}


