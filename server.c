#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>
#include <time.h>
#include "dbtime.h"

#define SERVER_PORT 1555

struct controlgram
{
	int expected;
	int acget;
};

struct datagram
{
	int index;
	char buf[1024];
};

int serverfd;
int controlfd;
static int id = 1;
struct sockaddr_in servaddr;
struct sockaddr_in clientaddr;
struct datagram recvData;
int len = sizeof(clientaddr);
int length = sizeof(struct datagram);
int reSend;
char *indexBuf;
FILE *filefd;
FILE *cfd;

void *reset()
{
	cfd = fopen("hello.txt", "r");
	struct datagram reData;
	struct controlgram cg;
	int newclientfd = accept(controlfd, (struct sockaddr*)&clientaddr, 
		&len);
	if(newclientfd < 0){printf("listen fail\n");exit(1);}

	
	while(1)
	{
		//判断客户端是否还活着
		recv(newclientfd, (char *)(&cg), sizeof(cg), 0);
		for(reSend = cg.expected; reSend < cg.acget; reSend++)
		{
			fseek(cfd, reSend * 1024, SEEK_SET);
			fgets(reData.buf, 1024, cfd);
			reData.index = reSend;	
			send(newclientfd, (char *)(&reData), length, 0);
			printf("resend%d\n", reSend);
		}		
	}
}

void main()
{
	int n;
	int index = 0;
	char recvBuf[1024]="\0";
	struct datagram sendData;
	reSend = 0;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverfd = socket(PF_INET, SOCK_DGRAM, 0);
	controlfd = socket(PF_INET, SOCK_STREAM, 0);

	if(bind(controlfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in))<0)
	{
		printf("control bind fail\n");
		exit(1);
	}

	listen(controlfd, 1);
	sleep(1);
	
	//控制线程
	pthread_t tid;
	pthread_create(&tid, NULL, reset, NULL);

	if(bind(serverfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in))<0)
	{
		printf("servre bind fail\n");
		exit(1);
	}

	
	n = recvfrom(serverfd, recvBuf, length, 0, (struct sockaddr*)&clientaddr, &len);
	if(n < 0)
	{
		printf("recv fail");
	}
	printf("%s\n", recvBuf);
	filefd = fopen("hello.txt", "r");
	
	while(fgets(sendData.buf, 1024, filefd) > 0)
	{	
		sendData.index = index;
		index++;
		//printf("send%d\n", index);
		sendto(serverfd, (char *)(&sendData), length, 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
	}
	printf("send finish\n");
}



