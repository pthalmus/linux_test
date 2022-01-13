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

class Player {
private:
	int player_num;
	int room_Number = 0;
	char* player_Name;
	int last_pop;
	int win_Stack = 0;
	int bot_Count = 0;
	std::vector<int> hand;
public:
	void pop(int index);
	void add(int add);
	std::vector<int> get_Hand();
	int get_Player_Num();
	void set_Player_Num(int num);
	char* get_Player_Name();
	void set_Player_Name(char* name);
	int get_Last_Pop();
	int get_Win_Stack();
	void up_Win_Stack();
	void set_Room_Num(int num);
	int get_Room_Num();
	void set_Random_Hand();
	int pop_Random(int num, int token);
	void set_Bot_Start();
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
}
void Player::add(int add)
{
	hand.push_back(add);
	sort(hand.begin(), hand.end());
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
char* Player::get_Player_Name()
{
	return player_Name;
}
void Player::set_Player_Name(char* name)
{
	player_Name = name;
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
	int count = 6;
	int check = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, count-1);


	while (1)
	{
		item = dis(gen);
		if (token == 0 && hand.at(item) >= num) // token == 0 : above
		{
			pop(hand.at(item));
			count--;
			return last_pop;
		}
		else if (token == 1 && hand.at(item) <= num) // token == 1 : below
		{
			pop(hand.at(item));
			count--;
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
	return 0;
}
void Player::set_Bot_Start()
{
	this->set_Random_Hand();
	bot_Count =1;
}


#endif /* PLAYER_H_ */
