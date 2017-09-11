/*
 * BATVerifier.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: sheldon
 */


#include <vector>
#include <iostream>
using namespace std;

#include "gtest/gtest.h"

#include "base/Common.h"
#include "BAT.h"

TEST(BAT, shuffle) {
	vector<int> v;
	for(int i = 0; i < 18; i ++) {
		v.push_back(i);
	}

	dumpVector("original", v);

	BAT bat;
	bat.shuffle(v);
	dumpVector("shuffled", v);
}
