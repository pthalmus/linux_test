/*
 * foward_test.cpp
 *
 *  Created on: 2022. 1. 18.
 *      Author: s9342931
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<pthread.h>

#define BUFSIZE 256
#define MAX_CLNT 20

void error_handling(char *message);
void * handle_clnt(void *arg);
void send_msg(char *msg, int len);

char error_sock[] = "socket() error";
char error_bind[] = "bind() error";
char error_listen[] = "listen() error";
char error_accept[] = "accept() error";
char check_message[] = "connect";

pthread_mutex_t mutx;
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];


int main(int argc, char **argv)
{
	int serv_sock;
	int clnt_sock;
	char message[BUFSIZE];
	int str_len;

	pthread_mutex_init(&mutx, NULL);
	pthread_t t_id;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	unsigned int clnt_addr_size;

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[1]);
		exit(1);
	}

	printf("starting server : in %s port\n", argv[1]);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);    /* 서버 소켓 생성 */

	if(serv_sock == -1)
	{
		error_handling(error_sock);
	}
	printf("Successfully create server socket\n");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));


	if( bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr) )==-1)
	{
		error_handling(error_bind);
	}

	if(listen(serv_sock, 5) == -1)
	{
		error_handling(error_listen);
	}

	printf("Successful binding\n");
	printf("Wating client\n");

	while(1)
	{
		pthread_mutex_lock(&mutx);

		clnt_addr_size = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
		{
			error_handling(error_accept);
		}

		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
	}

	close(serv_sock);	/* 연결 종료 */
	return 0;
}


void *handle_clnt(void *arg)
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUFSIZE];
    memset(&msg, 0, sizeof(msg));

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
		send_msg(msg, str_len);
		memset(&msg, 0, str_len);
    }

    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
    {
    	if (clnt_sock==clnt_socks[i])
    	{
    		while(i<clnt_cnt-1)
    		{
    			clnt_socks[i]=clnt_socks[i+1];
    			i++;
    		}
    		break;
    	}
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char* msg, int len)
{
	printf("%s  , %d\n", msg, len);
	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt; i++)
	{
		write(clnt_socks[i], msg, len);
	}

	pthread_mutex_unlock(&mutx);
}



void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}



