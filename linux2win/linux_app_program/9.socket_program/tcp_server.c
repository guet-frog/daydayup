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
	int listen_fd = -1, ret = -1, connect_fd = -1;
	struct sockaddr_in 	serverAddr = {0};
	struct sockaddr 	clientAddr = {0};	// fill infor automatic
	int clientAddrLen = 0;
	
// -- init sockaddr_in struct
	memset(&serverAddr, 0, sizeof(serverAddr));
	//serverAddr.sin_len = xxx;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port	= htons(SERVER_PORT);
	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
// -- create socket
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);	// domain：selects protocol family which will be used for communication -- AF_INET
	if (listen_fd < 0)
	{
		perror("socket");	// already has ':' && '\n'
		_exit(-1);
	}
	perror("socket");
	printf("listen_fd = %d\n", listen_fd);
	
// -- bind local addr
	ret = bind(listen_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (ret < 0)
	{
		perror("bind");
		close(listen_fd);
		_exit(-1);
	}
	perror("bind");
	
// -- listen
	ret = listen(listen_fd, MAX_CONNECT_NUM);	// socket referred to by sockfd(形参) as a passive socket
	if (ret < 0)								// listen_fd成为一个监听描述符
	{
		perror("listen");
		close(listen_fd);
		_exit(-1);
	}
	perror("listen");
	
	//while(1); // connect OK, multiple connect OK -- 2018-5-2 20:27:19
	
// -- accept
	connect_fd = accept(listen_fd, &clientAddr, &clientAddrLen);	// extracts(抽取) the first connection request
	if (connect_fd < 0)												// connect_fd应该与client_addr已经绑定
	{																// 阻塞等待客户端连接
		perror("accept");											// accept return新的描述符connect_fd和客户端通信
		close(listen_fd);
		_exit(-1);
	}
	printf("connect_fd = %d\n", connect_fd);
	
// -- recv && send
	uint8_t tcp_buff[TCP_BUFF_SIZE] = {0};
	while(1)
	{
		//ret = send(connect_fd, "hello world", sizeof("hello world"), 0);	// sizeof("字符串") OK; ret = 12
		//ret = send(connect_fd, "hello world", strlen("hello world"), 0);	// strlen("字符串") OK; ret = 11
		
		memset(tcp_buff, 0, sizeof(tcp_buff));	// need reflect: use buff(memory) -> clean
		
		ret = recv(connect_fd, tcp_buff, TCP_BUFF_SIZE, 0);	// len is buff max size -- overflow?
		printf("============in block just now============\n");
		if (ret < 0)
		{
			perror("send");
			close(connect_fd);
			close(listen_fd);
		}
		else if (0 == ret)
		{
			close(connect_fd);		// warning: important
			printf("disconnect\n");
			connect_fd = accept(listen_fd, &clientAddr, &clientAddrLen);
			printf("============in block just now============\n");
			printf("connect\n");
			printf("connect_fd = %d\n", connect_fd);
			if (connect_fd < 0)
			{
				perror("accept");
				close(listen_fd);
				_exit(-1);
			}
		}
		else
		{
			printf("receive num   = %d\n", ret);
			printf("receive infor = %s\n", tcp_buff);
			
			//ret = send(connect_fd, tcp_buff, TCP_BUFF_SIZE, 0);		// send num always: 128
			ret = send(connect_fd, tcp_buff, strlen(tcp_buff), 0);	// len is actual size
			if (ret < 0)
			{
				perror("send");
			}
			else
			{
				printf("send num   = %d\n", ret);
				printf("send infor = %s\n", tcp_buff);
			}
		}
	}
	
	return 0;
}
// init sockaddr_in
// create socket
// bind
// listen
// accept
// receive
// send		-- intended recipient is known	预期的接收者

/* connect: 发送SYN段
 * close  : 发送FIN段 
 * read   : 返回0, 代表收到FIN段 
 */
 
/*
 *	accept调试记录
 */
// int tmp_sock_fd = 0;
// while(1)
// {
	// printf("has sleep - 1 \n");
	// connect_fd = accept(listen_fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	// printf("has sleep - 2\n");
	
	// if (tmp_sock_fd != connect_fd)
	// {
		// tmp_sock_fd = connect_fd;
		// printf("connect_fd = %d\n", connect_fd);
		// printf("clientAddr = %s, clientPort = %d\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);	// inet_ntoa
	// }
// }

// MAX_CONNECT_NUM 并不是最大链接数
// while(1); 仍然可以connect成功, 需要进一步测试accept

// clientAddr = 192.168.0.129, clientPort = 64565	// warning: close(connect_fd) && inet_ntoa();
// connect_fd = 16
// clientAddr = 192.168.0.129, clientPort = 9526	// client ip is same, client port is random
// connect_fd = 17
// clientAddr = 192.168.0.129, clientPort = 11062
// connect_fd = 18
// clientAddr = 192.168.0.129, clientPort = 18742

// clientAddrLen 			= 16
// sizeof(struct sockaddr) 	= 16




