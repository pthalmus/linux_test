#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>
#include<player.h>


#define BUF_SIZE 100
#define MAX_CLNT 100
#define MAX_IP 30
#define MAX_ROOM 50
#define LOBBY 0

void * handle_clnt(void *arg);
void send_msg(char *msg, int len, int room_Num);
void error_handling(char *msg);



int clnt_cnt[MAX_ROOM]=0;
int clnt_socks[MAX_ROOM][MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_addr;
    socklen_t clnt_addr_sz;
    pthread_t t_id;

    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);

    if (argc != 2)
    {
        printf(" Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
    {
    	error_handling("bind() error");
    }

    if (listen(serv_sock, 5)==-1)
    {
    	error_handling("listen() error");
    }

    clnt_addr_sz= sizeof(clnt_addr);
    while(1)
    {
        t=localtime(&timer);
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_sz);

        pthread_mutex_lock(&mutx);
        clnt_socks[LOBBY][clnt_cnt[LOBBY]++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);
        printf(" Connected client IP : %s ", inet_ntoa(clnt_addr.sin_addr));
        printf("(%d-%d-%d %d:%d)\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min);
        printf(" chatter (%d/100)\n", clnt_cnt[LOBBY]);
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];

    Player player = Player();

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
        send_msg(msg, str_len, player.get_Room_Num());
    }

    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
    {
    	if (clnt_sock==clnt_socks[player.get_Room_Num][i])
    	{
    		while(i<clnt_cnt[player.get_Room_Num]-1)
    		{
    			clnt_socks[player.get_Room_Num][i]=clnt_socks[player.get_Room_Num][i+1];
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

void send_msg(char* msg, int len, int room_Num)
{
    int i;
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
        write(clnt_socks[room_Num][i], msg, len);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

