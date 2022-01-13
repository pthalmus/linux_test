/*
 * sub_Room.cpp
 *
 *  Created on: 2022. 1. 6.
 *      Author: s9342931
 */
#include<iostream>
#include<vector>
#include<algorithm>
#include<player.h>


class subRoom {
private:
	int room_Number=0;
	char* room_Pwd = NULL;
	char* room_Name = NULL;
	int max_Player = 8;
	std::vector<Player> player;
public:
	subRoom(int num, char* name, int pMNum)
	{
		room_Number = num;
		room_Name = name;
		max_Player = pMNum;
	}
	subRoom(int num, char* name, char* pwd, int pMNum)
	{
		room_Number = num;
		room_Name = name;
		room_Pwd = pwd;
		max_Player = pMNum;
	}
	bool add_Player(Player add_Player);
	void pop_Player(Player pop_Player);
	void block_Player();
	void free_Block_Player();
	bool invite_Player(Player online_Player);
	int get_Room_Number();
	int get_Room_PWD();
	int get_Max_Player();
	char* get_Room_Name();
};

bool subRoom::add_Player(Player add_player)
{
	if(player.size() < max_Player)
	{
		player.push_back(add_player);
		return true;
	}
	return false;
}
void subRoom::pop_Player(Player pop_Player)
{
	for(std::vector<Player>::iterator iter = player.begin(); iter!=player.end(); iter++)
	{
		if(*iter == pop_Player)
		{
			player.erase(iter);
			break;
		}
	}
}
void subRoom::block_Player()
{
	max_Player--;
}
void subRoom::free_Block_Player()
{
	if(max_Player <= 7)
	{
		max_Player++;
	}
}
bool subRoom::invite_Player(Player online_Player)
{

	return false;
}
int subRoom::get_Room_Number()
{
	return room_Number;
}
int subRoom::get_Room_PWD()
{
	if(room_Pwd == NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
int subRoom::get_Max_Player()
{
	return max_Player;
}
char* subRoom::get_Room_Name()
{
	return room_Name;
}
