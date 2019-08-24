#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BYTE_CNT	11

static char g_read_buff[BYTE_CNT] = {0};



int main(int argc, char **argv)
{
	int fd = -1, ret = -1;

    if (argc < 3)
    {
        printf("Usage:\n");
        printf("%s <0|1|2> <on|off> \n", argv[0]);
        exit(-1);
    }
    
    printf("argv[1] = %s\n", argv[1]);
    
    if (!strcmp("0", argv[1]))
    {
        fd = open("/dev/led_test0", O_RDWR);
    }
    else if (!strcmp("1", argv[1]))
    {
        fd = open("/dev/led_test1", O_RDWR);
    }
    else if (!strcmp("2", argv[1]))
    {
        fd = open("/dev/led_test2", O_RDWR);
    }
    else
    {
        printf("minor error\n");
        exit(-1);
    }
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}

    if (!strcmp("on", argv[2]))
    {
        ret = write(fd, "1", 1);
    }
    else
    {
        ret = write(fd, "0", 1);
    }
	
	
	ret = close(fd);
	if (ret < 0)
	{
		perror("close");
		exit(-1);
	}
	printf("app -- close /dev/led_test file success\n");
	
	return 0;
}


