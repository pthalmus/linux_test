/*
 * main_Room.cpp
 *
 *  Created on: 2022. 1. 6.
 *      Author: s9342931
 */

#include<iostream>
#include<vector>
#include<algorithm>
#include<string.h>
#include<sub_Room.h>

class main_Room {
private:
	std::vector<subRoom> room_Vector;
	std::vector<int> room_num;
public:
	char show_Room_Detail();
	void create_Sub_Room(char* msg);
};

char main_Room::show_Room_Detail()
{
	char details[100];


	return details;
}
void main_Room::create_Sub_Room(char* msg)
{
	char* ptr;
	char* context = NULL;
	std::vector<char*> input_Item;
	char temp[15];
	int count_Details = 0;

	ptr = strtok_r(msg, "&", &context);
	while (ptr != NULL)
	{
		ptr = strtok_r(NULL, "&", &context);
		input_Item.push_back(ptr);
	}
	count_Details = input_Item.size();
	if(count_Details == 3)
	{
		subRoom sb = subRoom(atoi(input_Item.at(0)),input_Item.at(1),atoi(input_Item.at(2)));
		room_Vector.push_back(sb);
	}
	else if( count_Details == 4)
	{
		subRoom sb = subRoom(atoi(input_Item.at(0)),input_Item.at(1), input_Item.at(2),atoi(input_Item.at(3)));
		room_Vector.push_back(sb);
	}

}
