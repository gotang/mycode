/*
 * NumberTheoryVerifier.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: sheldon
 */

#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "base/Common.h"

#include "NumberTheory.h"

TEST(NumberTheory, leastCommonMultiple) {
	NumberTheory n;
	EXPECT_EQ(n.leastCommonMultiple(2, 3), 6);
	EXPECT_EQ(n.leastCommonMultiple(2, 4), 4);
	EXPECT_EQ(n.leastCommonMultiple(2, 0), 0);
}

TEST(NumberTheory, greatestCommonDivisor) {
	NumberTheory n;
	EXPECT_EQ(n.greatestCommonDivisor(0, 2), 2);
	EXPECT_EQ(n.greatestCommonDivisor(2, 0), 2);
	EXPECT_EQ(n.greatestCommonDivisor(-12, 18), 6);
	EXPECT_EQ(n.greatestCommonDivisor(-18, 12), -6);
	EXPECT_EQ(n.greatestCommonDivisor(3, 7), 1);
}

TEST(NumberTheory, integerFactorization) {
	NumberTheory n;
	map<int, int> m = n.integerFactorization(12);
	dumpMap("integerFactorization", m);
}
