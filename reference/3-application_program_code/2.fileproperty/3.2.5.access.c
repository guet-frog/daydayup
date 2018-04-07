#include <stdio.h>
#include <unistd.h>


#define NAME 	"3.txt"

int main(void)
{
	int ret = -1;
	
	
	ret = access(NAME, F_OK);
	if (ret < 0)
	{
		printf("文件不存在 \n");
		return -1;
	}
	else
	{
		printf("文件存在	");
	}
	
	ret = access(NAME, R_OK);
	if (ret < 0)
	{
		printf("不可读 ");
	}
	else
	{
		printf("可读 ");
	}

	ret = access(NAME, W_OK);
	if (ret < 0)
	{
		printf("不可写 ");
	}
	else
	{
		printf("可写 ");
	}

	ret = access(NAME, X_OK);
	if (ret < 0)
	{
		printf("不可执行 \n");
	}
	else
	{
		printf("可执行 \n");
	}	
	
	
	
	return 0;
}











