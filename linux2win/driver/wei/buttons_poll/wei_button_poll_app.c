#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <poll.h>

#define BYTE_CNT	11

static int val = 0;



int main(int argc, char **argv)
{
	int fd = -1, ret = -1;
    struct pollfd fds[1];
    
    fd = open("/dev/button0", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}

    fds->fd      = fd;
    fds->events  = POLLIN;
    
    while (1)
    {
        int ret = -1;
        
        ret = poll(fds, 1, 5000);
        if (0 == ret)
        {
            printf("time out\n");
        }
        else
        {
            read(fd, &val, sizeof(val));
        
            printf("val = %d\n", val);
        }
    }
    
	ret = close(fd);
    
	
	return 0;
}



