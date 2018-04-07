#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define NAME "1.txt"

int main(void)
{
	int ret = -1;
	
	ret = open(NAME, O_RDONLY);
	if (ret > 0)
	{
		printf("可读  	");
		close(ret);
		
	}
	else
	{
		perror("read");
	}
	

	ret = open(NAME, O_WRONLY);
	if (ret > 0)
	{
		printf("可写  	");
		close(ret);
	}
	else
		perror("write");
	
	return 0;
}









