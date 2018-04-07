#include <stdio.h>
#include <stdlib.h>



int main(int argc, char **argv)
{
	int i = 0, val = 0;
	
/*	if (argc != 2)
	{
		printf("usage: %s num\n", argv[0]);
		return -1;
	}
*/	
	printf("RAND_MAX = %d.\n", RAND_MAX);		// 2147483647
	
	//srand(atoi(argv[1]));
	srand(time(NULL));
	for (i=0; i<6; i++)
	{
		val = rand();
		printf("%d ", (val % 6));
	}
	printf("\n");
	
	return 0;
}














