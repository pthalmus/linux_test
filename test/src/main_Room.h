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
	main_Room(){};
	void show_Room_Detail(int room_Num, char* detail);
	void create_Sub_Room(char* msg);
	void minus_Room_Max_Player(int room_Num);
	void enter_Room(int room_Num, Player player, char* msg);
	void out_Room(int room_Num, Player player);
	void set_pop(Player player);
};

void main_Room::show_Room_Detail(int room_Num, char* detail)
{
	char pwd;
	int room_Max_People;
	char* room_Name;
	for (int i = 1; i < max_room; i++)
	{
		if (sbroom[i].get_Room_Number() == room_Num)
		{
			if (sbroom[i].get_Room_PWD() == 0)
			{
				pwd = 'Y';
			}
			else
			{
				pwd = 'N';
			}
			room_Max_People = sbroom[i].get_Max_Player();
			room_Name = sbroom[i].get_Room_Name();
			sprintf(detail, "g&%d&%s&%c&%d", room_Num, room_Name, pwd, room_Max_People);
			break;
		}
	}
}
void main_Room::create_Sub_Room(char* msg)
{
	char* ptr;
	char* context = NULL;
	std::vector<char*> input_Item;
	int count_Details = 0;
	int count = 0;


	for (int i = 1; i < max_room; i++)
	{
		if (sbroom[i].get_Room_Number() == 0)
		{
			count = i;
			break;
		}
	}

	ptr = strtok(msg, "&");
	ptr = strtok(NULL, "&");
	while (ptr != NULL)
	{
		input_Item.push_back(ptr);
		ptr = strtok(NULL, "&");
	}
	count_Details = input_Item.size();
	if (count_Details == 3)
	{
		sbroom[count] = subRoom(atoi(input_Item.at(0)), input_Item.at(1), atoi(input_Item.at(2)));
		printf("%d번방 %s이(가) 생성되었습니다\n", atoi(input_Item.at(0)), input_Item.at(1));
	}
	else if (count_Details == 4)
	{
		sbroom[count] = subRoom(atoi(input_Item.at(0)), input_Item.at(1), atoi(input_Item.at(2)), input_Item.at(3));
		printf("비밀번호가 있는 %d번방 %s이(가) 생성되었습니다\n", atoi(input_Item.at(0)), input_Item.at(1));
	}
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
}
void main_Room::set_pop(Player player)
{
	sbroom[player.get_Room_Num()].push_Pop(player.get_Last_Pop());
}



#endif /* MAIN_ROOM_H_ */
