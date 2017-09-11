/*
 * NetEaseVerifier.cpp
 *
 *  Created on: Sep 4, 2017
 *      Author: sheldon
 */


#include <vector>
#include <iostream>
using namespace std;

#include "gtest/gtest.h"

#include "base/Common.h"
#include "NetEase.h"

TEST(NetEase, operationSequece) {
	NetEase N;
	vector<int> v;
	for(int i = 0; i < 5; i ++) {
		v.push_back(i);
	}
	dumpVector("result", N.operationSequece(v));
}

TEST(NetEase, independentYi) {
	NetEase N;
	EXPECT_EQ(N.independentYi(3, 5, 100, 10), 11);
}
