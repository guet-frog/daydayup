#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <linux/input.h>

#define KEY_BOARD_DEV	"/dev/input/event0"
#define MOUSE_DEV		"/dev/input/event3"



struct input_event input_event_buf;

int main(void)
{
	int ret = -1, fd = -1;
	unsigned int cnt = 0;
	
	fd = open(KEY_BOARD_DEV, O_RDONLY);
	//fd = open(MOUSE_DEV, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		exit(-1);
	}
	
	while (1)
	{
		ret = read(fd, &input_event_buf, sizeof(struct input_event));
		if (ret < 0)
		{
			perror("read");
			exit(-1);
		}
		else
		{
			printf("------------------%d--------------------\n", cnt);
			printf("read %d bytes\n", ret);
			printf("time: [%d]sec-[%d]usec\n", (int)input_event_buf.time.tv_sec, (int)input_event_buf.time.tv_usec);
			printf("type: %d\n", input_event_buf.type);
			printf("code: %d\n", input_event_buf.code);
			printf("value: %d\n", input_event_buf.value);
			printf("\n");
			
			cnt += 1;
		}
	}

	close(fd);
	
	return 0;
}