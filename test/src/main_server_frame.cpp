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
#include<main_Room.h>


#define BUF_SIZE 1000
#define MAX_CLNT 8
#define MAX_IP 30
#define MAX_ROOM 50
#define LOBBY 0

void * handle_clnt(void *arg);
void send_msg(char *msg, int len, Player player);
void error_handling(char *msg);



int clnt_cnt[MAX_ROOM]=0;
int clnt_socks[MAX_ROOM][MAX_CLNT];
pthread_mutex_t mutx;

main_Room Main_Room;
std::vector<std::vector<int>> vector_2d(MAX_ROOM, std::vector<int> (MAX_CLNT, 0));

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

    char *ptr;
    char *context;
    Player player = Player();

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
    	switch(msg[0])
		{
		case 'c': // plane chat
			msg = strtok_r(NULL, "&", &context);
			send_msg(msg, str_len, player);
			break;
		case 'f': // associate with friends
			break;
		case 'r': // get room details
			std::vector<char> detail_Msg;
			for(int i=0; i<MAX_ROOM; i++)
			{
				detail_Msg.push_back(Main_Room.show_Room_Detail(i));
			}
			send_Vector(detail_Msg, sizeof(detail_Msg), player);
			break;
		case 'm': // make room
			Main_Room.create_Sub_Room(msg);
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
			clnt_cnt[player.get_Room_Num]--;
			pthread_mutex_unlock(&mutx);
			ptr = strtok_r(msg, "&", &context);
			ptr = strtok_r(NULL, "&", &context);
			player.set_Room_Num(atoi(ptr));
			clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
			char enter_Msg[100];
			sprintf(enter_Msg, "%s enter room %d", player.get_Player_Name(), player.get_Room_Num());
			send_msg(enter_Msg, sizeof(enter_Msg), player);
			break;

		case 'e': // enter room
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
			clnt_cnt[player.get_Room_Num]--;
			pthread_mutex_unlock(&mutx);
			ptr = strtok_r(msg, "&", &context);
			ptr = strtok_r(NULL, "&", &context);
			player.set_Room_Num(atoi(ptr));
			clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
			char enter_Msg[100];
			sprintf(enter_Msg, "%s enter room %d", player.get_Player_Name(), player.get_Room_Num());
			send_msg(enter_Msg, sizeof(enter_Msg), player);
			break;

		case 'o': // out room
			char enter_Msg[100];
			sprintf(enter_Msg, "%s out room %d", player.get_Player_Name(), player.get_Room_Num());
			send_msg(enter_Msg, sizeof(enter_Msg), player);

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
			clnt_cnt[player.get_Room_Num]--;
			pthread_mutex_unlock(&mutx);

			player.set_Room_Num(LOBBY);
			pthread_mutex_lock(&mutx);
			clnt_socks[LOBBY][clnt_cnt[LOBBY]++]=clnt_sock;
			pthread_mutex_unlock(&mutx);
			break;

		case 'v': // invite friends
			break;
		case 'b': // block room capacity

			break;
		case 't': // add BOT player
			break;
		case 's': // game start
			break;
		case 'h': // submit hand
			break;
		case 'p': // pop out card
			break;
		default: // error code
			break;
		}
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
    clnt_cnt[player.get_Room_Num]--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char* msg, int len, Player player)
{

	int i;

	pthread_mutex_lock(&mutx);
	for (i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		write(clnt_socks[player.get_Room_Num()][i], msg, len);
	}
	pthread_mutex_unlock(&mutx);

}

void send_Vector(std::vector<char> msg, int len, Player player)
{
	int i;

	pthread_mutex_lock(&mutx);
	for (i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		write(clnt_socks[player.get_Room_Num()][i], (const void*)&msg, len);
	}
	pthread_mutex_unlock(&mutx);
}

void

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}


