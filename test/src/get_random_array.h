/*
 * get_random_array.h
 *
 *  Created on: 2022. 1. 4.
 *      Author: s9342931
 */

#ifndef GET_RANDOM_ARRAY_H_
#define GET_RANDOM_ARRAY_H_

#include<iostream>
#include<time.h>
#include<vector>

std::vector<int> get_random()
{
	std::vector<int> random_num;
	srand(time(NULL));

	for(int i=0; i <6; i++)
	{
		random_num.push_back(rand()%15+1);
		for(int j=0; j<i; j++)
		{
			if(random_num[i] == random_num[j] )
			{
				random_num.pop_back();
				i--;
			}
		}
	}
	return random_num;
}
#endif /* GET_RANDOM_ARRAY_H_ */
