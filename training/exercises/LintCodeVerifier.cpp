/*
 * LintCodeVerifier.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: sheldon
 */


#include "gtest/gtest.h"

#include "LintCode.h"

TEST(LintCode, digitCounts) {
	LintCode l;
	EXPECT_EQ(l.digitCounts(1, 12), 5);
}

TEST(LintCode, trailingZeros) {
	LintCode l;
	EXPECT_EQ(l.trailingZeros(10), 2);
	EXPECT_EQ(l.trailingZeros(25), 6);
	EXPECT_EQ(l.trailingZeros(126), 31);
}

TEST(LintCode, aplusb) {
	LintCode l;
	EXPECT_EQ(l.aplusb(1, 2), 3);
}
