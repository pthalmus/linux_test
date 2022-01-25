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

#define BUF_SIZE 1024
#define MAX_IP 30
#define MAX_ROOM 40
#define LOBBY 0

void error_handling(char *msg);
void * handle_clnt(void *arg);
void *handle_room(void *arg);
void error_Mysql(char *msg);
void send_msg(char *msg, int len, Player player);
void send_Json(Json::Value root, int clnt_sock , Player player);
void send_Json(Json::Value root, Player player);
void send_Json(Json::Value root, int clnt_sock);
void send_Detail(Json::Value root);
void send_reg(char* msg, int len, int clnt_sock);


main_Room Main_Room = main_Room();
int clnt_cnt[MAX_ROOM];
int clnt_socks[MAX_ROOM][100];
pthread_mutex_t mutx;


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
    pthread_t r_id;

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
    	pthread_create(&r_id, NULL, handle_room, (void*)&clnt_sock);
    	pthread_detach(r_id);

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

    Json::Value string_Value;
    Json::Reader string_Reader;

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
    	string_Reader.parse(msg,string_Value);
    	switch(string_Value["what"].asInt())
    	{
			case 100:
			{
				Json::Value reg;
				reg["what"] = 100;
				if(reg_Player(MYSQL_Connection,string_Value["userName"].asString(),string_Value["userID"].asString()))
				{
					reg["isValidate"] = "1";
				}
				else
				{
					reg["isValidate"] = "0";
				}
				send_Json(reg, clnt_sock);
				break;
			}
			case 101:
			{
				int mysql_status = 0;
				MYSQL_RES *mysql_res;
				MYSQL_ROW mysqlRow;

				char row1[50];
				char row2[30];
				char row3[13];
				char row4[13];

				Json::Value excute;
				excute["what"] = 101;
				excute["isUser"] = "0";

				if(mysql_res)
				{
					mysql_free_result(mysql_res);
					mysql_res = NULL;
				}

				mysql_res = selcet_Player(MYSQL_Connection,string_Value["userID"].asString().c_str());

				while((mysqlRow = mysql_fetch_row(mysql_res)) != NULL)
				{
					strcpy(row1,mysqlRow[0]);
					strcpy(row2,mysqlRow[1]);
					strcpy(row3,mysqlRow[2]);
					strcpy(row4,mysqlRow[3]);
					excute["userID"] = row1;
					excute["userName"] = row2;
					excute["rank"] = atoi(row3);
					excute["cash"] = atoi(row4);
					excute["isUser"] = "1";
				}
				player.set_Player_Name(row2);
				send_Json(excute, clnt_sock);
				break;
			}
			case 102:
			{
				Json::Value del;
				del["what"] = 102;
				if(del_Player(MYSQL_Connection,string_Value["userID"].asString()))
				{
					del["isDeleted"] = "1";
				}
				else
				{
					del["isDeleted"] = "0";
				}
				send_Json(del, clnt_sock);
				break;
			}
			case 103:
			{
				Json::Value friendS;
				friendS["what"] = 103;
				friendS["who"] = player.get_Player_Name();
				friendS["to"] = string_Value["who"];

				break;
			}
			case 104:
			{
				Json::Value friendA;
				break;
			}
			case 200:
			{
				Json::Value chat;
				chat["what"] = 200;
				chat["who"]=player.get_Player_Name();
				chat["chat"] = string_Value["chat"];
				send_Json(chat, player);
				break;
			}
			case 300:
			{
				Json::Value hand;
				hand["what"] = 300;
				player.add(atoi(string_Value["1"].asString().c_str()));
				player.add(atoi(string_Value["2"].asString().c_str()));
				player.add(atoi(string_Value["3"].asString().c_str()));
				player.add(atoi(string_Value["4"].asString().c_str()));
				player.add(atoi(string_Value["5"].asString().c_str()));
				player.add(atoi(string_Value["6"].asString().c_str()));
				hand["status"] = 1;
				send_Json(hand, clnt_sock, player);
				break;
			}
			case 301:
			{
				Json::Value pop;
				pop["what"] = 301;
				player.pop(atoi(string_Value["pop"].asString().c_str()));
				Main_Room.set_pop(player);
				pop["status"] = 1;
				send_Json(pop, clnt_sock, player);
				break;
			}
				break;
				/*
			case 302:
				break;
				*/
			case 303:
			{
				Json::Value block;
				block["what"] = 303;
				if(Main_Room.minus_Room_Max_Player(string_Value["where"].asInt()))
				{
					block["status"] = 1;
				}
				else
				{
					block["status"] = 0;
				}
				send_Json(block,player);
				break;
			}
			case 400:
			{
				Json::Value make;
				make["what"] =400;
				int Num400;
				if((Num400 = Main_Room.create_Sub_Room(string_Value)) != 0)
				{
					make["status"] ="1";
					send_Json(make, clnt_sock);

					pthread_mutex_lock(&mutx);

					for (int i=0; i<clnt_cnt[LOBBY]; i++)
					{
						if (clnt_sock==clnt_socks[LOBBY][i])
						{
							while(i<clnt_cnt[LOBBY]-1)
							{
								clnt_socks[LOBBY][i]=clnt_socks[LOBBY][i+1];
								i++;
							}
							clnt_cnt[LOBBY]--;
							break;
						}
					}
					player.set_Room_Num(Num400);

					clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
					pthread_mutex_unlock(&mutx);
				}
				else
				{
					make["status"] = 0;
				}
				send_Json(make, clnt_sock);
				break;
			}
			case 401:
			{
				Json::Value in;
				in["what"] = 401;
				if(Main_Room.check_Status(atoi(string_Value["roomNum"].asString().c_str())))
				{
					pthread_mutex_lock(&mutx);
					for (int i=0; i<clnt_cnt[LOBBY]; i++)
					{
						if (clnt_sock==clnt_socks[LOBBY][i])
						{
							while(i<clnt_cnt[LOBBY]-1)
							{
								clnt_socks[LOBBY][i]=clnt_socks[LOBBY][i+1];
								i++;
							}
							clnt_cnt[LOBBY]--;
							break;
						}
					}
					pthread_mutex_unlock(&mutx);

					player.set_Room_Num(string_Value["where"].asInt());
					clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
					Main_Room.enter_Room(player.get_Room_Num(), player, ptr);

					in["who"] = player.get_Player_Name();
					in["where"] = player.get_Room_Num();
					in["status"] = 1;
				}
				else
				{
					in["status"] = 0;
				}
				send_Json(in,player);
				break;
			}
			case 402:
			{
				Json::Value out;
				out["what"] = 402;
				out["who"] = player.get_Player_Name();
				out["where"] = player.get_Room_Num();

				send_Json(out,player);

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
			}
				/*
			case 404:
				break;
			case 405:
				break;
				*/
			default:
			{
				Json::Value error;
				error["what"] = 900;
				error["why"] ="Unknown Error Code!!";
				send_Json(error, clnt_sock);
				break;
			}
    	}
    	/*
    	switch(string_Value["what"].asString())
		{
		case 's': // game start
			Main_Room.game_Start(player.get_Room_Num());
			send_msg(game_start, sizeof(game_start), player);
			break;
		case 'p': // pop out card
			ptr = strtok(msg, "&");
			player.pop(atoi(msg));
			Main_Room.set_pop(player);
			break;
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
    printf(" chatter (%d/100)\n", clnt_cnt[LOBBY]);
    return NULL;
}

void *handle_room(void *arg)
{
	Json::Value room;
	room["what"] = 403;
	while(1)
	{
		for(int i=1; i<40; i++)
		{
			if(Main_Room.check_Room_Detail(i))
			{
				room["detail"].append(Main_Room.show_Room_Detail(i));
			}
		}
		send_Detail(room);
		std::cout<<"send room detail\n";
		sleep(5000);
	}
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
	jsonadpter.reserve(1024);
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	std::cout<<"player room : " << player.get_Room_Num() <<"\n";

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		write(clnt_socks[player.get_Room_Num()][i], jsonadpter.c_str(), jsonadpter.size());
		std::cout<<"clnt_socks: " << clnt_socks[player.get_Room_Num()][i] <<"\n";
	}
	pthread_mutex_unlock(&mutx);
}
void send_Json(Json::Value root, int clnt_sock)
{
	std::string jsonadpter;
	jsonadpter.reserve(root.size());
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[LOBBY]; i++)
	{
		if(clnt_socks[LOBBY][i] == clnt_sock)
		{
			write(clnt_socks[LOBBY][i], jsonadpter.c_str(), jsonadpter.size());
			break;
		}
	}
	pthread_mutex_unlock(&mutx);
}
void send_Json(Json::Value root, int clnt_sock, Player player)
{
	std::string jsonadpter;
	jsonadpter.reserve(root.size());
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		if(clnt_socks[player.get_Room_Num()][i] == clnt_sock)
		{
			write(clnt_socks[player.get_Room_Num()][i], jsonadpter.c_str(), jsonadpter.size());
			break;
		}
	}
	pthread_mutex_unlock(&mutx);
}
void send_Detail(Json::Value root)
{
	std::string jsonadpter;
	jsonadpter.reserve(root.size());
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[LOBBY]; i++)
	{
		write(clnt_socks[LOBBY][i], jsonadpter.c_str(), jsonadpter.size());
	}
	pthread_mutex_unlock(&mutx);
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


