/*
 * STL.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: sheldon
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <iomanip>
#include <bitset>
#include <list>
#include <unordered_map>
#include <forward_list>
#include <fstream>
#include <iostream>

#include "gtest/gtest.h"

TEST(STL, make_pair) {
	std::pair<int, std::string> mypair;
	mypair = std::make_pair(2, "mypair");
	std::cout << "first:" << mypair.first << std::endl;
	std::cout << "second:" << mypair.second << std::endl;
}

TEST(STL, array) {
	std::array<int,4> myarray = {4, 26, 80, 14};
	std::cout << "size of array:" << sizeof(myarray) << std::endl;

	std::cout << "myarray contains:";
	for ( auto it = myarray.begin(); it < myarray.end(); ++it )
	std::cout << ' ' << *it;

	std::cout << '\n' << "reverse:";
	for ( auto rit = myarray.rbegin(); rit < myarray.rend(); ++rit )
	std::cout << ' ' << *rit;

	std::cout << '\n';
}

TEST(STL, bitset) {
	std::bitset<4> foo (std::string("0001"));

	std::cout << foo.flip(2) << '\n'; // 0101
	std::cout << foo.flip() << '\n';// 1010
}

TEST(STL, vector) {

	std::vector<int> v;
	for(int i = 0; i < 10; i++) {
		v.push_back(i);
	}
	std::cout << "v capacity " << v.capacity() << ", size " << v.size() << std::endl;
	v.resize(8);

	std::cout << "v capacity " << v.capacity() << ", size " << v.size() << std::endl;
}

TEST(STL, string) {

}

TEST(STL, deque) {
	std::deque<int> first;
	std::deque<int> second;
	std::deque<int> third;
	std::deque<int> forth;

	first.assign (7,100); // 7 ints with a value of 100

	std::deque<int>::iterator it;
	it=first.begin()+1;

	second.assign (it,first.end()-1);// the 5 central values of first

	int myints[] = {1776,7,4};
	third.assign (myints,myints+3); // assigning from array.

	std::cout << "Size of first: " << int (first.size()) << '\n';
	std::cout << "Size of second: " << int (second.size()) << '\n';
	std::cout << "Size of third: " << int (third.size()) << '\n';

	forth.push_back(1);
	forth.push_back(3);
	forth.push_back(2);
	forth.push_front(4);
	std::cout << "Size of forth: " << int (forth.size()) << '\n';
//	for(auto it = forth.begin(); it != forth.end(); ++it) {
	for(auto & x: forth) {
		std::cout << x << " ";
	}
	std::cout << std::endl;
}

// a binary predicate implemented as a function:
bool same_integral_part(double first, double second) {
	return (int(first) == int(second));
}

// a binary predicate implemented as a class:
class is_near_class {
public:
	bool operator()(double first, double second) {
		return (fabs(first - second) < 5.0);
	}
} is_near_object;

TEST(STL, forward_list) {
	// constructors used in the same order as described above:
	std::forward_list<double> mylist = {15.2, 73.0, 3.14, 15.85, 69.5,
		73.0, 3.99, 15.2, 69.2, 18.5};

	mylist.sort(); //   3.14,  3.99, 15.2, 15.2, 15.85
				   //  18.5,  69.2,  69.5, 73.0, 73.0

	mylist.unique();//   3.14,  3.99, 15.2, 15.85
				   //  18.5,  69.2,  69.5, 73.0
	mylist.unique (same_integral_part);//  3.14, 15.2, 18.5,  69.2, 73.0

	mylist.unique (is_near_object);//  3.14, 15.2, 69.2

	std::cout << "mylist contains:";
	for (double& x: mylist) std::cout << ' ' << x;
	std::cout << '\n';
}

TEST(STL, list) {
	std::list<int> mylist1, mylist2;
	std::list<int>::iterator it;

	// set some initial values:
	for (int i=1; i<=4; ++i)
		mylist1.push_back(i);// mylist1: 1 2 3 4

	for (int i=1; i<=3; ++i)
		mylist2.push_back(i*10);// mylist2: 10 20 30

	it = mylist1.begin();
	++it;// points to 2

	mylist1.splice (it, mylist2);// mylist1: 1 10 20 30 2 3 4
								 // mylist2 (empty)
								 // "it" still points to 2 (the 5th element)

	mylist2.splice (mylist2.begin(),mylist1, it);
								 // mylist1: 1 10 20 30 3 4
								 // mylist2: 2
								 // "it" is now invalid.
	it = mylist1.begin();
	std::advance(it,3);// "it" points now to 30

	mylist1.splice ( mylist1.begin(), mylist1, it, mylist1.end());
	// mylist1: 30 3 4 1 10 20

	std::cout << "mylist1 contains:";
	for (it=mylist1.begin(); it!=mylist1.end(); ++it)
	std::cout << ' ' << *it;
	std::cout << '\n';

	std::cout << "mylist2 contains:";
	for (it=mylist2.begin(); it!=mylist2.end(); ++it)
	std::cout << ' ' << *it;
	std::cout << '\n';
}

TEST(STL, set) {
	std::set<int> myset;
	myset.insert(10);
	myset.insert(20);
	myset.insert(30);
	myset.insert(25);
	std::pair<std::set<int>::iterator,bool> ret;
	ret = myset.insert(30);
	if(ret.second != true) {
		std::cout << "30 is already existed\n";
	}
	for(std::set<int>::iterator it = myset.begin(); it != myset.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

}

TEST(STL, unordered_set) {

}

TEST(STL, map) {
	std::map<char,int> mymap;

	// first insert function version (single parameter):
	mymap.insert ( std::pair<char,int>('a',100) );
	mymap.insert ( std::pair<char,int>('z',200) );

	std::pair<std::map<char,int>::iterator,bool> ret;
	ret = mymap.insert ( std::pair<char,int>('z',500) );
	if (ret.second==false) {
		std::cout << "element 'z' already existed";
		std::cout << " with a value of " << ret.first->second << '\n';
	}

	// second insert function version (with hint position):
	std::map<char,int>::iterator it = mymap.begin();
	mymap.insert (it, std::pair<char,int>('b',300));// max efficiency inserting
	mymap.insert (it, std::pair<char,int>('c',400));// no max efficiency inserting

	// third insert function version (range insertion):
	std::map<char,int> anothermap;
	anothermap.insert(mymap.begin(),mymap.find('c'));

	// showing contents:
	std::cout << "mymap contains:\n";
	for (it=mymap.begin(); it!=mymap.end(); ++it)
	std::cout << it->first << " => " << it->second << '\n';

	std::cout << "anothermap contains:\n";
	for (it=anothermap.begin(); it!=anothermap.end(); ++it)
	std::cout << it->first << " => " << it->second << '\n';
}

TEST(STL, unordered_map) {

	std::unordered_map<char,int> mymap;

	// first insert function version (single parameter):
	mymap.insert ( std::pair<char,int>('a',100) );
	mymap.insert ( std::pair<char,int>('z',200) );

//	std::pair<std::unordered_map<char,int>::iterator,bool> ret;
	auto ret = mymap.insert ( std::pair<char,int>('z',500) );
	if (ret.second == false) {
		std::cout << "element 'z' already existed";
		std::cout << " with a value of " << ret.first->second << '\n';
	}
	// second insert function version (with hint position):
	std::unordered_map<char,int>::iterator it = mymap.begin();
	mymap.insert (it, std::make_pair('b',300));
	mymap.insert (mymap.end(), std::pair<char,int>('c',400));

	// third insert function version (range insertion):
	std::unordered_map<char,int> anothermap;
	anothermap.insert(mymap.begin(),mymap.find('a'));

	// showing contents:
	std::cout << "mymap contains:\n";
	for (it=mymap.begin(); it!=mymap.end(); ++it)
	std::cout << it->first << " => " << it->second << '\n';

	std::cout << "anothermap contains:\n";
	for (it=anothermap.begin(); it!=anothermap.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';

	std::cout << "bucket count:" <<mymap.bucket_count() << "\n";
	std::cout << "max bucket count:" << mymap.max_bucket_count() << "\n";
	std::cout << "load factor:" << mymap.load_factor() << "\n";
	std::cout << "max load factor:" << mymap.max_load_factor() << "\n";
	std::cout << "size:" << mymap.size() << "\n";

	for(auto it = mymap.begin(); it != mymap.end(); ++it) {
		std::cout<<"["<<it->first <<"," << it->second << "] \n";
	}

	int n = mymap.bucket_count();
	for(int i = 0; i < n; i ++) {
		std::cout<<"bucket[" << i <<"]:";
		for(auto it = mymap.begin(i); it != mymap.end(i); ++it) {
			std::cout<<"["<<it->first <<"," << it->second << "] ";
		}
		std::cout << std::endl;
	}
}

TEST(STL, CreatVectorFromMap) {
	std::map<char,int> mymap;

	// insert some values:
	mymap['a']=10;
	mymap['b']=20;
	mymap['c']=30;
	mymap['e']=50;
	mymap['f']=60;
	mymap['d']=40;
	std::cout << "mymap contains:";
	for(auto it = mymap.begin(); it != mymap.end(); ++it) {
		std::cout << "[" << it->first <<", " << it->second << "] ";
	}
	std::cout << std::endl;

	std::vector<std::pair<char, int> > v(mymap.begin(), mymap.end());
	std::cout << "vector contains:";
	for(auto it = v.begin(); it != v.end(); ++it) {
		std::cout << "[" << (*it).first <<", " << it->second << "] ";
	}
	std::cout << std::endl;
}

TEST(STL, merge) {
	int data1[] = {1, 2, 5, 6, 8, 9, 10};
	int data2[] = {0, 2, 3, 4, 7, 8, 10};

	std::vector<int> v1(data1, data1+sizeof(data1)/sizeof(data1[0]));
	std::vector<int> v2(data2, data2+sizeof(data2)/sizeof(data2[0]));

	std::vector<int> tmp(std::max(v1.size(), v2.size()));

	std::vector<int> res = std::vector<int> (tmp.begin(), std::set_intersection(
			v1.begin(), v1.end(), v2.begin(), v2.end(), tmp.begin()));
	for(int &x : res) {
		std::cout << "[" << x<< "] ";
	}
	std::cout << std::endl;

	for(int &x : tmp) {
		std::cout << "[" << x<< "] ";
	}
	std::cout << std::endl;
}

TEST(STL, move) {
	std::vector<std::string> v;
	std::string str1 = "Knock";
	std::string str2 = "Knock";

	std::cout << "a:" << &str1 <<", b:" << &str2 <<std::endl;
	v.push_back(str1);
	v.push_back(std::move(str2));

	for(int i = 0; i < v.size(); i ++) {
		std::cout << "v[" <<i << "]:" << v[i] << ", " << &v[i] <<std::endl;
	}
	std::cout << "a:" << str1 <<", b:" << str2 <<std::endl;
}

TEST(STL, stream) {
	using namespace std;

    // ofstream is used for writing files
    // We'll make a file called Sample.dat
    ofstream outf("Sample.dat");

    // If we couldn't open the output file stream for writing
    if (!outf)
    {
        // Print an error and exit
        cerr << "Uh oh, Sample.dat could not be opened for writing!" << endl;
        exit(1);
    }

    // We'll write two lines into this file
    outf << "This is line 1" << endl;
    outf << "This is line 2" << endl;

	ifstream inf("Sample.dat");

	// If we couldn't open the input file stream for reading
	if (!inf)
	{
		// Print an error and exit
		cerr << "Uh oh, Sample.dat could not be opened for reading!" << endl;
		exit(1);
	}

	// While there's still stuff left to read
	while (inf)
	{
		// read stuff from the file into a string and print it
		std::string strInput;
		getline(inf, strInput);
		cout << strInput << endl;
	}
}

