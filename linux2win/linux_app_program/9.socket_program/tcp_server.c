#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define SERVER_PORT		5505
#define SERVER_ADDR		"192.168.0.154"


int main(void)
{	
	int sock_fd = -1, ret = -1;
	struct sockaddr_in serverAddr = {0};	
	
	// init sockaddr_in struct
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port	= htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	//serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	// create socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);	// domain：selects protocol family which will be used for communication -- AF_INET
	if (fd < 0)
	{
		perror("socket  ");
		_exit(-1);
	}
	perror("socket  ");
	printf("sock_fd = %d\n", sock_fd);
	
	// bind local addr
	ret = bind(sock_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		perror("bind ");
		_exit(-1);
	}
	perror("bind  ");
	
	
	
	return 0;
}

// bind

// listen

// accept
