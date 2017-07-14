/*
 * Common.h
 *
 *  Created on: Jun 19, 2017
 *      Author: sheldon
 */

#include <iostream>
#include <iterator>

template <typename T>
static void dumpVector(const std::string title, const std::vector<T>& v) {
	if(!title.empty()) {
		printf("%s:", title.c_str());
	}

	std::ostream_iterator<T> out(std::cout, " ");
	copy(v.begin(), v.end(), out);
	std::cout << std::endl;
}

static void dumpVector(const std::string title, const std::vector<char>& v) {
	if(!title.empty()) {
		printf("%s:", title.c_str());
	}

	std::ostream_iterator<char> out(std::cout, "");
	copy(v.begin(), v.end(), out);
	std::cout << std::endl;
}

static void dumpMatrix(const std::string title, const std::vector<vector<int> >& v) {
	if(!title.empty()) {
		printf("%s:\n", title.c_str());
	}
	std::ostream_iterator<int> out(std::cout, " ");
	for(int i = 0; i < v.size(); i ++) {
		copy(v[i].begin(), v[i].end(), out);
		std::cout << std::endl;
	}
}
