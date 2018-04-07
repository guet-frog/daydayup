#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void func1(void)
{
	printf("func1\n");
}

void func2(void)
{
	printf("func2\n");
}


int main(void)
{
	printf("hello world.\n");
	
	// 当进程被正常终止时，系统会自动调用这里注册的func1执行
	atexit(func2);
	atexit(func1);
	
	printf("my name is lilei hanmeimei\n");
	
	
	//return 0;
	//exit(0);
	_exit(0);
}







