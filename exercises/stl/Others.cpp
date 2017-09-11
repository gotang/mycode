/*
 * Others.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: sheldon
 */

#include <limits>
#include <array>
#include <algorithm>
#include <iostream>
using namespace std;

#include "Common.h"
#include "gtest/gtest.h"

TEST(Others, limits) {
	cout << "limits of int:" << endl;
	cout << "is_specialized:" << numeric_limits<int>::is_specialized << endl;
	cout << "digits:" << numeric_limits<int>::digits << endl;
	cout << "digits10:" << numeric_limits<int>::digits10 << endl;
	cout << "max_digits10:" << numeric_limits<int>::max_digits10 << endl;
	cout << "min:" << numeric_limits<int>::min() << endl;
	cout << "max:" << numeric_limits<int>::max() << endl;
	cout << "is_signed:" << numeric_limits<int>::is_signed << endl;
	cout << "radix:" << numeric_limits<int>::radix << endl;
	cout << "epsilon:" << numeric_limits<int>::epsilon() << endl;

	cout << "min_exponent:" << numeric_limits<int>::min_exponent << endl;
	cout << "min_exponent10:" << numeric_limits<int>::min_exponent10 << endl;
	cout << "max_exponent:" << numeric_limits<int>::max_exponent << endl;
	cout << "max_exponent10:" << numeric_limits<int>::max_exponent10 << endl;
	cout << "has_infinity:" << numeric_limits<int>::has_infinity << endl;
	cout << "infinity:" << numeric_limits<int>::infinity() << endl;
	cout << "has_quiet_NaN:" << numeric_limits<int>::has_quiet_NaN << endl;
	cout << "quiet_NaN:" << numeric_limits<int>::quiet_NaN() << endl;
	cout << "has_signaling_NaN:" << numeric_limits<int>::has_signaling_NaN << endl;
	cout << "signaling_NaN:" << numeric_limits<int>::signaling_NaN() << endl;
	cout << "has_denorm:" << numeric_limits<int>::has_denorm << endl;
	cout << "denorm_min:" << numeric_limits<int>::denorm_min() << endl;
	cout << "has_denorm_loss:" << numeric_limits<int>::has_denorm_loss << endl;

	cout << "is_iec559:" << numeric_limits<int>::is_iec559 << endl;
	cout << "is_bounded:" << numeric_limits<int>::is_bounded << endl;
	cout << "is_modulo:" << numeric_limits<int>::is_modulo << endl;
	cout << "traps:" << numeric_limits<int>::traps << endl;
	cout << "tinyness_before:" << numeric_limits<int>::tinyness_before << endl;
	cout << "round_style:" << numeric_limits<int>::round_style << endl;


	cout << endl << "limits of float:" << endl;
	cout << "is_specialized:" << numeric_limits<float>::is_specialized << endl;
	cout << "digits:" << numeric_limits<float>::digits << endl;
	cout << "digits10:" << numeric_limits<float>::digits10 << endl;
	cout << "max_digits10:" << numeric_limits<float>::max_digits10 << endl;
	cout << "min:" << numeric_limits<float>::min() << endl;
	cout << "max:" << numeric_limits<float>::max() << endl;
	cout << "is_signed:" << numeric_limits<float>::is_signed << endl;
	cout << "radix:" << numeric_limits<float>::radix << endl;
	cout << "epsilon:" << numeric_limits<float>::epsilon() << endl;

	cout << "min_exponent:" << numeric_limits<float>::min_exponent << endl;
	cout << "min_exponent10:" << numeric_limits<float>::min_exponent10 << endl;
	cout << "max_exponent:" << numeric_limits<float>::max_exponent << endl;
	cout << "max_exponent10:" << numeric_limits<float>::max_exponent10 << endl;
	cout << "has_infinity:" << numeric_limits<float>::has_infinity << endl;
	cout << "infinity:" << numeric_limits<float>::infinity() << endl;
	cout << "has_quiet_NaN:" << numeric_limits<float>::has_quiet_NaN << endl;
	cout << "quiet_NaN:" << numeric_limits<float>::quiet_NaN() << endl;
	cout << "has_signaling_NaN:" << numeric_limits<float>::has_signaling_NaN << endl;
	cout << "signaling_NaN:" << numeric_limits<float>::signaling_NaN() << endl;
	cout << "has_denorm:" << numeric_limits<float>::has_denorm << endl;
	cout << "denorm_min:" << numeric_limits<float>::denorm_min() << endl;
	cout << "has_denorm_loss:" << numeric_limits<float>::has_denorm_loss << endl;

	cout << "is_iec559:" << numeric_limits<float>::is_iec559 << endl;
	cout << "is_bounded:" << numeric_limits<float>::is_bounded << endl;
	cout << "is_modulo:" << numeric_limits<float>::is_modulo << endl;
	cout << "traps:" << numeric_limits<float>::traps << endl;
	cout << "tinyness_before:" << numeric_limits<float>::tinyness_before << endl;
	cout << "round_style:" << numeric_limits<float>::round_style << endl;
}

TEST(Others, algorithms) {
	{
		cout << endl << "all of:" << endl;
		std::array<int,8> foo = {3,5,7,33,13,17,19,23};

		if ( std::all_of(foo.begin(), foo.end(), [](int i) {return i%2;}) )
			std::cout << "All the elements are odd numbers.\n";

		sort(foo.begin(), foo.end(), [](int i, int j){return i > j;});
		ostream_iterator<int> out(cout, " ");
		copy(foo.begin(), foo.end(), out);
	}

	{

		cout << endl << "binary search:" << endl;
		int myints[] = {1,2,3,4,5,4,3,2,1};
		std::vector<int> v(myints,myints+9);                // 1 2 3 4 5 4 3 2 1

		// using default comparison:
		std::sort (v.begin(), v.end());

		std::cout << "looking for a 3... ";
		if (std::binary_search (v.begin(), v.end(), 3))
			std::cout << "found!\n";
		else
			std::cout << "not found.\n";

		// using myfunction as comp:
		std::sort (v.begin(), v.end(), [](int i, int j){return i > j;});

		std::cout << "looking for a 2... ";
		dumpVector(" ", v);
		if (std::binary_search (v.begin(), v.end(), 2, [](int i, int j){return i > j;}))
			std::cout << "found!\n";
		else
			std::cout << "not found.\n";
	}

	cout << endl << "all of:" << endl;
}


