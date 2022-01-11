/*
 * main_Room.cpp
 *
 *  Created on: 2022. 1. 6.
 *      Author: s9342931
 */

#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<sub_Room.h>

class main_Room {
private:
	std::vector<subRoom> room_Vector;
	std::vector<int> room_num;
public:
	const char* show_Room_Detail(int room_Num);
	void create_Sub_Room(char* msg);
};

const char* main_Room::show_Room_Detail(int room_Num)
{
	char details[100];

	for (std::vector<int>::iterator iter = room_num.begin(); iter != room_num.end(); iter++)
	{
		if (*iter == room_Num)
		{
			subRoom temp_Room = room_Vector.at(room_Num);
			strcpy_s(details, temp_Room.get_detail().c_str());
			break;
		}
	}
	return details;
}
void main_Room::create_Sub_Room(char* msg)
{
	char* ptr;
	char* context = NULL;
	std::vector<char*> input_Item;
	char temp[15];

	ptr = strtok_s(msg, "&", &context);
	while (ptr != NULL)
	{
		ptr = strtok_s(NULL, "&", &context);
		input_Item.push_back(ptr);
	}
}
