
// unsigned char a[x][x] 与 unsigned char (*b)[x]	-- a b 都是 unsigned char (*)[x]

// unsigned char **c 与 unsigned char *d[x]			-- c d 都是 unsigned char **

// unsigned char ** (+4) 与 unsigned int ** (+4)
// unsigned char *  (+1) 与 unsigned int *  (+4)

// (1) 二维数组名: 一维数组类型(not基本类型) -- 二维数组内存中是线性排列, 不需要等效
// (2) 二重指针: 指针 数组

// ①二重指针 -> 指针数组(type: 指针类型) 	②二维数组 -> 一维数组类型

#include <stdio.h>

int main(void)
{
	unsigned char a[2][5] = {0};
	unsigned char (*b)[5] = NULL;
	unsigned char **c = NULL;
	
	unsigned char *d[5] = {0};
	
	unsigned char *e = (unsigned char *)d;
	
	unsigned int *f = (unsigned int *)d;
	
	//b = a;  	// it's ok
	//c = a;	// warning: assignment from incompatible pointer type
	
	b = a;
	//c = b;	// warning: assignment from incompatible pointer type
	c = d;
	
	printf("a     = %p\n", a);
	printf("a + 1 = %p\n", a + 1);	// 5
	
	printf("b     = %p\n", b);
	printf("b + 1 = %p\n", b + 1);	// 5
	
	printf("c     = %p\n", c);		// 4
	printf("c + 1 = %p\n", c + 1);
	
	printf("d     = %p\n", d);		// 4  d[0] -- unsigned char *		&d[0] = unsigned char **
	printf("d + 1 = %p\n", d + 1);
	
	printf("e     = %p\n", e);		// 1
	printf("e + 1 = %p\n", e + 1);
	
	printf("f     = %p\n", f);		// 4
	printf("f + 1 = %p\n", f + 1);
	
	return 0;
}

// a     = 0xbfced642
// a + 1 = 0xbfced647	5
// b     = 0xbfced642
// b + 1 = 0xbfced647	5
// c     = 0xbfced64c
// c + 1 = 0xbfced650	4
// d     = 0xbfced64c
// d + 1 = 0xbfced650	4
// e     = 0xbfced64c
// e + 1 = 0xbfced64d	1
// f     = 0xbfced64c
// f + 1 = 0xbfced650	4



