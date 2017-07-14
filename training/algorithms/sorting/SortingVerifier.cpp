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
#include "Sorting.h"

template <typename T>
static bool veriferSortAscending(const std::vector<T>& v, int n = 0) {
	for(int i = n + 1; i < v.size(); i ++) {
		if(v[i - 1] > v[i])
			return false;
	}
	return true;
}

class Data {
public:
	Data() {
		size = 0;
	}
	void dataAt(int index, std::vector<int> & data) {
		struct timeval tv;
		gettimeofday(&tv, NULL);

		srandom((int)tv.tv_usec + getpid());
		if(index == 0) {
			for(int i = 0; i < size; i ++) {
				data.push_back((int)(100 * (random()/(RAND_MAX + 1.0))));
			}
		} else if (index == 1) {
			int value = (int)(100 * (random()/(RAND_MAX + 1.0 )));
			for(int i = 0; i < size; i ++) {
				data.push_back(value --);
			}
		} else if (index == 2) {
			int value = (int)(100 * (random()/(RAND_MAX + 1.0 )));
			for(int i = 0; i < size; i ++) {
				data.push_back(value ++);
			}
		}
		data.resize(size);
	}
	std::string dataAttribute(int index) {
		std::string str;
		if(index == 0) {
			str.assign("random");
		} else if (index == 1) {
			str.assign("descending");
		} else if (index == 2) {
			str.assign("ascending");
		} else {
			str.assign("null");
		}
		return str;
	}
	void setSize(int len) {
		size = len;
	}
private:
	size_t size;
};

TEST(Sorting, selection) {
	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);

		std::vector<int> sort;
		sort.assign(v.begin(), v.end());
		s.selection(sort);
		dumpVector(std::string("selection"), sort);
		EXPECT_TRUE(veriferSortAscending(sort));
	}
}

TEST(Sorting, insertion) {
	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);
		s.insertion(v);
		dumpVector(std::string("insertion"), v);
		EXPECT_TRUE(veriferSortAscending(v));
	}
}

TEST(Sorting, bubble) {
	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);

		//
		s.bubble(v);
		dumpVector(std::string("bubble   "), v);
		EXPECT_TRUE(veriferSortAscending(v));
	}
}

TEST(Sorting, shell) {
	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);

		s.shell(v);
		dumpVector(std::string("shell    "), v);
		EXPECT_TRUE(veriferSortAscending(v));
	}
}

TEST(Sorting, merge) {
	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);

		//
		s.merge(v);
		dumpVector(std::string("merge    "), v);
		EXPECT_TRUE(veriferSortAscending(v));
	}
}

TEST(Sorting, quick) {

	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);

		s.quick(v);
		dumpVector(std::string("quick    "), v);
		EXPECT_TRUE(veriferSortAscending(v));
	}
}

TEST(Sorting, heap) {

	Data d;
	d.setSize(21);

	Sorting<int> s;
	std::vector<int> v;
	for(int i = 0; i < 3; i ++) {
		v.clear();
		d.dataAt(i, v);
		std::cout << "\nTesting " << d.dataAttribute(i) << " data:" <<std::endl;
		dumpVector(std::string("original "), v);

		s.heap(v);
		dumpVector(std::string("heap     "), v);
		EXPECT_TRUE(veriferSortAscending(v, 1));
	}
}

TEST(Sorting, Heap) {
	Heap<char> h;
	std::vector<char> v;
	std::string str("0SORTEXAMPLE");

	v.assign(str.begin(), str.end());
	h.dump("origin", v);

	h.build(v);
	h.dump("build", v);
	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0XTSPLRAMOEE"), 0);
	std::cout << std::endl;

	v.at(11) = 'Y';
	h.swim(v, 11);
	h.dump("swim", v);
	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0YXSPTRAMOEL"), 0);

	std::cout << std::endl;
	v.at(1) = 'E';
	h.sink(v, 1);
	h.dump("sink", v);
	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0XTSPLRAMOEE"), 0);

	std::cout << std::endl;
	h.sort(v);
	h.dump("sort", v);

	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0AEELMOPRSTX"), 0);

	h.build(v);
	h.dump("build", v);
	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0XTPSMOERLEA"), 0);

	h.insert(v, 'Y');
	h.dump("insert", v);
	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0YTXSMPERLEAO"), 0);

	h.remove(v);
	h.dump("remove", v);
	EXPECT_EQ(std::string(v.begin(), v.end()).compare("0XTPSMOERLEA"), 0);
}
