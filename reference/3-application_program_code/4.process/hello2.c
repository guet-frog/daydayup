#include <stdio.h>

// env就是我们给main函数额外传递的环境变量字符串数组
int main(int argc, char **argv, char **env)
{
	int i = 0;
	
	printf("argc = %d.\n", argc);
	
	while (NULL != argv[i])
	{
		printf("argv[%d] = %s\n", i, argv[i]);
		i++;
	}
	
	i = 0;
	while (NULL != env[i])
	{
		printf("env[%d] = %s\n", i, env[i]);
		i++;
	}
	
	
	return 0;
}

