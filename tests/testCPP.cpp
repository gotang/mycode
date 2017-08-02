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


class A
{
    public:
    A()
    {
        printf("1");
    }
    A(A &a)
    {
        printf("2");
    }
    A &operator=(const A &a)
    {
        printf("3");
        return *this;
    }
    void get(){}
};

template<class InputIterator1, class InputIterator2>
bool myEqual(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
	while (first1 != last1) {
		if (!(*first1 == *first2)) {// or: if (!pred(*first1,*first2)), for version 2
			cout << *first1  << " vs " << *first2 << endl;
			return false;
		}
		++first1;
		++first2;
	}
	return true;
}
class Widget {
public:
	Widget() {
		cout << "Widget " << endl;
	}
	Widget(const Widget &u) {
		cout << "Widget copy" << endl;
	}
};
Widget f(Widget u)
{
	cout << "1 " << endl;
	Widget v(u);
	cout << "2" << endl;
	Widget w = v;
	cout << "3, w:" << &w << endl;
	return w;
}
extern void testShareLibrary();
int main()
{
    Widget x;
    Widget y=f(f(x));
    cout << "y:" << &y << endl;
    testShareLibrary();
	return 0;
}
