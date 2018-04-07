#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>


int main(void)
{
	time_t tNow = -1;
	struct tm tmNow;
	char buf[100];
	struct timeval tv = {0};
	struct timezone tz = {0};
	int ret = -1;
	
	// time
	//tNow = time(NULL);		// 返回值
	time(&tNow);				// 指针做输出型参数
	if (tNow < 0)
	{
		perror("time");
		return -1;
	}
	printf("time: %ld.\n", tNow);
	
	// ctime
	printf("ctime: %s.\n", ctime(&tNow));
#if 0	
	// gmtime 和localtime
	memset(&tmNow, 0, sizeof(tmNow));
	gmtime_r(&tNow, &tmNow);
	printf("年%d月%d日%d时%d.\n", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday, tmNow.tm_hour);
	
	memset(&tmNow, 0, sizeof(tmNow));
	localtime_r(&tNow, &tmNow);
	printf("年%d月%d日%d时%d.\n", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday, tmNow.tm_hour);
#endif

#if 0
	// asctime
	memset(&tmNow, 0, sizeof(tmNow));
	localtime_r(&tNow, &tmNow);
	printf("年%d月%d日%d时%d.\n", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday, tmNow.tm_hour);
	printf("asctime:%s.\n", asctime(&tmNow));
#endif

#if 0
	// strftime
	memset(&tmNow, 0, sizeof(tmNow));
	localtime_r(&tNow, &tmNow);
	printf("年%d月%d日%d时%d.\n", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday, tmNow.tm_hour);
	
	memset(buf, 0, sizeof(buf));
	strftime(buf, sizeof(buf), "%Y * %m * %d, %H-%M-%S.", &tmNow);
	printf("时间为：[%s].\n", buf);
#endif

	// gettimeofday
	ret = gettimeofday(&tv, &tz);
	if (ret < 0)
	{
		perror("gettimeofday");
		return -1;
	}
	printf("seconde: %ld.\n", tv.tv_sec);
	printf("timezone:%d.\n", tz.tz_minuteswest);
	
	
	return 0;
}







