#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>



int main(int argc, char *argv[])
{
	pid_t pid = -1;
	int ret = -1;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		printf("parent process: pid = %d\n", getpid());
		printf("parent process: ppid = %d\n", getppid());
	}
	else
	{
		printf("child process: pid = %d\n", getpid());
		printf("child process: ppid = %d\n", getppid());
		
		//ret = execl("./hello", "./hello", "aaaa", "bbbb", NULL);
		
		//ret = execl("/bin/ls", "ls", "-al", NULL);
		//ret = execl("ls", "ls", "-al", NULL);
		ret = execlp("ls", "ls", "-al", NULL);
		
		printf("CCCC\n");
		
		if (-1 == ret)
		{
			perror("execl");
			exit(-1);
		}
		else
		{
			printf("ret = %d\n", ret);
		}
		
		return 5;
	}
	
	return 0;
}


