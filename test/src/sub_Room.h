/*
 * sub_Room.h
 *
 *  Created on: 2022. 1. 6.
 *      Author: s9342931
 */

#ifndef SUB_ROOM_H_
#define SUB_ROOM_H_

#include<iostream>
#include<vector>
#include<algorithm>
#include<random>
#include"player.h"
#include<string.h>
#include"checking.h"


class subRoom {
private:
	int room_Number = 0;
	std::string room_Name;
	std::string room_Pwd;
	int cur_Player = 0;
	int max_Player = 8;
	Player room_Player[8];
	int order = 0;
	int token = 0;
	std::vector<int> pop_Inventory;
	int count =0;
	int room_Status = 0;
	int room_Leader = 1;
public:
	subRoom() {};
	subRoom(int num, std::string name, int pMNum)
	{
		room_Number = num;
		room_Name = name;
		for(int i=pMNum; i<max_Player; i++)
		{
			room_Player[i].set_Player_Name("Locked");
		}
		max_Player = pMNum;

	}
	subRoom(int num, std::string name, int pMNum, std::string pwd )
	{
		room_Number = num;
		room_Name = name;
		room_Pwd = pwd;
		for(int i=pMNum; i<max_Player; i++)
		{
			room_Player[i].set_Player_Name("Locked");
		}
		max_Player = pMNum;
	}
	void add_Player(Player add_Player);
	void pop_Player(Player pop_Player);
	void block_Player(int num);
	void free_Block_Player(int num);
	bool invite_Player(Player online_Player);
	int get_Room_Number();
	std::string get_Room_PWD();
	int get_Max_Player();
	std::string get_Room_Name();
	void set_Order();
	int get_Order();
	int get_Token();
	std::string push_Pop(int num);
	int get_Cur_Player();
	void set_Room_Status(int status);
	int get_Room_Status();
	std::string get_Player_Name(int count);
	void add_BOT(int num);
	void pop_BOT(int num);
	void set_Leader(int num);
	int get_Leader();
};

void subRoom::add_Player(Player add_player)
{
	if (cur_Player < max_Player)
	{
		for (int i = 0; i < max_Player; i++)
		{
			if (room_Player[i].get_Player_Name().size()==0)
			{
				room_Player[i].set_Player_Name(add_player.get_Player_Name());
				cur_Player++;
				break;
			}
		}
	}
}
void subRoom::pop_Player(Player pop_player)
{
	for (int i = 0; i < max_Player; i++)
	{
		if (room_Player[i].get_Player_Name().compare(pop_player.get_Player_Name())  == 0 )
		{
			room_Player[i].set_Player_Name("");
			cur_Player--;
			break;
		}
	}
}
void subRoom::block_Player(int num)
{
	room_Player[num].set_Player_Name("Locked");
	max_Player--;
}
void subRoom::free_Block_Player(int num)
{
	if (max_Player <= 7)
	{
		max_Player++;
		room_Player[num].set_Player_Name("");
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
std::string subRoom::get_Room_PWD()
{
	return room_Pwd;
}
int subRoom::get_Max_Player()
{
	return max_Player;
}
std::string subRoom::get_Room_Name()
{
	return room_Name;
}

void subRoom::set_Order()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(3, 12);

	order = dis(gen);

	std::random_device md;
	std::mt19937 men(md());
	std::uniform_int_distribution<int> mis(0, 1);
	token = mis(men);
}
int subRoom::get_Order()
{
	return order;
}
int subRoom::get_Token()
{
	return token;
}

std::string subRoom::push_Pop(int num)
{
	pop_Inventory.push_back(num);
	count++;
	int winner =0;
	int check;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, cur_Player);
	std::string output;

	check = dis(gen);

	if(count == cur_Player)
	{
		winner = calculrating(pop_Inventory);
		if( winner !=20 )
		{
			pop_Inventory.erase(std::remove(pop_Inventory.begin(),pop_Inventory.end(), winner), pop_Inventory.end());
			output += std::to_string(winner);
			output += "&";
			output += std::to_string(pop_Inventory[check]);
		}
		std::vector<int>().swap(pop_Inventory);
		count =0;
	}
	return output;
}
int subRoom::get_Cur_Player()
{
	return cur_Player;
}
void subRoom::set_Room_Status(int status)
{
	this->room_Status = status;
}
int subRoom::get_Room_Status()
{
	return room_Status;
}

std::string subRoom::get_Player_Name(int count)
{
	return room_Player[count].get_Player_Name();
}
void subRoom::add_BOT(int num)
{
	std::string botName = "BOT";
	if(cur_Player < max_Player)
	{
		room_Player[num].set_Player_Name(botName);
		room_Player[num].set_Random_Hand();
		cur_Player++;
	}
}
void subRoom::pop_BOT(int num)
{
	if(room_Player[num].get_Player_Name().compare("BOT") == 0)
	{
		room_Player[num].set_Player_Name("");
		room_Player[num].refresh_Hand();
		cur_Player--;
	}
}
void subRoom::set_Leader(int num)
{
	room_Leader = num;
}
int subRoom::get_Leader()
{
	return room_Leader;
}
#endif /* SUB_ROOM_H_ */
