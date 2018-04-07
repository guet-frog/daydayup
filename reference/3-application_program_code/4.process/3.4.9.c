#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/wait.h>
#include <stdlib.h>



int main(void)
{
	pid_t pid = -1;
	pid_t ret = -1;
	int status = -1;
	
	pid = fork();
	if (pid > 0)
	{
		// 父进程
		printf("parent, 子进程id = %d.\n", pid);
	}
	else if (pid == 0)
	{
		// 子进程
		//execl("/bin/ls", "ls", "-l", "-a", NULL);		// ls -l -a
		//char * const arg[] = {"ls", "-l", "-a", NULL};
		//execv("/bin/ls", arg);
		
		//execl("hello", "aaa", "bbb", NULL);
		//char * const arg[] = {"aaa", "bbb", NULL};
		//execv("hello", arg);
		
		//execlp("ls", "ls", "-l", "-a", NULL);	
		char * const envp[] = {"AA=aaaa", "XX=abcd", NULL};
		execle("hello", "hello", "-l", "-a", NULL, envp);
		
		return 0;
	}
	else
	{
		perror("fork");
		return -1;
	}
	
	return 0;
}