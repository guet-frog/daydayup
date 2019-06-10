#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>



#define ENABLE_RW			(0666)
#define TMP_WRITE_BUFF		"ddddd"
#define TMP_BUFF_SIZE		20
#define READ_BYTE_NUM		30

char gTmpBuff[TMP_BUFF_SIZE] = {0};

void fun(void)
{
	printf("in fun\n");
	return;
	//exit(-1);
	//_exit(-1);
}


int main(int argc, char *argv[])
{
	int fd = -1, ret = -1, fd1 = -1;

	fd = open("./test.txt", O_RDWR | O_CREAT | O_APPEND | O_TRUNC, ENABLE_RW);
	
	if (fd < 0)
	{
		perror("func@open():");
	}
	
	if (argc > 1)
	{
		static uint32_t tmp_cnt = 0;
		uint8_t i = 0;
		
		for (i = 1; i < argc; i++)
		{
			ret = write(fd, argv[i], strlen((const char *)argv[i]));
			if (ret < 0)
			{
				perror("func@write():");
			}
			
			tmp_cnt += strlen((const char *)argv[i]);
		}
		
		printf("write %d byte\n", tmp_cnt);
		tmp_cnt = 0;
	}
	else
	{
		ret = write(fd, TMP_WRITE_BUFF, strlen((const char *)TMP_WRITE_BUFF));
		if (ret < 0)
		{
			perror("func@write():");
		}
		else
		{
			printf("write %d bytes\n", ret);	
		}
	}
	
	/* - set file point */
	lseek(fd, SEEK_SET, 0);
	ret = read(fd, gTmpBuff, READ_BYTE_NUM);
	if (ret < 0)
	{
		perror("func@read():");
		_exit(-1);
	}
	else
	{
		printf("read %d byte: %s\n", ret, gTmpBuff);
	}
	
	lseek(fd, SEEK_SET, 0);
	ret = read(fd, gTmpBuff, READ_BYTE_NUM);
	if (ret < 0)
	{
		perror("func@read():");
		_exit(-1);
	}
	else
	{
		printf("read %d byte: %s\n", ret, gTmpBuff);
	}
	
	ret = close(fd);
	perror("func@close()");
	
	fun();

	printf("in main\n");
	
	return 0;
}
