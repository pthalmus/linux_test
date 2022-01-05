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

class Player {
private:
	int player_num;
	char player_Name[];
	int last_pop;
	int win_Stack=0;
	std::vector<int> hand;
public:
	void pop(int index);
	void add(int add);
	std::vector<int> get_Hand();
	void set_Hand(std::vector<int> submit_hand);
	int get_Player_Num();
	void set_Player_Num(int num);
	char get_Player_Name();
	void set_Player_Name(char name[]);
	int get_Last_Pop();
	int get_Win_Stack();
	void up_Win_Stack();

};

void Player::pop(int index)
{
	for(std::vector<int>::iterator iter = hand.begin(); iter!=hand.end(); iter++)
	{
		if(*iter == index)
		{
			hand.erase(iter);
			this->last_pop = index;
			break;
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
void Player::set_Hand(std::vector<int> submit_hand)
{
	hand.swap(submit_hand);
}
int Player::get_Player_Num()
{
	return player_num;
}
void Player::set__Player_Num(int num)
{
	this->player_num = num;
}
char Player::get_Player_Name()
{
	return player_Name;
}
void Player::set_Player_Name(char name[])
{
	this->player_Name = name;
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



#endif /* PLAYER_H_ */
