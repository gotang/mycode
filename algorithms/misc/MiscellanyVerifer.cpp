/*
 * AlgorithmsVerifier.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: sheldon
 */

#include <sys/time.h>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "gtest/gtest.h"
#include "base/Common.h"
#include "Miscellany.h"

TEST(Miscellany, permutation) {
	Miscellany<int> a;

	std::vector<int> list {1, 2, 3, 4};

	dumpVector("orignal", list);
	a.permutation(list);
}
