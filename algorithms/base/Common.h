/*
 * Common.h
 *
 *  Created on: Jun 19, 2017
 *      Author: sheldon
 */

#include <iostream>
#include <iterator>

template <typename T>
static void dumpVector(const std::string &title, const std::vector<T>& v) {
	if(!title.empty()) {
		printf("%s:", title.c_str());
	}

	std::ostream_iterator<T> out(std::cout, " ");
	copy(v.begin(), v.end(), out);
	std::cout << std::endl;
}

static void dumpVector(const std::string &title, const std::vector<char>& v) {
	if(!title.empty()) {
		printf("%s:", title.c_str());
	}

	std::ostream_iterator<char> out(std::cout, "");
	copy(v.begin(), v.end(), out);
	std::cout << std::endl;
}

template<class Key, class Value>
static void dumpMap(const std::string &title, const std::map<Key, Value>& m) {
	if(!title.empty()) {
		printf("%s:\n", title.c_str());
	}
	typename std::map<Key, Value>::const_iterator it;
	int i = 0;
	for(it = m.begin(); it != m.end(); ++it, ++i)
		cout << i << ":[" << it->first << "][" << it->second << "]" << endl;
}

static void printDelimiterLine(const std::string &str, int len = 80) {
	std::cout << std::endl;

	int prefix = (len - str.size())/2;
	int suffix = len - prefix - str.size();
	for(int i = 0; i < prefix - 1; i ++) {
		std::cout << "*";
	}
	std::cout << " " << str << " ";

	for(int i = 0; i < suffix - 1; i ++) {
		std::cout << "*";
	}
	std::cout << std::endl;
}
