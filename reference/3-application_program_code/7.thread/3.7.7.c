#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>



char buf[200] = {0};
pthread_mutex_t mutex;
pthread_cond_t cond;
unsigned int flag = 0;


// 子线程程序，作用是统计buf中的字符个数并打印
void *func(void *arg)
{
	// 子线程首先应该有个循环
	// 循环中阻塞在等待主线程激活的时候，子线程被激活后就去获取buf中的字符
	// 长度，然后打印；完成后再次被阻塞
	
	//while (strncmp(buf, "end", 3) != 0)
	//sleep(1);
	while (flag == 0)
	{	
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		printf("本次输入了%d个字符\n", strlen(buf));
		memset(buf, 0, sizeof(buf));
		pthread_mutex_unlock(&mutex);
		//sleep(1);
	}
	
	
	pthread_exit(NULL);
}


int main(void)
{
	int ret = -1;
	pthread_t th = -1;
	

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	
	ret = pthread_create(&th, NULL, func, NULL);
	if (ret != 0)
	{
		printf("pthread_create error.\n");
		exit(-1);
	}
	
	printf("输入一个字符串，以回车结束\n");
	while (1)
	{
		//pthread_mutex_lock(&mutex);
		scanf("%s", buf);
		pthread_cond_signal(&cond);
		//pthread_mutex_unlock(&mutex);
		// 去比较用户输入的是不是end，如果是则退出，如果不是则继续		
		if (!strncmp(buf, "end", 3))
		{
			printf("程序结束\n");
			flag = 1;
			
			//exit(0);
			break;
		}
		//sleep(1);
		// 主线程在收到用户收入的字符串，并且确认不是end后
		// 就去发信号激活子线程来计数。
		// 子线程被阻塞，主线程可以激活，这就是线程的同步问题。
		// 信号量就可以用来实现这个线程同步
	}

	
	// 回收子线程
	printf("等待回收子线程\n");
	ret = pthread_join(th, NULL);
	if (ret != 0)
	{
		printf("pthread_join error.\n");
		exit(-1);
	}
	printf("子线程回收成功\n");
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	
	return 0;
}



