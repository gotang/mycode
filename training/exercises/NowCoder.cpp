/*
 * NowCoder.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */
#include <vector>
#include <set>
#include <iostream>
using namespace std;

#include "NowCoder.h"

int NowCoder::buyHat(int nthCheapest) {
	int n = 0;
	std::cout << "How many hats:";
	std::cin >> n;
	std::cout << "There's " << n << " hats" << std::endl;

	int p;
	std::set<int> prices;
	std::cout << "Prices:";
	for(int i = 0; i < n; i ++) {
		std::cin >> p;
		auto ret = prices.insert(p);
		if(ret.second == false) {
			std::cout << "prices " << p << " has already existed\n";
		}
	}
	if(prices.size() >= nthCheapest) {
		auto it = prices.begin();
		std::advance(it, 2);
		return *(it);
	} else {
		return -1;
	}
}

bool NowCoder::find(int target, vector<vector<int> > array) {
	bool found = false;
	return found;
}

int NowCoder::countBitDiff(int m, int n) {
	m = m^n;
	int count = 0;
	while(m) {
		count ++;
		m = m & (m-1);
	}
	return count;
}

int NowCoder::calculateMax(vector<int> prices) {
	return 0;
}

