
#include <stdio.h>
#include <cmath>
#include <cassert>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <iomanip>

using namespace std;

template<class BidirIt, class Compare>
bool permutation(BidirIt first, BidirIt last, Compare comp)
{
	//empty
    if (first == last) {
    	std::cout << "empty" << std::endl;
    	return false;
    }

    BidirIt i = last;
    //only one element
    if (first == --i) {
    	std::cout << "only one element" << std::endl;
    	return false;
    }

    while (true) {
        BidirIt i1, i2;

        i1 = i;
        if(comp(*--i, *i1)) {
        	//largest k so a[k] < a[k + 1]
        	i2 = last;
        	while(!comp(*i, *--i2));
			//largest l, so a[k] < a[l]
			std::iter_swap(i2, i);
			std::reverse(i1, last);
			return true;
        }

        if(i == first) {
        	//has gone over all permutations
        	//std::reverse(first, last);
        	return false;
        }
    }
}


#include <iostream>

class Base
{
public:
    Base()
    {
        std::cout << 1 << std::endl;
    }
    ~Base()
    {
        std::cout << 2 << std::endl;
    }
};

class A
{
public:
    A()
    {
        std::cout << 3 << std::endl;
    }
    ~A()
    {
        std::cout << 4 << std::endl;
    }
};

class B
{
public:
    B()
    {
        std::cout << 5 << std::endl;
    }
    ~B()
    {
        std::cout << 6 << std::endl;
    }
};

class Child : public Base
{
public:
    Child()
    {
        std::cout << 7 << std::endl;
    }
    ~Child()
    {
        std::cout << 8 << std::endl;
    }
private:
    B b;
    A a;
};

int main()
{
    Child child;
    return 0;
}
