/*
 * NetEase.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: sheldon
 */

#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <cstdlib>

#include <vector>
#include <iostream>
using namespace std;
#include "NetEase.h"

NetEase::NetEase() {

}

NetEase::~NetEase() {

}

vector<int> NetEase::operationSequece(vector<int> &s) {
	int n = s.size();
	vector<int> result;
	for(int i = n - 1; i >= 0; i -= 2) {
		result.push_back(s[i]);
	}
	for(int i = 1; i < n - 1; i += 2) {
		result.push_back(s[i]);
	}
	return result;
}


int NetEase::independentYi(int x, int f, int d, int p) {
	bool maxF = f*x >= d;
	if(maxF) {
		return (int)floor(d/x);
	} else {
		return f + (int)floor((d - f * x)/(p + x));
	}
}
