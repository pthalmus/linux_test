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
	int create_Sub_Room(Json::Value root);
	void enter_Room(int room_Num, Player player);
	void out_Room(int room_Num, Player player);
	std::string set_pop(Player player);
	bool check_Room(int room_Num);
	void game_Start(int room_Num);
	void game_End(int room_Num);
	bool check_Status(int room_Num);

	std::string get_room_Name(int room_Num);
	std::string get_room_PW(int room_Num);
	std::string get_room_Cur_Player(int room_Num);
	std::string get_room_Max_Player(int room_Num);
	std::string get_room_Player(int room_Num, int count);
	std::string get_room_Host(int room_Num);
	std::string get_order(int room_Num);
	std::string get_token(int room_Num);

	void Win(int room_Num, std::string value);
	std::vector<int> return_Hand(int room_Num, Player player);


	int check_room_Player(int room_Num,int count);
	void lock_Room(int room_Num, int point);
	void unlock_Room(int room_Num, int point);
	void add_BOT(int room_Num, int point);
	void pop_BOT(int room_Num, int point);
	void change_Host(int room_Num, int point);
	void set_order(int room_Num);
	void add_Hand(int room_Num, Player player);
	bool check_Hand_Count(int room_Num);
	int return_last_Pop(int room_Num, int num);
	void re_Pop_Bot(int room_Num);
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
int main_Room::create_Sub_Room(Json::Value root)
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
	if (root["roomPW"].asString().size() == 0)
	{
		sbroom[count] = subRoom(count, root["roomName"].asString(),stoi(root["maxPlayer"].asString()));
		printf("%d번방 %s(이)가 생성되었습니다\n", count, (char*)root["roomName"].asCString());
		return count;
	}
	else
	{
		sbroom[count] = subRoom(count, root["roomName"].asString(),stoi(root["maxPlayer"].asString()),root["roomPW"].asString());
		printf("비밀번호가 있는 %d번방 %s(이)가 생성되었습니다\n", count, (char*)root["roomName"].asCString());
		return count;
	}
	return 0;
}
void main_Room::enter_Room(int room_Num, Player player)
{
	sbroom[room_Num].add_Player(player);
}
void main_Room::out_Room(int room_Num, Player player)
{
	if(room_Num != 0)
	{
		if( sbroom[room_Num].toss_Host(player) )
		{
			sbroom[room_Num].pop_Player(player);
		}
		else if( sbroom[room_Num].check_BOT(player) )
		{
			sbroom[room_Num].pop_Player(player);
			sbroom[room_Num] = subRoom();

		}
		else
		{
			sbroom[room_Num].pop_Player(player);
		}
	}


}
std::string main_Room::set_pop(Player player)
{
	int return_Item;
	std::string SItem;
	std::string output = sbroom[player.get_Room_Num()].push_Pop(player.get_Last_Pop(), player);
	if(output.size() != 0)
	{
		SItem = output;
		std::cout<<SItem<<"\n";
		output+="&";
		output+= sbroom[player.get_Room_Num()].add_Item(SItem);
	}
	return output;
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
	if(sbroom[room_Num].get_Cur_Player() == sbroom[room_Num].get_Max_Player())
	{
		return false;
	}
	else if(sbroom[room_Num].get_Room_Status() == 0)
	{
		return true;
	}
	return false;
}

std::string main_Room::get_room_Name(int room_Num)
{
	return sbroom[room_Num].get_Room_Name();
}
std::string main_Room::get_room_PW(int room_Num)
{
	return sbroom[room_Num].get_Room_PWD();
}
std::string main_Room::get_room_Cur_Player(int room_Num)
{
	return std::to_string(sbroom[room_Num].get_Cur_Player());
}
std::string main_Room::get_room_Max_Player(int room_Num)
{
	return std::to_string(sbroom[room_Num].get_Max_Player());
}
std::string main_Room::get_room_Player(int room_Num,int count)
{
	return sbroom[room_Num].get_Player_Name(count);
}
std::string main_Room::get_room_Host(int room_Num)
{
	return std::to_string(sbroom[room_Num].get_Host());
}
std::string main_Room::get_order(int room_Num)
{
	return std::to_string(sbroom[room_Num].get_Order());
}
std::string main_Room::get_token(int room_Num)
{
	return std::to_string(sbroom[room_Num].get_Token());
}

int main_Room::return_last_Pop(int room_Num, int num)
{
	int last_Pop;
	last_Pop = sbroom[room_Num].return_Last_Pop(num);
	return last_Pop;
}

void main_Room::Win(int room_Num, std::string value)
{
	sbroom[room_Num].add_Item(value);
}
std::vector<int> main_Room::return_Hand(int room_Num, Player player)
{
	return sbroom[room_Num].return_Hand(player);
}
int main_Room::check_room_Player(int room_Num,int count)
{
	ssize_t pos;
	if(get_room_Player(room_Num,count).size() !=0)
	{
		if((pos = get_room_Player(room_Num,count).find("BOT", 0, 3)) != std::string::npos)
		{
			return 2;
		}
		else if(get_room_Player(room_Num,count).compare("Locked") == 0 )
		{
			return 3;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}
void main_Room::lock_Room(int room_Num, int point)
{
	sbroom[room_Num].block_Player(point);
}
void main_Room::unlock_Room(int room_Num, int point)
{
	sbroom[room_Num].free_Block_Player(point);
}
void main_Room::add_BOT(int room_Num, int point)
{
	sbroom[room_Num].add_BOT(point);
}
void main_Room::pop_BOT(int room_Num, int point)
{
	sbroom[room_Num].pop_BOT(point);
}
void main_Room::change_Host(int room_Num, int point)
{
	sbroom[room_Num].set_Host(point);
}
void main_Room::set_order(int room_Num)
{
	sbroom[room_Num].set_Order();
}
void main_Room::add_Hand(int room_Num, Player player)
{
	sbroom[room_Num].set_Player_Hand(player);
	sbroom[room_Num].add_Hand_Count();
}
bool main_Room::check_Hand_Count(int room_Num)
{
	if(sbroom[room_Num].get_Hand_Count() == sbroom[room_Num].get_Cur_Player())
	{
		return true;
	}
	else
	{
		return false;
	}
}
void main_Room::re_Pop_Bot(int room_Num)
{
	sbroom[room_Num].re_Pop_Bot();
}
#endif /* MAIN_ROOM_H_ */
