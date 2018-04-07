#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>


int main(void)
{
	openlog("my_demon", LOG_CONS, LOG_DAEMON);
	syslog(LOG_NOTICE, "this demon is runing\n");
	printf("this demon");
	closelog();
	
	
	return 0;
}
