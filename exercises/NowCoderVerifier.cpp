/*
 * NowCoderVerifier.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <iostream>

#include "gtest/gtest.h"
using namespace std;

#include "NowCoder.h"

TEST(NowCoder, BuyHats){
}

TEST(NowCoder, countBitDiff){
	NowCoder n;
	EXPECT_EQ(n.countBitDiff(1999, 2299), 7);
}
