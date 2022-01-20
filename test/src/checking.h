/*
 * checking.h
 *
 *  Created on: 2022. 1. 5.
 *      Author: s9342931
 */

#ifndef CHECKING_H_
#define CHECKING_H_

#include<vector>
#include<algorithm>

int calculrating(std::vector<int> pop_num)
{
	int temp=0;
	int check=0;
	sort(pop_num.rbegin(), pop_num.rend());
	for(int i =0; i < pop_num.size(); i++)
	{
		if(pop_num.at(i) > temp)
		{
			temp = pop_num.at(i);
		}
		else if(pop_num.at(i) == temp)
		{
			check = 1;
		}
		else if(temp > pop_num.at(i) && check == 1)
		{
			temp = pop_num.at(i);
			check = 0;
		}
	}
	if(check == 1)
	{
		temp = 20;
	}
	return temp;
}

#endif /* CHECKING_H_ */
