#include <stdio.h>
#include <time.h>
#include <string.h>



int main(void)
{
	time_t tNow = -1;
	struct tm tmNow;
	
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
	
	// gmtime 和localtime
	memset(&tmNow, 0, sizeof(tmNow));
	gmtime_r(&tNow, &tmNow);
	printf("年%d月%d日%d时%d.\n", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday, tmNow.tm_hour);
	
	memset(&tmNow, 0, sizeof(tmNow));
	localtime_r(&tNow, &tmNow);
	printf("年%d月%d日%d时%d.\n", tmNow.tm_year, tmNow.tm_mon, tmNow.tm_mday, tmNow.tm_hour);
	
	
	return 0;
}







