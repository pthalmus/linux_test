/*
 * main_Room.h
 *
 *  Created on: 2022. 1. 13.
 *      Author: s9342931
 */

#ifndef MAIN_ROOM_H_
#define MAIN_ROOM_H_

#include<iostream>
#include<vector>
#include<algorithm>
#include<string.h>
#include"sub_Room.h"

#define max_room 40


class main_Room {
private:
	subRoom sbroom[max_room];
public:
	main_Room()
	{
		for(int i=0; i<max_room; i++)
		{
			sbroom[i] = subRoom();
		}
	};
	void show_Room_Detail(Json::Value root);
	bool create_Sub_Room(Json::Value root, Json::Value outPut);
	void minus_Room_Max_Player(int room_Num);
	void enter_Room(int room_Num, Player player, char* msg);
	void out_Room(int room_Num, Player player);
	void set_pop(Player player);
	bool check_Room(int room_Num);
	void game_Start(int room_Num);
	void game_End(int room_Num);
	bool check_Status(int room_Num);
};

void main_Room::show_Room_Detail(Json::Value root)
{
	char pwd;
	Json::Value roomDetail;

	for (int i = 1; i < max_room; i++)
	{
		if (sbroom[i].get_Room_Number() != 0 && sbroom[i].get_Room_Status() == 0)
		{
			if (sbroom[i].get_Room_PWD() == 0)
			{
				pwd = 'Y';
			}
			else
			{
				pwd = 'N';
			}
			roomDetail["roomNum"]=i;
			roomDetail["roomName"]=sbroom[i].get_Room_Name();
			roomDetail["roomPW"]= pwd;
			roomDetail["curPlayer"]=sbroom[i].get_Cur_Player();
			roomDetail["Max_Player"]=sbroom[i].get_Max_Player();
			roomDetail["status"]=sbroom[i].get_Room_Status();
			root["roomDetail"] = roomDetail;
		}
	}
}
bool main_Room::create_Sub_Room(Json::Value root, Json::Value outPut)
{
	int count;
	for (int i = 1; i < max_room; i++)
	{
		if (sbroom[i].get_Room_Number() == 0)
		{
			count = i;
			break;
		}
	}
	if (root["roomPW"] == "")
	{
		sbroom[count] = subRoom(count, (char*)root["roomName"].asCString(),atoi(root["maxPlayer"].asCString()));
		printf("%d번방 %s이(가) 생성되었습니다\n", count, (char*)root["roomName"].asCString());
		outPut["status"] ="1";
		return true;
	}
	else
	{
		sbroom[count] = subRoom(count, (char*)root["roomName"].asCString(),atoi(root["maxPlayer"].asCString()),(char*)root["roomName"].asCString());
		printf("비밀번호가 있는 %d번방 %s이(가) 생성되었습니다\n", count, (char*)root["roomName"].asCString());
		outPut["status"] ="1";
		return true;
	}
	outPut["status"] ="0";
	return false;
}
void main_Room::minus_Room_Max_Player(int room_Num)
{
	for(int i=0; i<max_room; i++)
	{
		if(sbroom[i].get_Room_Number() == room_Num)
		{
			sbroom[i].block_Player();
		}
	}
}
void main_Room::enter_Room(int room_Num, Player player, char* msg)
{
	sbroom[room_Num].add_Player(player,msg);
}
void main_Room::out_Room(int room_Num, Player player)
{
	sbroom[room_Num].pop_Player(player);
	if(sbroom[room_Num].get_Cur_Player() ==0 )
	{
	}
}
void main_Room::set_pop(Player player)
{
	sbroom[player.get_Room_Num()].push_Pop(player.get_Last_Pop());
}
bool main_Room::check_Room(int room_Num)
{
	if( sbroom[room_Num].get_Room_Number() != 0)
	{
		return true;
	}
	return false;
}
void main_Room::game_Start(int room_Num)
{
	sbroom[room_Num].set_Room_Status(1);
}
void main_Room::game_End(int room_Num)
{
	sbroom[room_Num].set_Room_Status(0);
}
bool main_Room::check_Status(int room_Num)
{
	if(sbroom[room_Num].get_Room_Status() == 0)
	{
		return true;
	}
	return false;
}


#endif /* MAIN_ROOM_H_ */
