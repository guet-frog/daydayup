#include <stdio.h>		// standard input output
#include <stdlib.h>
#include <string.h>


#define FILENAME	"1.txt"

int main(void)
{
	FILE *fp = NULL;
	size_t len = -1;
	//int array[10] = {1, 2, 3, 4, 5};
	char buf[100] = {0};
	
	fp = fopen(FILENAME, "r+");
	if (NULL == fp)
	{
		perror("fopen");
		exit(-1);
	}
	printf("fopen success. fp = %p.\n", fp);
	
	// 在这里去读写文件
	memset(buf, 0, sizeof(buf));
	len = fread(buf, 1, 10, fp);
	printf("len = %d.\n", len);
	printf("buf is: [%s].\n", buf);

#if 0	
	fp = fopen(FILENAME, "w+");
	if (NULL == fp)
	{
		perror("fopen");
		exit(-1);
	}
	printf("fopen success. fp = %p.\n", fp);
	
	// 在这里去读写文件
	//len = fwrite("abcde", 1, 5, fp);
	//len = fwrite(array, sizeof(int), sizeof(array)/sizeof(array[0]), fp);
	len = fwrite(array, 4, 10, fp);
	printf("len = %d.\n", len);
#endif	
	
	fclose(fp);
	return 0;
}






