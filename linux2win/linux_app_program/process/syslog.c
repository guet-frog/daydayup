#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>



int main(int argc, char *argv[])
{
	uint32_t cnt = 0;
	pid_t pid = -1;
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if (0 == pid)	/* code executed by child */
	{
		/* open syslog */
		openlog("syslog_demo", LOG_PID, LOG_DAEMON);
		
		/* 2, 子进程脱离原会话，进程组，控制终端 */
		pid = setsid();
		if (pid < 0)
		{
			perror("setsid");
			exit(EXIT_FAILURE);
		}
		else
		{
			//printf("new session id = %d\n", pid);
			syslog(LOG_DEBUG, "new session id = %d\n", pid);
		}
		
		/* 3, 更改当前工作目录 */
		chdir("/");
		
		/* 4, 修改默认文件权限 */
		umask(0);
		
		/* 5, 关闭所有文件描述符 */
		uint32_t fd_num = getdtablesize();
		uint32_t i = 0;
		printf("fd_num = %d\n", fd_num);
		
		for (i = 0; i < fd_num; i++)
		{
			close(i);
		}
		
		/* 6, 文件描述符0 1 2定位到/dev/null */
		
		while (1)
		{
			int fd = -1;
			int write_num = -1;
			char *write_buf = "hello world\n";
			
			//fd = open("/tmp/daemon", O_RDWR | O_CREAT | O_TRUNC, 0600);
			fd = open("/tmp/daemon.log", O_RDWR | O_APPEND, 0600);
			if (fd < 0)
			{
				//perror("open");
				syslog(LOG_ERR, "open");
				exit(-1);
			}
			
			printf("strlen(write_buf) = %d\n", strlen(write_buf));
			
			write(fd, write_buf, strlen(write_buf));
			
			close(fd);
			
			sleep(1);
		}
		
		closelog();
	}
	else
	{
		/* 1, 父进程先与子进程退出 */
		exit(0);		/* process parent exit */
	}
	
	return 0;
}