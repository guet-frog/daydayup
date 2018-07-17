#include <stdio.h>

int main(void)
{
	char a[5] = {0};
	char b[2][5] = {0};
	
	printf("a   = %p\n", a);
	printf("a+1 = %p\n", a+1);	// 1
	
	printf("&a  = %p\n", &a);
	printf("&a+1= %p\n", &a+1);	// 5
	
	printf("b   = %p\n", b);
	printf("b+1 = %p\n", b+1);	// warning: 5
	
	printf("&b  = %p\n", &b);
	printf("&b+1= %p\n", &b+1);	// 0xA
	
	printf("\n\n");
	
	printf("b[0]   		= %p\n", b[0]);
	printf("b[0]+1   	= %p\n", b[0]+1);	// warning
	
	printf("&b[0]   	= %p\n", &b[0]);
	printf("&b[0]+1   	= %p\n", &b[0]+1);	// warning
	
	printf("&b[0][0]   	= %p\n", &b[0][0]);
	printf("&b[0][0]+1 	= %p\n", &b[0][0]+1);	// 1
	
	return 0;
}

a   = 0xbfa9e847
a+1 = 0xbfa9e848
&a  = 0xbfa9e847
&a+1= 0xbfa9e84c

&b  = 0xbfa9e83d	// 二维数组指针类型
&b+1= 0xbfa9e847

b   = 0xbfa9e83d	// warning: 二维数组名 -- 一维数组指针类型(非普通类型)
b+1 = 0xbfa9e842

b[0]   		= 0xbfa9e83d	// 等效为 -- 一维数组名(普通类型)
b[0]+1   	= 0xbfa9e83e

&b[0]   	= 0xbfa9e83d	// 等效为 -- 一维数组名取地址(非普通类型)
&b[0]+1   	= 0xbfa9e842

&b[0][0]   	= 0xbfa9e83d	// 普通类型
&b[0][0]+1 	= 0xbfa9e83e












