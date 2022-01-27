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
void error_Mysql(char *msg);
void send_msg(char *msg, int len, Player player);
void send_Json(Json::Value root, int clnt_sock , Player player);
void send_Json(Json::Value root, Player player);
void send_Without(Json::Value root,int clnt_sock, Player player);
void send_Json(Json::Value root, int clnt_sock);
void send_Json(Json::Value root, int clnt_sock, int room_Num);


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

    Json::Value string_Value;
    Json::Reader string_Reader;

    while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
    	std::cout<<msg<<"\n";
    	string_Reader.parse(msg,string_Value);
    	switch(string_Value["what"].asInt())
    	{
			case 100: // 가입
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
			case 101: // 초기 로그인 및 아이디 확인
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
				player.set_Player_Name(excute["userName"].asString());
				send_Json(excute, clnt_sock);
				break;
			}
			case 102: //회원탈퇴
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
			case 103: //로그인
			{
				int mysql_status = 0;
				MYSQL_RES *mysql_res;
				MYSQL_ROW mysqlRow;

				char row1[50];
				char row2[30];
				char row3[13];
				char row4[13];

				Json::Value excute;
				excute["what"] = 103;
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
				player.set_Player_Name(excute["userName"].asString());
				send_Json(excute, clnt_sock);
				break;
			}
			case 104:
			{
				Json::Value friendA;
				break;
			}
			case 200: //일반 채팅
			{
				Json::Value chat;
				chat["what"] = 200;
				chat["who"]=player.get_Player_Name();
				chat["chat"] = string_Value["chat"];
				send_Json(chat, player);
				break;
			}
			case 300: // 핸드(6장 제출)
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
				Main_Room.add_Hand(player.get_Room_Num(), player);
				send_Json(hand, clnt_sock, player);
				if(Main_Room.check_Hand_Count(player.get_Room_Num()))
				{
					std::cout<<"hand 304 call\n";
					hand["what"] = 304;
					Main_Room.set_order(player.get_Room_Num());
					hand["order"] = Main_Room.get_order(player.get_Room_Num());
					hand["token"] = Main_Room.get_token(player.get_Room_Num());
					send_Json(hand, player);
				}
				break;
			}
			case 303: //게임 시작
			{
				Json::Value Game_Start;
				Main_Room.game_Start(player.get_Room_Num());
				Game_Start["what"] = 303;
				Game_Start["status"] = 1;
				send_Json(Game_Start, player);
				break;
			}
			case 301: //패(1장) 제출
			{
				Json::Value pop;
				std::string check301;
				Json::Value winner;
				std::vector<int> hand;
				Json::Value order;
				std::string player_Count = "player";
				ssize_t pos;

				std::string Swinner;
				std::string Sloser;

				pop["what"] = 301;
				player.pop(stoi(string_Value["pop"].asString()));
				check301 = Main_Room.set_pop(player);
				hand = player.get_Hand();
				for(int i = hand.size(); i < 6; i++)
				{
					hand.push_back(0);
				}
				pop["1"] = std::to_string(hand.at(0));
				pop["2"] = std::to_string(hand.at(1));
				pop["3"] = std::to_string(hand.at(2));
				pop["4"] = std::to_string(hand.at(3));
				pop["5"] = std::to_string(hand.at(4));
				pop["6"] = std::to_string(hand.at(5));
				std::cout<<pop<<"\n";
				send_Json(pop, clnt_sock, player);

				if(check301.size() !=0)
				{
					Swinner = check301.substr(0, check301.find("&"));
					Sloser = check301.substr(check301.find("&"));
					Sloser = Sloser.substr(1);
					std::cout<< Sloser<<"\n";
					winner["what"] = 305;
					winner["winner"] = Swinner;
					if(Swinner.compare("NoOne") == 0)
					{
						Json::Value order;
						order["what"] = 304;
						Main_Room.set_order(player.get_Room_Num());
						order["order"] = Main_Room.get_order(player.get_Room_Num());
						order["token"] = Main_Room.get_token(player.get_Room_Num());
						send_Json(order, player);
						continue;
					}
					winner["loser"] = Sloser;
					if(Sloser.compare("Win")==0)
					{
						Json::Value total_Winner;
						total_Winner["what"] = 302;
						total_Winner["winner"] = Swinner;
						Main_Room.game_End(player.get_Room_Num());
						send_Json(total_Winner, player);
						continue;
					}
					send_Json(winner, player);

					usleep(200000);
					pos = Swinner.find("BOT");

					if( pos != -1 )
					{
						std::cout<<"bot 304\n";
						order["what"] = 304;
						Main_Room.set_order(player.get_Room_Num());
						order["order"] = Main_Room.get_order(player.get_Room_Num());
						order["token"] = Main_Room.get_token(player.get_Room_Num());

						send_Json(order, player);

						usleep(200000);
						Main_Room.re_Pop_Bot(player.get_Room_Num());

						for(int i=0; i<8; i++)
						{
							Json::Value chat;
							chat["what"] = 200;
							chat["who"]= "system";
							int return_Pop;
							std::string pop_Player_Name;
							std::string combi = "\n";
							pop_Player_Name = Main_Room.get_room_Player(player.get_Room_Num(), i);
							if(pop_Player_Name.compare("Locked") == 0)
							{
								continue;
							}
							return_Pop = Main_Room.return_last_Pop(player.get_Room_Num(), i);
							combi+=pop_Player_Name;
							combi+="가 ";
							combi+=std::to_string(return_Pop);
							combi+="를 제출하였습니다.\n";
							chat["chat"] = combi;
							send_Json(chat, player);
							usleep(200000);
						}
					}
					std::cout<<winner<<"\n";
				}
				break;
			}
			case 306:
			{
				Json::Value winner;
				std::vector<int> hand;
				winner["what"] = 301;
				Json::Value order;
				player.set_Hand(Main_Room.return_Hand(player.get_Room_Num(),player));
				hand = player.get_Hand();
				for(int i = hand.size(); i < 6; i++)
				{
					hand.push_back(0);
				}
				winner["1"] = std::to_string(hand.at(0));
				winner["2"] = std::to_string(hand.at(1));
				winner["3"] = std::to_string(hand.at(2));
				winner["4"] = std::to_string(hand.at(3));
				winner["5"] = std::to_string(hand.at(4));
				winner["6"] = std::to_string(hand.at(5));
				send_Json(winner, clnt_sock, player);

				usleep(200000);

				std::cout<<"order 304 call\n";
				order["what"] = 304;
				Main_Room.set_order(player.get_Room_Num());
				order["order"] = Main_Room.get_order(player.get_Room_Num());
				order["token"] = Main_Room.get_token(player.get_Room_Num());
				send_Json(order, player);

				usleep(200000);


				Main_Room.re_Pop_Bot(player.get_Room_Num());

				for(int i=0; i<8; i++)
				{
					Json::Value chat;
					chat["what"] = 200;
					chat["who"]= "system";
					int return_Pop;
					std::string pop_Player_Name;
					std::string combi = "\n";
					pop_Player_Name = Main_Room.get_room_Player(player.get_Room_Num(), i);
					if(pop_Player_Name.compare("Locked") == 0)
					{
						continue;
					}
					return_Pop = Main_Room.return_last_Pop(player.get_Room_Num(), i);
					combi+=pop_Player_Name;
					combi+="가 ";
					combi+=std::to_string(return_Pop);
					combi+="를 제출하였습니다.\n";
					chat["chat"] = combi;
					send_Json(chat, player);
					usleep(200000);
				}
				break;
			}
			case 400: //방 만들기
			{
				Json::Value make;
				Json::Value player_in_room;
				make["what"] =400;
				int Num400;
				if((Num400 = Main_Room.create_Sub_Room(string_Value)) != 0 && player.get_Room_Num() == LOBBY)
				{
					player.set_Room_Num(Num400);


					pthread_mutex_lock(&mutx);
					Main_Room.enter_Room(player.get_Room_Num(), player);
					pthread_mutex_unlock(&mutx);


					make["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
					make["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
					make["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
					make["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
					make["roomNum"] = std::to_string(player.get_Room_Num());
					make["host"] = Main_Room.get_room_Host(player.get_Room_Num());

					std::string player_Count = "player";
					for(int i=0; i<8; i++)
					{
						if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
						{
							player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
						}
						else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
						{
							player_in_room[player_Count] = "Bot";
						}
						else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
						{
							player_in_room[player_Count] ="Locked";
						}
						else
						{
							player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
						}
						make["players"].append(player_in_room);
					}
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
			case 401: //방 입장
			{
				Json::Value in;
				Json::Value player_in_room;
				in["what"] = 401;

				if(Main_Room.check_Status(std::stoi(string_Value["roomNum"].asString())))
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


					player.set_Room_Num(std::stoi(string_Value["roomNum"].asString()));
					clnt_socks[player.get_Room_Num()][clnt_cnt[player.get_Room_Num()]++]=clnt_sock;
					Main_Room.enter_Room(player.get_Room_Num(), player);

					pthread_mutex_unlock(&mutx);

					in["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
					in["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
					in["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
					in["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
					in["roomNum"] = std::to_string(player.get_Room_Num());
					in["host"] = Main_Room.get_room_Host(player.get_Room_Num());

					std::string player_Count = "player";
					for(int i=0; i<8; i++)
					{
						if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
						{
							player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
						}
						else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
						{
							player_in_room[player_Count] = "Bot";
						}
						else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
						{
							player_in_room[player_Count] ="Locked";
						}
						else
						{
							player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
						}
						in["players"].append(player_in_room);
					}
					in["status"] = 1;
					send_Json(in,player);
				}
				else
				{
					in["status"] = 0;
					send_Json(in,clnt_sock);
				}
				break;
			}
			case 402: //방 퇴장
			{
				Json::Value out;
				Json::Value player_in_room;

				if(player.get_Room_Num() != LOBBY)
				{
					Main_Room.out_Room(player.get_Room_Num(), player);

					out["what"] = 401;
					out["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
					out["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
					out["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
					out["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
					out["roomNum"] = std::to_string(player.get_Room_Num());
					out["host"] = Main_Room.get_room_Host(player.get_Room_Num());
					std::string player_Count = "player";
					for(int i=0; i<8; i++)
					{
						if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
						{
							player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
						}
						else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
						{
							player_in_room[player_Count] = "Bot";
						}
						else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
						{
							player_in_room[player_Count] ="Locked";
						}
						else
						{
							player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
						}
						out["players"].append(player_in_room);
					}
					send_Without(out,clnt_sock, player);
					out["what"] = 402;
					send_Json(out,clnt_sock, player.get_Room_Num());



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

					player.set_Room_Num(LOBBY);
					clnt_socks[LOBBY][clnt_cnt[LOBBY]++]=clnt_sock;
					pthread_mutex_unlock(&mutx);
				}

				break;
			}
			case 403: //방 정보 불러오기
			{
				Json::Value room;
				room["what"] = 403;
				for(int i=1; i<40; i++)
				{
					if(Main_Room.check_Room_Detail(i))
					{
						room["detail"].append(Main_Room.show_Room_Detail(i));
					}
				}
				send_Json(room, clnt_sock);
				break;
			}
			case 404: // 방 최대인원 줄이기
			{
				pthread_mutex_lock(&mutx);
				Main_Room.lock_Room(player.get_Room_Num(), std::stoi(string_Value["lockNum"].asString()));
				pthread_mutex_unlock(&mutx);
				Json::Value change;
				Json::Value player_in_room;
				change["what"] = 401;
				std::string player_Count = "player";

				change["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
				change["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
				change["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
				change["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
				change["roomNum"] = std::to_string(player.get_Room_Num());
				change["host"] = Main_Room.get_room_Host(player.get_Room_Num());
				for(int i=0; i<8; i++)
				{
					if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
					{
						player_in_room[player_Count] = "Bot";
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
					{
						player_in_room[player_Count] ="Locked";
					}
					else
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					change["players"].append(player_in_room);
				}

				send_Json(change,player);
				break;
			}
			case 405: // 방 최대인원 늘리기
			{
				pthread_mutex_lock(&mutx);
				Main_Room.unlock_Room(player.get_Room_Num(),std::stoi(string_Value["lockNum"].asString()));
				pthread_mutex_unlock(&mutx);
				Json::Value change;
				Json::Value player_in_room;
				change["what"] = 401;
				std::string player_Count = "player";
				change["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
				change["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
				change["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
				change["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
				change["roomNum"] = std::to_string(player.get_Room_Num());
				change["host"] = Main_Room.get_room_Host(player.get_Room_Num());
				for(int i=0; i<8; i++)
				{
					if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
					{
						player_in_room[player_Count] = "Bot";
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
					{
						player_in_room[player_Count] ="Locked";
					}
					else
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					change["players"].append(player_in_room);
				}
				send_Json(change,player);
				break;
			}
			case 406: // 방 봇 늘리기
			{
				pthread_mutex_lock(&mutx);
				Main_Room.add_BOT(player.get_Room_Num(),std::stoi(string_Value["botNum"].asString()));
				pthread_mutex_unlock(&mutx);
				Json::Value change;
				Json::Value player_in_room;
				change["what"] = 401;
				std::string player_Count = "player";
				change["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
				change["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
				change["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
				change["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
				change["roomNum"] = std::to_string(player.get_Room_Num());
				change["host"] = Main_Room.get_room_Host(player.get_Room_Num());
				for(int i=0; i<8; i++)
				{
					if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
					{
						player_in_room[player_Count] = "Bot";
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
					{
						player_in_room[player_Count] ="Locked";
					}
					else
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					change["players"].append(player_in_room);
				}
				send_Json(change,player);
				break;
			}
			case 407: // 방 봇 줄이기
			{
				pthread_mutex_lock(&mutx);
				Main_Room.pop_BOT(player.get_Room_Num(),std::stoi(string_Value["botNum"].asString()));
				pthread_mutex_unlock(&mutx);
				Json::Value change;
				Json::Value player_in_room;
				change["what"] = 401;
				std::string player_Count = "player";
				change["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
				change["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
				change["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
				change["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
				change["roomNum"] = std::to_string(player.get_Room_Num());
				change["host"] = Main_Room.get_room_Host(player.get_Room_Num());
				for(int i=0; i<8; i++)
				{
					if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
					{
						player_in_room[player_Count] = "Bot";
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
					{
						player_in_room[player_Count] = "Locked";
					}
					else
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					change["players"].append( player_in_room);
				}
				send_Json(change,player);
				break;
			}
			case 408: // 인원 강퇴
			{
				Json::Value kick;
				kick["what"] = 408;
				kick["kickNum"] = string_Value["kickNum"];
				send_Json(kick,player);
				break;
			}
			case 409:
			{
				Json::Value change;
				Json::Value player_in_room;
				change["what"] = 401;
				Main_Room.change_Host(player.get_Room_Num(), std::stoi(string_Value["hostNum"].asString()));
				std::string player_Count = "player";
				change["roomName"] = Main_Room.get_room_Name(player.get_Room_Num());
				change["roomPW"] = Main_Room.get_room_PW(player.get_Room_Num());
				change["curPlayer"] = Main_Room.get_room_Cur_Player(player.get_Room_Num());
				change["maxPlayer"] = Main_Room.get_room_Max_Player(player.get_Room_Num());
				change["roomNum"] = std::to_string(player.get_Room_Num());
				change["host"] = Main_Room.get_room_Host(player.get_Room_Num());
				for(int i=0; i<8; i++)
				{
					if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 1)
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 2)
					{
						player_in_room[player_Count] = "Bot";
					}
					else if(Main_Room.check_room_Player(player.get_Room_Num(),i) == 3)
					{
						player_in_room[player_Count] = "Locked";
					}
					else
					{
						player_in_room[player_Count] = Main_Room.get_room_Player(player.get_Room_Num(),i);
					}
					change["players"].append( player_in_room);
				}
				send_Json(change,player);
				break;
			}
			default: //이외의 잘못된 플래그
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
    		Main_Room.out_Room(player.get_Room_Num(), player);
    		break;
    	}
    }
    clnt_cnt[player.get_Room_Num()]--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    printf(" chatter (%d/100)\n", clnt_cnt[LOBBY]);
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
	jsonadpter.reserve(1024);
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		write(clnt_socks[player.get_Room_Num()][i], jsonadpter.c_str(), jsonadpter.size());
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
void send_Without(Json::Value root,int clnt_sock, Player player)
{
	std::string jsonadpter;
	jsonadpter.reserve(1024);
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[player.get_Room_Num()]; i++)
	{
		if(clnt_socks[player.get_Room_Num()][i] != clnt_sock)
		{
			write(clnt_socks[player.get_Room_Num()][i], jsonadpter.c_str(), jsonadpter.size());
		}
	}
	pthread_mutex_unlock(&mutx);
}
void send_Json(Json::Value root, int clnt_sock, int room_Num)
{
	std::string jsonadpter;
	jsonadpter.reserve(root.size());
	Json::StyledWriter writer;
	jsonadpter = writer.write(root);

	pthread_mutex_lock(&mutx);
	for (int i=0; i<clnt_cnt[room_Num]; i++)
	{
		if(clnt_socks[room_Num][i] == clnt_sock)
		{
			write(clnt_socks[room_Num][i], jsonadpter.c_str(), jsonadpter.size());
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


