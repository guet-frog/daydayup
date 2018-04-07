#include <stdio.h>



int main(void)
{
	extern char **environ;		// 声明就能用
	int i = 0;
	
	while (NULL != environ[i])
	{
		printf("%s\n", environ[i]);
		i++;
	}
	
	
	return 0;
}




