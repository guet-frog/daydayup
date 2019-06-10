#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>



int main(int argc, char argv[])
{
	pid_t pid = -1, wait_ret = -1;
	int status = -1;

	pid = fork();

	if (pid > 0)
	{
		printf("parent process: pid = %d\n", getpid());
		printf("parent process: ppid = %d\n", getppid());
		
		wait_ret = wait(&status);
		if (wait_ret < 0)
		{
			printf("fun@wait() exe error\n");
			exit(-1);
		}
		
		printf("recycle child pid = %d\n", wait_ret);
		printf("return value = %d\n", WEXITSTATUS(status));
	}
	else if (0 == pid)
	{
		printf("child process: pid = %d\n", getpid());
		printf("child process: ppid = %d\n", getppid());
		
		//sleep(1);
		
		return 5;
	}
	else
	{
		perror("func@fork()");
		exit(-1);
	}
	
	return 0;
}


