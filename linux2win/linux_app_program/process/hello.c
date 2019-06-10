#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[])
{
	int fd = -1;
	int write_num = -1;
	char *write_buf = "hello world\n";
	
	//fd = open("/tmp/daemon.log", O_RDWR | O_CREAT | O_TRUNC, 0600);
	fd = open("./tmp.log", O_RDWR | O_CREAT | O_APPEND, 0600);
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}
	
	printf("strlen(write_buf) = %d\n", strlen(write_buf));
	
	// write_num = write(fd, write_buf, strlen(write_buf));
	// printf("write_num = %d\n", write_num);
	
	write_num = write(fd, write_buf, strlen(write_buf) + 1);
	printf("write_num = %d\n", write_num);
	
	
	close(fd);
	
	sleep(1);
}
