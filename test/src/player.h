/*
 * player.h
 *
 *  Created on: 2022. 1. 5.
 *      Author: s9342931
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include<vector>
#include<algorithm>
#include<random>
#include<string.h>

class Player {
public:
	int player_num = 0;
	int room_Number = 0;
	std::string player_Name;
	int last_pop = 0;
	int win_Stack = 0;
	int bot_Count = 0;
	std::vector<int> hand;
	bool loosing = false;

	void pop(int index);
	void add(int add);
	void set_Hand(std::vector<int> change);
	std::vector<int> get_Hand();
	int get_Player_Num();
	void set_Player_Num(int num);
	std::string get_Player_Name();
	void set_Player_Name(std::string name);
	int get_Last_Pop();
	int get_Win_Stack();
	void up_Win_Stack();
	void set_Room_Num(int num);
	int get_Room_Num();
	void set_Random_Hand();
	int pop_Random(int num, int token);
	void set_Bot_Start();
	void refresh_Hand();
	void set_Bot_End();
	int check_Bot();
};

void Player::pop(int index)
{
	for (std::vector<int>::iterator iter = hand.begin(); iter != hand.end(); iter++)
	{
		if (*iter == index)
		{
			hand.erase(iter);
			this->last_pop = index;
			break;
		}
		else
		{
			last_pop = 0;
		}
	}
	if(last_pop == 0)
	{
		loosing = true;
		refresh_Hand();
	}
}
void Player::add(int add)
{
	hand.push_back(add);
	sort(hand.begin(), hand.end());
}
void Player::set_Hand(std::vector<int> change)
{
	hand = change;
}
std::vector<int> Player::get_Hand()
{
	return hand;
}
int Player::get_Player_Num()
{
	return player_num;
}
void Player::set_Player_Num(int num)
{
	this->player_num = num;
}
std::string Player::get_Player_Name()
{
	return player_Name;
}
void Player::set_Player_Name(std::string name)
{
	if(name.size() == 0)
	{
		player_Name = "";
	}
	else
	{
		player_Name = name;
	}
}
int Player::get_Last_Pop()
{
	return last_pop;
}
int Player::get_Win_Stack()
{
	return win_Stack;
}
void Player::up_Win_Stack()
{
	this->win_Stack++;
}
void Player::set_Room_Num(int num)
{
	this->room_Number = num;
}
int Player::get_Room_Num()
{
	return room_Number;
}


void Player::set_Random_Hand()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(1, 15);

	for (int i = 0; i < 6; i++)
	{
		hand.push_back(dis(gen));
		for (int j = 0; j < i; j++)
		{
			if (hand[i] == hand[j])
			{
				hand.pop_back();
				i--;
			}
		}
	}
	std::sort(hand.begin(), hand.end());
}
int Player::pop_Random(int num, int token)
{
	int item;
	int count = hand.size();
	int check = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, count - 1);


	while (1)
	{
		item = dis(gen);
		if (token == 0 && hand.at(item) >= num) // token == 0 : above
		{
			pop(hand.at(item));
			return last_pop;
		}
		else if (token == 1 && hand.at(item) <= num) // token == 1 : below
		{
			pop(hand.at(item));
			return last_pop;
		}
		else if(token ==2 && hand.at(item)%2 == 1)
		{
			pop(hand.at(item));
			return last_pop;
		}
		else if(token == 3 && hand.at(item)%2 == 0)
		{
			pop(hand.at(item));
			return last_pop;
		}
		else
		{
			check++;
		}
		if (check > count * 2)
		{
			break;
		}
	}
	loosing = true;
	return 0;
}
void Player::set_Bot_Start()
{
	this->set_Random_Hand();
	bot_Count = 1;
}
void Player::refresh_Hand()
{
	std::vector<int>().swap(hand);
}
void Player::set_Bot_End()
{
	this->refresh_Hand();
	bot_Count = 0;
}
int Player::check_Bot()
{
	return bot_Count;
}

#endif /* PLAYER_H_ */
