


#include <stdio.h>

int main(void)
{
	int a[7] = {1, 2, 3, 4, 5, 6, 7};
	int *t = a;
	int n = 0;
	
	for (n = 0; n<7; ++n)
	{
		*t++ = 999;
		printf("a = %p\n", a);
		printf("t = %p\n", t);
		
		// t++;
		
		// printf("t[%d] = %d\n", n, t[n]);
	}
	
	// printf("t[5] = %d\n", t[5]);
	// printf("t[6] = %d\n", t[6]);
	
	
	return 0;
}




/*
int main(void)
{
	char *a[4] = {0};
	int  *b[4] = {0};

	char **ptr = null;
	char **ptr_1 = NULL;
	
	printf("a    = %p\n", 		a);	
	printf("a+1  = %p\n", 		a+1);
	printf("&a   = %p\n", 		&a);
	printf("&a+1 = %p\n\n",	 	&a+1);
	
	printf("b    = %p\n", 		b);
	printf("b+1  = %p\n", 		b+1);
	printf("&b   = %p\n",		&b);
	printf("&b+1 = %p\n\n",		&b+1);
	
	printf("ptr   = %p\n", 		ptr);
	printf("ptr_1 = %p\n\n", 	ptr_1);
	
	return 0;
}

// a 	 = 0xbfccd6a8
// b 	 = 0xbfccd698
*/

