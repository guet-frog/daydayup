#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>



int main(int argc, char *argv[])
{
	uint32_t i = 0;

	if (argc < 2)
	{
		printf("usage: ./a.out xxx\n");
		return 0;
	}

	/* test input param */
	printf("argc = %d\n", argc);
	for (i = 0; i < argc; i++)
	{
		printf("argv[%d] = %s\n", i, argv[i]);	
	}
	printf("\n");
	
	for (i = 1; i < argc; i++)
	{
		if (NULL == getenv(argv[i]))
		{
			printf("environ %s not found\n", argv[i]);
		}
		else
		{
			printf("[%s]: %s\n", argv[i], getenv(argv[i]));
		}
	}
	
	return 0;
}

