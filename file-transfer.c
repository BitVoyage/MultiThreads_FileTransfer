#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#include"file-stream.h"
#include"file-transfer.h"
#include<pthread.h>
#include"dbtime.h"

#define THREAD_NUM 8

void *tcp_send_thread(void *a)
{	
	char *finish;
	struct thread_args *args = (struct thread_args *)a;
	int fd = args->fd;
	int size = args->size;
	int seek = args->seek;
	struct datagram sendData;
	int length = sizeof(sendData);
	int index = 0;
	FILE *filefd;
	filefd = fopen(args->filename, "r");
	fseek(filefd, seek*1024, SEEK_SET);
	send(fd, (char *)(&seek), 4, 0);
	while(index < size)
	{	
		if(fgets(sendData.buf, 1024, filefd) > 0)
		{
			sendData.index = index;
			//printf("send%d\n", index);
			send(fd, (char*)&sendData, length, 0);
			index++;
		}
		else
		{
			break;
		}
	}	
	sendData.index = -1;
	//printf("send -1\n");
	send(fd, (char *)(&sendData), length, 0);
	close(filefd);
	close(fd);	
}

void *tcp_recv_thread(void *a)
{
	struct thread_args *args = (struct thread_args *)a;
	int newfd = args->fd;
	int seek;
	struct datagram recvData;
	int length = sizeof(recvData);
	FILE *filefd;
	filefd = fopen("hi.txt", "r+");
	recv(newfd, (char*)&seek, 4, 0);
	fseek(filefd, seek*1024, SEEK_SET);
	for(;;)
	{
		recv(newfd, (char*)&recvData, length, 0);
		if(recvData.index < 0)
		{
			//printf("send finish\n");
			close(filefd);
			break;
		}
		else
		{
			//printf("recv:%d    %d\n", recvData.index,newfd);
			fputs(recvData.buf, filefd);
		}		
	}	
}

void tcp_file_send(int controlfd, int serverfd, char *filename)
{
	struct datagram sendData;
	struct sockaddr_in clientaddr;
	char *finish;
	int len = sizeof(clientaddr);
	int length = sizeof(sendData);
	int i;
	int size = (1073741824 / 1024 + 1) / THREAD_NUM + 1;
	pthread_t tid[10];
	
	for(i = 0; i<THREAD_NUM; i++)
	{
		listen(serverfd, 10);
		struct thread_args *args;
		args = (struct thread_args *)malloc(sizeof(struct thread_args));
		int newclientfd = accept(serverfd, (struct sockaddr*)&clientaddr, &len);
		args->filename = filename;
		args->seek = i * size;		
		args->size = size;
		args->fd = newclientfd;
		pthread_create(&(tid[i]), NULL, tcp_send_thread, (void *)args);		
	}
	
	i = 0;
	for(i = 0; i<THREAD_NUM; i++)
	{
		pthread_join(tid[i], NULL);
	}
	printf("finish\n");
	
}

void tcp_file_get(int fd)
{
	char *finish;
	struct datagram recvData;
	int length = sizeof(recvData);
	FILE *filefd;
	filefd = fopen("hi.txt", "w+");
	close(filefd);
	int i;
	pthread_t tid[10];
	dbtime_startTest ("timing");

	for(i = 0; i<THREAD_NUM; i++)
	{
		struct thread_args *args;
		args = (struct thread_args *)malloc(sizeof(struct thread_args));
		int newfd = open_client();
		args->fd = newfd;
		args->size = fd;
		pthread_create(&(tid[i]), NULL, tcp_recv_thread, (void *)args);
	}

	i = 0;
	for(i = 0; i<THREAD_NUM; i++)
	{
		pthread_join(tid[i], NULL);
	}
	dbtime_endAndShow ();
	printf("finish\n");
	close(fd);
}

