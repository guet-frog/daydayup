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
		//sleep(1);
		printf("parent.\n");
		ret = wait(&status);
		
		printf("子进程已经被回收，子进程pid = %d.\n", ret);
		printf("子进程是否正常退出：%d\n", WIFEXITED(status));
		printf("子进程是否非正常退出：%d\n", WIFSIGNALED(status));
		printf("正常终止的终止值是：%d.\n", WEXITSTATUS(status));
	}
	else if (pid == 0)
	{
		// 子进程
		printf("child pid = %d.\n", getpid());
		return 51;
		//exit(0);
	}
	else
	{
		perror("fork");
		return -1;
	}
	
	return 0;
}












