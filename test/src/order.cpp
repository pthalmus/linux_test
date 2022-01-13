/*
 * order.cpp
 *
 *  Created on: 2022. 1. 13.
 *      Author: s9342931
 */
#include<random>

class order
{
private:
	int criterion = 0;
	int UP_or_DOWN = 0;
public:
	order()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(3, 12);

		criterion = dis(gen);

		std::random_device md;
		std::mt19937 men(md());
		std::uniform_int_distribution<int> mis(0, 1);
		UP_or_DOWN = mis(men);
	}
	int get_Criterion();
	int get_UP_or_DOWN();
	void reOrder();
};

int order::get_Criterion()
{
	return criterion;
}
int order::get_UP_or_DOWN()
{
	return UP_or_DOWN;
}
void order::reOrder()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(3, 12);

	criterion = dis(gen);

	std::random_device md;
	std::mt19937 men(md());
	std::uniform_int_distribution<int> mis(0, 1);
	UP_or_DOWN = mis(men);
}



