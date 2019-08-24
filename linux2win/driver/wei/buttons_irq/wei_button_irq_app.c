#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BYTE_CNT	11

static int val = 0;



int main(int argc, char **argv)
{
	int fd = -1, ret = -1;
    
    fd = open("/dev/button0", O_RDWR);
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}
    
    while (1)
    {
        static int key_cnt = 0;
    
        read(fd, &val, sizeof(val));
        
        printf("val = %d\n", val);

        key_cnt += 1;
        
        if (key_cnt > 15)
        {
            break;
        }
    }
    
	ret = close(fd);
    
	
	return 0;
}


