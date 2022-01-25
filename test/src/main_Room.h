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
	Json::Value show_Room_Detail(int room_Num);
	bool check_Room_Detail(int room_Num);
	bool create_Sub_Room(Json::Value root);
	bool minus_Room_Max_Player(int room_Num);
	void enter_Room(int room_Num, Player player, char* msg);
	void out_Room(int room_Num, Player player);
	void set_pop(Player player);
	bool check_Room(int room_Num);
	void game_Start(int room_Num);
	void game_End(int room_Num);
	bool check_Status(int room_Num);
};

Json::Value main_Room::show_Room_Detail(int room_Num)
{
	Json::Value rd;

	if (sbroom[room_Num].get_Room_Number() != 0 && sbroom[room_Num].get_Room_Status() == 0)
	{
		rd["roomNum"]= std::to_string(room_Num);
		rd["roomName"]=sbroom[room_Num].get_Room_Name();
		rd["roomPW"]=sbroom[room_Num].get_Room_PWD();
		rd["curPlayer"]=sbroom[room_Num].get_Cur_Player();
		rd["maxPlayer"]=sbroom[room_Num].get_Max_Player();
	}
	return rd;
}
bool main_Room::check_Room_Detail(int room_Num)
{
	if (sbroom[room_Num].get_Room_Number() != 0 && sbroom[room_Num].get_Room_Status() == 0)
	{
		return true;
	}
	return false;
}
bool main_Room::create_Sub_Room(Json::Value root)
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
		sbroom[count] = subRoom(count, (char*)root["roomName"].asCString(),atoi(root["maxPlayer"].asString().c_str()));
		printf("%d번방 %s(이)가 생성되었습니다\n", count, (char*)root["roomName"].asCString());
		return true;
	}
	else
	{
		sbroom[count] = subRoom(count, (char*)root["roomName"].asCString(),atoi(root["maxPlayer"].asString().c_str()),root["roomPW"].asString());
		printf("비밀번호가 있는 %d번방 %s(이)가 생성되었습니다\n", count, (char*)root["roomName"].asCString());
		return true;
	}
	return false;
}
bool main_Room::minus_Room_Max_Player(int room_Num)
{
	for(int i=1; i<max_room; i++)
	{
		if(sbroom[i].get_Room_Number() == room_Num)
		{
			sbroom[i].block_Player();
			return true;
		}
	}
	return false;
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
