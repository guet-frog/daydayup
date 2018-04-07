#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>


int main(void)
{
	printf("my pid = %d.\n", getpid());
	
	
	openlog("b.out", LOG_PID | LOG_CONS, LOG_USER);
	
	syslog(LOG_INFO, "this is my log info.%d", 23);
	
	
	syslog(LOG_INFO, "this is another log info.");
	syslog(LOG_INFO, "this is 3th log info.");
	
	closelog();
}