/*
 * checking.cpp
 *
 *  Created on: 2022. 1. 4.
 *      Author: s9342931
 */
#include<iostream>
#include<get_random_array.h>
#include<vector>
#include<algorithm>
#include<cstring>

class Player {
private:
	int player_num;
	std::vector<int> hand;
public:
	Player(int num)
	{
		this->player_num = num;
	}
	void pop(int index)
	{
		for(std::vector<int>::iterator iter = hand.begin(); iter!=hand.end(); iter++)
		{
			if(*iter == index)
			{
				hand.erase(iter);
				break;
			}
		}
	}
	void add(int add)
	{
		hand.push_back(add);
		sort(hand.begin(), hand.end());
	}
	std::vector<int> get_Hand()
	{
		return hand;
	}
};
char calculrating(std::vector<int> pop_num)
{
	int temp=0;
	int temp2=0;
	for(std::vector<int>::iterator iter = pop_num.begin(); iter!=pop_num.end(); iter++)
	{
		if(iter > temp)
		{
			temp2 = temp;
			temp = *iter;
		}
		else if(temp == *iter)
		{
			temp = temp2;
		}
	}
	std::string toS = std::to_string(temp);
	char message[30] = "win number is ";
	strcat(message, toS.c_str());
	return message;
}

