#include <stdio.h>
#include <unistd.h>			// unix standand
#include <signal.h>



void func(int sig)
{
	/*
	if (sig == SIGALRM)
	{
		printf("alarm happened.\n");
	}
	*/
}

void mysleep(unsigned int seconds);


int main(void)
{
	printf("before mysleep.\n");
	mysleep(3);
	printf("after mysleep.\n");
	
	
/*	unsigned int ret = -1;
	struct sigaction act = {0};
	
	act.sa_handler = func;
	sigaction(SIGALRM, &act, NULL);
	
	//signal(SIGALRM, func);
	ret = alarm(5);
	printf("1st, ret = %d.\n", ret);


	sleep(3);
	
	ret = alarm(5);		// 返回值是2但是本次alarm会重新定5s
	printf("2st, ret = %d.\n", ret);
	sleep(1);
	
	ret = alarm(5);
	printf("3st, ret = %d.\n", ret);
	
	
	
	
	//while (1);
	pause();		
*/	
	return 0;
}

void mysleep(unsigned int seconds)
{
	struct sigaction act = {0};
	
	act.sa_handler = func;
	sigaction(SIGALRM, &act, NULL);
	
	alarm(seconds);
	pause();
}



