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
	int room_Status = 2;
	int room_Host = 0;
	int hand_Count = 0;
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
		room_Status = 0;
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
		room_Status = 0;
	}

	void add_Player(Player add_Player);
	void pop_Player(Player pop_Player);
	void block_Player(int num);
	void free_Block_Player(int num);

	int get_Cur_Player();
	int get_Max_Player();
	int get_Room_Number();
	std::string get_Room_PWD();
	std::string get_Room_Name();
	std::string get_Player_Name(int count);

	std::string push_Pop(int num, Player player);
	std::string add_Item(std::string Value);
	std::vector<int> return_Hand(Player player);

	void set_Order();
	int get_Order();
	int get_Token();

	void set_Room_Status(int status);
	int get_Room_Status();

	bool check_BOT(Player player);
	void add_BOT(int num);
	void pop_BOT(int num);

	bool set_Host(int num);
	int get_Host();
	bool toss_Host(Player player);

	void add_Hand_Count();
	int get_Hand_Count();
	void set_Player_Hand(Player player);
	int return_Last_Pop(int num);
};

void subRoom::add_Player(Player add_player)
{
	if (cur_Player < max_Player)
	{
		for (int i = 0; i < max_Player; i++)
		{
			if (room_Player[i].get_Player_Name().size()==0)
			{
				room_Player[i] = add_player;
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

int subRoom::get_Cur_Player()
{
	return cur_Player;
}
int subRoom::get_Max_Player()
{
	return max_Player;
}
int subRoom::get_Room_Number()
{
	return room_Number;
}
std::string subRoom::get_Room_PWD()
{
	return room_Pwd;
}
std::string subRoom::get_Room_Name()
{
	return room_Name;
}
std::string subRoom::get_Player_Name(int count)
{
	return room_Player[count].get_Player_Name();
}

void subRoom::set_Order()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(3, 12);

	order = dis(gen);

	std::random_device md;
	std::mt19937 men(md());
	std::uniform_int_distribution<int> mis(0, 3);
	token = mis(men);
	if(token == 2 || token == 3 )
	{
		order = 0;
	}
}
int subRoom::get_Order()
{
	return order;
}
int subRoom::get_Token()
{
	return token;
}

std::string subRoom::push_Pop(int num, Player player)
{
	pop_Inventory.push_back(num);
	for(int i=0; i<8; i++)
	{
		if(room_Player[i].get_Player_Name().compare(player.get_Player_Name()) == 0)
		{
			room_Player[i].pop(num);
		}
	}
	count++;
	int winner =0;
	std::string output;

	if(count == cur_Player)
	{
		winner = calculrating(pop_Inventory);
		if( winner !=20 )
		{
			pop_Inventory.erase(std::remove(pop_Inventory.begin(),pop_Inventory.end(), winner), pop_Inventory.end());
			for(int i=0; i<8; i++)
			{
				if(room_Player[i].get_Last_Pop() == winner)
				{
					output += room_Player[i].get_Player_Name();
				}
			}
		}
		else
		{
			output = "NoOne";
		}

		count =0;
	}
	return output;
}
std::string subRoom::add_Item(std::string value)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, cur_Player-2);
	int check;
	check = dis(gen);
	int item = pop_Inventory.at(check);
	std::string loserName;

	for(int i=0; i<8; i++)
	{
		if(room_Player[i].get_Player_Name().compare(value) == 0)
		{
			room_Player[i].add(item);
		}
	}
	std::vector<int>().swap(pop_Inventory);
	for(int i =0; i<8; i++)
	{
		if( room_Player[i].get_Last_Pop() == item)
		{
			loserName = room_Player[i].get_Player_Name();
		}
	}

	return loserName;
}
std::vector<int> subRoom::return_Hand(Player player)
{
	std::vector<int> a;
	for(int i=0; i<8; i++)
	{
		if(room_Player[i].get_Player_Name().compare(player.get_Player_Name()) == 0)
		{
			return room_Player[i].get_Hand();
		}
	}
	return a;
}

void subRoom::set_Room_Status(int status)
{
	this->room_Status = status;

	if(room_Status == 1)
	{
		for(int i = 0; i< 8; i ++)
		{
			if(room_Player[i].check_Bot() == 1)
			{
				push_Pop(room_Player[i].pop_Random(order, token), room_Player[i]);
				add_Hand_Count();
			}
		}
	}
}
int subRoom::get_Room_Status()
{
	return room_Status;
}


void subRoom::add_BOT(int num)
{
	std::string botName = "BOT";
	botName.append(std::to_string(num));
	if(cur_Player < max_Player)
	{
		room_Player[num].set_Player_Name(botName);
		room_Player[num].set_Random_Hand();
		room_Player[num].set_Bot_Start();
		cur_Player++;
	}
}
void subRoom::pop_BOT(int num)
{
	if(room_Player[num].get_Player_Name().compare("BOT") == 0)
	{
		room_Player[num].set_Player_Name("");
		room_Player[num].set_Bot_End();
		cur_Player--;
	}
}
bool subRoom::set_Host(int num)
{
	ssize_t pos;
	if( (pos = room_Player[num].get_Player_Name().find("BOT", 0, 3)) == std::string::npos )
	{
		if( room_Player[num].get_Player_Name().compare("Locked") != 0 )
		{
			if( room_Player[num].get_Player_Name().size() != 0 )
			{
				room_Host = num;
				return true;
			}
		}
	}
	return false;
}
int subRoom::get_Host()
{
	return room_Host;
}
bool subRoom::toss_Host(Player player)
{
	if(room_Player[room_Host].get_Player_Name().compare(player.get_Player_Name()) == 0)
	{
		for(int i = room_Host+1; i<8; i++)
		{
			if( set_Host(i) )
			{
				return true;
			}
		}
		for(int i=0; i<room_Host; i++)
		{
			if( set_Host(i) )
			{
				return true;
			}
		}
	}
	return false;
}
bool subRoom::check_BOT(Player player)
{
	ssize_t pos;
	int count = 0;
	for(int i=0; i<8; i++)
	{
		if( (pos = room_Player[i].get_Player_Name().find("BOT", 0, 3)) == std::string::npos && room_Player[i].get_Player_Name().compare("Locked") != 0 && room_Player[i].get_Player_Name().size() != 0 )
		{
			if(room_Player[i].get_Player_Name().compare(player.get_Player_Name()) != 0)
			{
				count++;
			}
		}
	}
	if(count == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void subRoom::add_Hand_Count()
{
	hand_Count++;
}
int subRoom::get_Hand_Count()
{
	return hand_Count;
}
void subRoom::set_Player_Hand(Player player)
{
	for(int i=0; i<8; i++)
	{
		if(room_Player[i].get_Player_Name().compare(player.get_Player_Name()) == 0)
		{
			room_Player[i].hand = player.hand;
		}
	}
}
int subRoom::return_Last_Pop(int num)
{
	return room_Player[num].get_Last_Pop();
}
#endif /* SUB_ROOM_H_ */
