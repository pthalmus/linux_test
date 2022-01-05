#include<iostream>
#include<time.h>
/*
 * game.cpp
 *
 *  Created on: 2022. 1. 4.
 *      Author: s9342931
 */
int main()
{
	int random_num_array[6];
	srand(time(NULL));

	for(int i=0; i <sizeof(random_num_array)/sizeof(random_num_array[0]); i++)
	{
		random_num_array[i] = (rand()%15)+1;
		for(int j=0; j<i; j++)
		{
			if(random_num_array[i] == random_num_array[j] )
			{
				i--;
			}
		}
	}
	return random_num_array;
}



