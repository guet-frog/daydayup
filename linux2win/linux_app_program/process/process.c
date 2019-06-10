#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



void atexit_callback(void)
{
	printf("atexit_callback() exe\n");

	return;
}

void fun(void)
{
	printf("call fun()\n");

	//exit(0);
	//_exit(0);
	return;
}

int main(int argc, char argv[])
{
	pid_t pid = -1;
	int fd = -1;
	int ret = 0;
	
	// /* open file */
	// fd = open("./test.txt", (O_RDWR | O_CREAT | O_TRUNC), 0666);
	// if (fd < 0)
	// {
		// perror("func@open()");
		// exit(-1);
	// }

	pid = fork();

	if (pid > 0)
	{
		printf("parent process: pid = %d\n", getpid());
		printf("parent process: ppid = %d\n", getppid());
		
		/* open file */
		fd = open("./test.txt", (O_RDWR | O_CREAT | O_TRUNC), 0666);
		if (fd < 0)
		{
			perror("parent@open()");
			exit(-1);
		}
		
		ret = write(fd, "hello", 5);
		if (ret < 0)
		{
			perror("parent@write()");
			exit(-1);
		}

		sleep(1);
	}
	else if (0 == pid)
	{
		printf("child process: pid = %d\n", getpid());
		printf("child process: ppid = %d\n", getppid());
		
		sleep(1);

		ret = write(fd, "world", 5);
		if (ret < 0)
		{
			perror("child@write()");
			exit(-1);
		}

		/* close file */
		// ret = close(fd);
		// if (ret < 0)
		// {
			// perror("func@close()");
			// exit(-1);
		// }
	}
	else
	{
		perror("func@fork()");
		exit(-1);
	}
	
	return 0;
}
