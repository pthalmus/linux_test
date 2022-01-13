//============================================================================
// Name        : linux_socket_test.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <vector>
#include "checking.h"
#include "player.h"
#include <cstring>

#define BUF_SIZE 256
#define MAX_CLNT 8
#define MAX_IP 30

void * handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);


int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;
int pop_Count =0;

char start_Msg[] = "game start";
char win_Msg[] = "win";
char lose_Msg[] = "lose";
char draw_Msg[] = "draw";

Player player[MAX_CLNT];



int main(int argc, char* argv[]) {
	int serv_sock;
	int clnt_sock;

	pthread_t t_id;

	struct tm *t;
	time_t timer = time(NULL);
	t=localtime(&timer);

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char messageSucces[] = "1\n";
	char messageFalse[] = "0\n";
	printf("Starting server\n");

	pthread_mutex_init(&mutx, NULL);

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) ==-1)
	{
		std::cout << "server can not binding!\n";
		exit(1);
	}
	if(listen(serv_sock, 5)==-1)
	{
		std::cout <<"server can not listening!\n";
		exit(1);
	}

	std::cout << "Waiting connection request\n";
	clnt_addr_size = sizeof(clnt_addr);
	while(1)
	{
		t=localtime(&timer);
		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);

		pthread_mutex_lock(&mutx);
		player[clnt_cnt] = Player();
		player[clnt_cnt].set_Player_Num(clnt_cnt);
		clnt_socks[clnt_cnt++]=clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);

		std::cout << " Connected client IP : " << inet_ntoa(clnt_addr.sin_addr);
		printf("(%d-%d-%d %d:%d)\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday,
		t->tm_hour, t->tm_min);
		printf(" chatter (%d/8)\n", clnt_cnt);
	}
	close(serv_sock);
	return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];
    char *ptr;
    char *context;
    static std::vector<int> pop_Item;


    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
    	if(msg[0] == 'c') //plane text
    	{
    		msg = strtok_r(NULL, "&", &context);
    		send_msg(msg, sizeof(msg));
    	}
    	else if(msg[0] == 's') // host start
    	{
    		send_msg(start_Msg, sizeof(start_Msg));
    	}
    	else if(msg[0] == 'i') // player send hand
		{
    		int check;
    		ptr = strtok_r(msg, "&", &context);
    		ptr = strtok_r(NULL, "&", &context);
    		check = atoi(ptr);
    		ptr = strtok_r(NULL, "&", &context);
    		while(ptr != NULL)
    		{
    			player[check].add(atoi(ptr));
    			ptr = strtok_r(NULL, "&", &context);
    		}
		}
    	else if(msg[0] == 'p') // player send card
    	{
    		int check=0;
    		int win=0;
    		pthread_mutex_lock(&mutx);
    		ptr = strtok_r(msg, "&", &context);
    		ptr = strtok_r(NULL, "&", &context);
    		check = atoi(ptr);
    		ptr = strtok_r(NULL, "&", &context);
    		player[check].pop(atoi(ptr));
    		pop_Item.push_back(atoi(ptr));
    		pop_Count++;
    		pthread_mutex_unlock(&mutx);
    		if(pop_Count == clnt_cnt)
    		{
    			win = calculrating(pop_Item);
    			if(win != 20)
    			{
    				pop_Count =0;
					for(int i=0; i<clnt_cnt; i++)
					{
						if(player[i].get_Last_Pop() == win)
						{
							player[i].up_Win_Stack();
							send_msg(win_Msg, sizeof(win_Msg));
						}
						else
						{
							send_msg(lose_Msg, sizeof(lose_Msg));
						}
					}
    			}
    			else
    			{
    				send_msg(draw_Msg, sizeof(draw_Msg));
    			}

    			std::vector<int>().swap(pop_Item);
    		}
    	}
    }


    // remove disconnected client
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
    {
        if (clnt_sock==clnt_socks[i])
        {
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
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
    int i;
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
    {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

