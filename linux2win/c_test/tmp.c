#include <stdio.h>

int f(void)
{
	//return (int *)123 != NULL;
	return NULL != NULL;
}

int main(void)
{
	printf("hello world\n");
	
	printf("ret = %d\n", f());
	
	return 0;
}