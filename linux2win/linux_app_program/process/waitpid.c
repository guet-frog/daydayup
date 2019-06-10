#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>



int main (int argc, char *argv[])
{
	pid_t pid = -1;
	int ret = -1, status = -1;
	
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	/* code executed by child */
	if (0 == pid)
	{
		printf("child process has exe\n");
		printf("pid = %d, ppid = %d\n", getpid(), getppid());
		sleep(5);
		printf("child process end\n");
	}
	else 	/* code executed by parent */
	{
		printf("parent process has exe\n");
		printf("pid = %d, ppid = %d\n", getpid(), getppid());
		
		while (1)
		{
			ret = waitpid(pid, &status, WNOHANG);
			
			if (0 == ret)
			{
				printf("wait . . .\n");
				sleep(1);
			}
			else if (ret > 0)
			{
				printf("child process %d has end\n", ret);
				
				if (WIFEXITED(status))
				{
					printf("exited, status=%d\n", WEXITSTATUS(status));
				}
				else if (WIFSIGNALED(status))
				{
					printf("killed by signal %d\n", WTERMSIG(status));
				}
				else if (WIFSTOPPED(status))
				{
					printf("stopped by signal %d\n", WSTOPSIG(status));
				}
				else if (WIFCONTINUED(status))
				{
					printf("continued\n");
				}
				
				break;
			}
			else
			{
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	return 0;
}



/*
int main(int argc, char *argv[])
{
   pid_t cpid, w;
   int status;

   cpid = fork();
   if (cpid == -1)
   {
	   perror("fork");
	   exit(EXIT_FAILURE);
   }

	// Code executed by child
   if (cpid == 0)
   {
	   printf("Child PID is %ld\n", (long) getpid());
	   if (argc == 1)
	   {
		  pause();	// Wait for signals
	   }
	   
	   _exit(atoi(argv[1]));
   }
   else
   {
		do
		{
		   w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);		// block
		   if (w == -1) {
			   perror("waitpid");
			   exit(EXIT_FAILURE);
		   }

		   if (WIFEXITED(status)) {
			   printf("exited, status=%d\n", WEXITSTATUS(status));
		   } else if (WIFSIGNALED(status)) {
			   printf("killed by signal %d\n", WTERMSIG(status));
		   } else if (WIFSTOPPED(status)) {
			   printf("stopped by signal %d\n", WSTOPSIG(status));
		   } else if (WIFCONTINUED(status)) {
			   printf("continued\n");
		   }
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		exit(EXIT_SUCCESS);
	}
}
*/
