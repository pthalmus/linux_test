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
#include<cstring>

int calculrating(std::vector<int> pop_num)
{
	int temp=0;
	int check=0;
	sort(pop_num.rbegin(), pop_num.rend());
	for(std::vector<int>::iterator iter = pop_num.begin(); iter!=pop_num.end(); iter++)
	{
		if(iter > temp)
		{
			temp = *iter;
		}
		else if(iter == temp)
		{
			check = 1;
		}
		else if(temp > iter && check == 1)
		{
			temp = *iter;
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
