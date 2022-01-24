#include<iostream>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>



#include"player.h"
#include"main_Room.h"
#include"mysql_Injection.h"

#define JSON_IS_AMALGAMATION

#define BUF_SIZE 128
#define MAX_CLNT 8
#define MAX_IP 30
#define MAX_ROOM 40
#define LOBBY 0

void error_handling(char *msg);
void * handle_clnt(void *arg);
void error_Mysql(char *msg);
void send_msg(char *msg, int len, Player player);
void send_Json(Json::Value root, Player player);
void send_Json(Json::Value root, int clnt_sock);
void send_reg(char* msg, int len, int clnt_sock);



int clnt_cnt[MAX_ROOM];
int clnt_socks[MAX_ROOM][MAX_CLNT];
pthread_mutex_t mutx;
main_Room Main_Room;

MYSQL *mysql;
MYSQL *MYSQL_Connection = NULL;


char error_sock[] = "socket() error";
char error_bind[] = "bind() error";
char error_listen[] = "listen() error";
char error_accept[] = "accept() error";
char check_message[] = "connect";
char game_start[] = "game start!!";
char unknown_Error[] = "Unknown Error Code!!";
char error_room[] ="cannot access to room";
char reg_succes[] = "true";

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

	std::string hostName = "127.0.0.1";
	std::string userId = "root";
	std::string password = "kiiamm12";
	std::string DB = "GAMEDB";

	int mysql_status = 0;
	MYSQL_RES *mysql_res = NULL;

	MYSQL_ROW mysqlRow;
	MYSQL_FIELD *mysqlFields;
	my_ulonglong numRows;
	unsigned int numFields;

	MYSQL_Connection = mysql_init(NULL);

	mysql = mysql_real_connect(MYSQL_Connection,hostName.c_str(),userId.c_str(),password.c_str(),DB.c_str(),0,NULL,0);

	if(mysql == NULL)
	{
		error_Mysql( (char*)mysql_error(MYSQL_Connection));
		mysql_close(MYSQL_Connection);
		exit(1);
	}
	printf("MYSQL Connection info %s\n",mysql_get_host_info(MYSQL_Connection));
	printf("MYSQL Client info %s\n",mysql_get_client_info());
	printf("MYSQL Server info %s\n",mysql_get_server_info(MYSQL_Connection));

    printf("starting server : in %s port\n", argv[1]);

    pthread_mutex_init(&mutx, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
	{
		error_handling(error_sock);
	}

    printf("Successfully create server socket\n");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
    {
    	error_handling(error_bind);
    }

    if (listen(serv_sock, 5)==-1)
    {
    	error_handling(error_listen);
    }

    printf("Successful binding\n");
	printf("Waiting client\n");

    clnt_addr_sz= sizeof(clnt_addr);
    while(1)
    {
        t=localtime(&timer);
        clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_sz);
		if(clnt_sock == -1)
		{
			error_handling(error_accept);
		}

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
    int str_len=0;
    char msg[BUF_SIZE];
    memset(&msg, 0, sizeof(msg));

    char *ptr;
    Player player = Player();
    int count =0;

    std::string input_msg;
    Json::Value string_Value;
    Json::Reader string_Reader;

    std::string temp = "400";

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
    	input_msg = msg;
    	string_Reader.parse(input_msg,string_Value);

    	if(temp.compare(string_Value["what"].asString()) ==0)
    	{
    		Json::Value make;
			pthread_mutex_lock(&mutx);
			for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
			{
				if (clnt_sock==clnt_socks[player.get_Room_Num()][i])
				{
					while(i<clnt_cnt[player.get_Room_Num()]-1)
					{
						clnt_socks[player.get_Room_Num()][i]=clnt_socks[player.get_Room_Num()][i+1];
						i++;
					}
					break;
				}
			}
			clnt_cnt[player.get_Room_Num()]--;
			pthread_mutex_unlock(&mutx);
			Main_Room.create_Sub_Room(string_Value,make);
			player.set_Room_Num(atoi(string_Value["roomNum"].asCString()));

			clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
			make["what"] = "400";
			send_Json(make, clnt_sock);
    	}
    	else
    	{
    		send_msg(unknown_Error,sizeof(unknown_Error), player);
    	}
    	/*
    	switch(string_Value["what"].asString())
		{
		case'200': // plane chat format "c&'player name : plane text'"
			Json::Value chat;
			ptr = strtok(msg, "&");
			chat["what"] = "200";
			chat["who"]=player.get_Player_Name();
			chat["chat"] = ptr;
			send_Json(chat, player);
			break;
		case'403': // get room details format "r"
			Json::Value room;
			room["what"] = "403";
			Main_Room.show_Room_Detail(room);
			send_Json(room,clnt_sock);
			break;
		case'400': // make room format m&room_Num&room_Name&max_Player&room_pwd(있다면)

			break;

		case'e': // enter room format e&room_Num
			char enter_Msg[100];
			ptr = strtok(msg, "&");
			if(Main_Room.check_Status(atoi(ptr)))
			{
				pthread_mutex_lock(&mutx);
				for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
				{
					if (clnt_sock==clnt_socks[player.get_Room_Num()][i])
					{
						while(i<clnt_cnt[player.get_Room_Num()]-1)
						{
							clnt_socks[player.get_Room_Num()][i]=clnt_socks[player.get_Room_Num()][i+1];
							i++;
						}
						break;
					}
				}
				clnt_cnt[player.get_Room_Num()]--;
				pthread_mutex_unlock(&mutx);

				player.set_Room_Num(atoi(ptr));
				clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
				Main_Room.enter_Room(player.get_Room_Num(), player, ptr);

				sprintf(enter_Msg, "%s enter room %d", player.get_Player_Name(), player.get_Room_Num());
				send_msg(enter_Msg, sizeof(enter_Msg), player);

				break;
			}

			send_msg(error_room, sizeof(error_room), player);
			break;

		case'o': // out room
			char out_Msg[100];
			sprintf(out_Msg, "%s out of room %d", player.get_Player_Name(), player.get_Room_Num());
			send_msg(out_Msg, sizeof(out_Msg), player);

			pthread_mutex_lock(&mutx);
			Main_Room.out_Room(player.get_Room_Num(), player);
			for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
			{
				if (clnt_sock==clnt_socks[player.get_Room_Num()][i])
				{
					while(i<clnt_cnt[player.get_Room_Num()]-1)
					{
						clnt_socks[player.get_Room_Num()][i]=clnt_socks[player.get_Room_Num()][i+1];
						i++;
					}
					break;
				}
			}
			clnt_cnt[player.get_Room_Num()]--;
			player.set_Room_Num(LOBBY);
			clnt_socks[LOBBY][clnt_cnt[LOBBY]++]=clnt_sock;
			pthread_mutex_unlock(&mutx);
			break;
		case 'b': // block room capacity
			ptr = strtok(msg, "&");
			Main_Room.minus_Room_Max_Player(atoi(ptr));
			break;
		case 's': // game start
			Main_Room.game_Start(player.get_Room_Num());
			send_msg(game_start, sizeof(game_start), player);
			break;
		case 'h': // submit hand
			ptr = strtok(msg, "&");
			ptr = strtok(NULL, "&");
			ptr = strtok(NULL, "&");
			while (ptr != NULL)
			{
				player.add(atoi(ptr));
				ptr = strtok(NULL, "&");
			}
			break;
		case 'p': // pop out card
			ptr = strtok(msg, "&");
			player.pop(atoi(msg));
			Main_Room.set_pop(player);
			break;
			/*
			 *
		case 'v': // invite friends
			break;
		case'f': // associate with friends
			break;
		case 't': // add BOT player
			break;

		case '*': // player sign up format *&'player_Name'&'player_UID'
			ptr = strtok(msg, "&");
			player.set_Player_Name(msg);
			ptr = strtok(NULL, "&");
			if(reg_Player(MYSQL_Connection,player.get_Player_Name(),ptr))
			{
				send_reg(reg_succes,sizeof(reg_succes),clnt_sock);
			}
			break;
			 *
 */
    	memset(&msg, 0, sizeof(msg));
    }

    pthread_mutex_lock(&mutx);
    for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
    {
    	if (clnt_sock==clnt_socks[player.get_Room_Num()][i])
    	{
    		while(i<clnt_cnt[player.get_Room_Num()]-1)
    		{
    			clnt_socks[player.get_Room_Num()][i]=clnt_socks[player.get_Room_Num()][i+1];
    			i++;
    		}
    		break;
    	}
    }
    clnt_cnt[player.get_Room_Num()]--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char* msg, int len, Player player)
{
	printf("%s \n",msg);
	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		write(clnt_socks[player.get_Room_Num()][i], msg, len);
	}
	pthread_mutex_unlock(&mutx);

}
void send_Json(Json::Value root, Player player)
{
	std::string jsonadpter;
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	char* msg = (char*)malloc(sizeof(jsonadpter)+10);
	strcpy(msg,root["what"].asCString());
	strcpy(msg,jsonadpter.c_str());
	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		write(clnt_socks[player.get_Room_Num()][i], msg, sizeof(msg));
	}
	pthread_mutex_unlock(&mutx);
	free(msg);
}
void send_Json(Json::Value root, int clnt_sock)
{
	std::string jsonadpter;
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	char* msg = (char*)malloc(sizeof(jsonadpter)+10);
	strcpy(msg,root["what"].asCString());
	strcpy(msg,jsonadpter.c_str());
	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[LOBBY]; i++)
	{
		if(clnt_socks[LOBBY][i] == clnt_sock)
		{
			write(clnt_socks[LOBBY][i], msg, sizeof(msg));
			break;
		}
	}
	pthread_mutex_unlock(&mutx);
	free(msg);
}
void send_reg(char* msg, int len, int clnt_sock)
{
	printf("%s \n",msg);
	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[LOBBY]; i++)
	{
		if(clnt_socks[LOBBY][i] == clnt_sock)
		{
			write(clnt_socks[LOBBY][i], msg, len);
			break;
		}
	}
	pthread_mutex_unlock(&mutx);

}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    mysql_close(MYSQL_Connection);
    exit(1);
}
void error_Mysql(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
}


