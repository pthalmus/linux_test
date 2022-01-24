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
#include"checking.h"


class subRoom {
private:
	int room_Number = 0;
	char* room_Pwd = NULL;
	char* room_Name = NULL;
	int cur_Player = 0;
	int max_Player = 8;
	Player room_Player[8];
	int order = 0;
	int token = 0;
	std::vector<int> pop_Inventory;
	int count =0;
	int room_Status =0;
public:
	subRoom() {};
	subRoom(int num, char* name, int pMNum)
	{
		room_Number = num;
		room_Name = name;
		max_Player = pMNum;
	}
	subRoom(int num, char* name, int pMNum, char* pwd )
	{
		room_Number = num;
		room_Name = name;
		room_Pwd = pwd;
		max_Player = pMNum;
	}
	subRoom(const subRoom& origin_Room)
	{
		room_Number = origin_Room.room_Number;
		room_Pwd = origin_Room.room_Pwd;
		room_Name = origin_Room.room_Name;
		max_Player = origin_Room.max_Player;
		for (int i = 0; i < max_Player; i++)
		{
			room_Player[i] = origin_Room.room_Player[i];
		}
	}
	~subRoom() {};
	bool add_Player(Player add_Player, char* msg);
	void pop_Player(Player pop_Player);
	void block_Player();
	void free_Block_Player();
	bool invite_Player(Player online_Player);
	int get_Room_Number();
	int get_Room_PWD();
	int get_Max_Player();
	char* get_Room_Name();
	void set_Order();
	int get_Order();
	int get_Token();
	void push_Pop(int num);
	int get_Cur_Player();
	void set_Room_Status(int status);
	int get_Room_Status();
};

bool subRoom::add_Player(Player add_player, char* msg)
{
	if (this->room_Pwd == NULL && cur_Player < max_Player)
	{
		for (int i = 0; i < max_Player; i++)
		{
			if (room_Player[i].get_Player_Name() == NULL)
			{
				room_Player[i] = add_player;
				cur_Player++;
				break;
			}
		}
	}
	else if(this->room_Pwd !=NULL && strcmp(room_Pwd,msg) == 0 && cur_Player < max_Player)
	{
		for (int i = 0; i < max_Player; i++)
		{
			if (room_Player[i].get_Player_Name() == NULL)
			{
				room_Player[i] = add_player;
				cur_Player++;
				break;
			}
		}
	}

	return false;
}
void subRoom::pop_Player(Player pop_player)
{
	for (int i = 0; i < max_Player; i++)
	{
		if (room_Player->get_Player_Name() == pop_player.get_Player_Name())
		{
			while (i < max_Player - 1)
			{
				room_Player[i] = room_Player[i + 1];
				i++;
			}
			cur_Player--;
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
	if (max_Player <= 7)
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
	if (room_Pwd == NULL)
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
void subRoom::push_Pop(int num)
{
	pop_Inventory.push_back(num);
	count++;
	int winner =0;


	if(count == max_Player)
	{
		winner = calculrating(pop_Inventory);
		std::vector<int>().swap(pop_Inventory);
		count =0;
	}
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



#endif /* SUB_ROOM_H_ */
