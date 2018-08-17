
// 验证 二重指针类型 与 二维数组名类型 之间关系  -- line19 二维数组名作为左值和右值意义不同
// 验证 二重指针自增地址偏移

#include <stdio.h>

typedef unsigned char (*array5_t)[5];

int main(void)
{
	unsigned char a[2][5] = {0};
	
	unsigned char var;
	unsigned char *p = &var;
	
	unsigned char **b = NULL;
	b = &p;
	
	//a = a + 1;	// error: incompatible types when assigning to type ‘unsigned char[2][5]’ from type ‘unsigned char (*)[5]
	//b = a;		// warning: assignment from incompatible pointer type [enabled by default]
	
	//unsigned char (*)[5] c = a;	// expected identifier or ‘(’ before ‘)’ token unsigned char (*)[5] c = a
	//unsigned char (*c)[5] = a;	// it's ok
	
	array5_t c = a;
	
	printf("a     = %p\n", a);
	printf("a + 1 = %p\n", a + 1);
	
	printf("b     = %p\n", b);
	printf("b + 1 = %p\n", b + 1);
	
	printf("c     = %p\n", c);
	printf("c + 1 = %p\n", c + 1);
	
	return 0;
}


// a     = 0xbff817c6
// a + 1 = 0xbff817cb
// b     = 0xbff817b8
// b + 1 = 0xbff817bc
// c     = 0xbff817c6
// c + 1 = 0xbff817cb