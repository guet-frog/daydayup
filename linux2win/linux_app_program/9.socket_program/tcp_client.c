#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>


#define SERVER_PORT			5505
#define SERVER_ADDR			"192.168.0.154"
#define MAX_CONNECT_NUM		3
#define	TCP_BUFF_SIZE		128


int main(void)
{	
	int sock_fd = -1, ret = -1, link_sock_fd = -1;
	struct sockaddr_in 	serverAddr = {0};
	//struct sockaddr 	clientAddr = {0};	// fill infor automatic
	//int clientAddrLen = 0;
	
// -- init sockaddr_in struct
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port	= htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
// -- create socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	{
		perror("socket");	// already has ':' && '\n'
		_exit(-1);
	}
	perror("socket");
	printf("sock_fd = %d\n", sock_fd);
	
// -- connect remote server addr
	ret = connect(sock_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));	// connect(fd, 服务器地址端口)
	if (ret < 0)																// 发起连接请求, 阻塞等待服务器应答 -- 发出SYN段
	{
		perror("connect");
		close(sock_fd);
		_exit(-1);
	}
	perror("connect");
	
	while(1);

	return 0;
}

// init sockaddr_in
// create socket
// connect
// send && receive

// warning: tcp握手相关操作(listen connect accept close)


