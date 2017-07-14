/*
 * main.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */


#include <stdio.h>
#include <unistd.h>
#include "gtest/gtest.h"

int main(int argc, char *argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
