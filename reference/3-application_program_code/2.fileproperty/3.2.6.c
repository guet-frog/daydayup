#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


int main(int argc, char **argv)
{
	DIR *pDir = NULL;
	struct dirent * pEnt = NULL;
	unsigned int cnt = 0;
	
	if (argc != 2)
	{
		printf("usage: %s dirname\n", argv[0]);
		return -1;
	}
	
	pDir = opendir(argv[1]);
	if (NULL == pDir)
	{
		perror("opendir");
		return -1;
	}
	
	while (1)
	{
		pEnt = readdir(pDir);
		if(pEnt != NULL)
		{
			// 还有子文件，在此处理子文件
			printf("name：[%s]	,", pEnt->d_name);
			cnt++;
			if (pEnt->d_type == DT_REG)
			{
				printf("是普通文件\n");
			}
			else
			{
				printf("不是普通文件\n");
			}
		}
		else
		{
			break;
		}
	};
	printf("总文件数为：%d\n", cnt);
	
	
	return 0;
}












