/*
 * Miscellany.cpp
 *
 *  Created on: Jun 27, 2017
 *      Author: sheldon
 */



#include <vector>
#include <iostream>
#include <algorithm>

#include "base/Common.h"
#include "Miscellany.h"

template class Miscellany<int>;
//list all permutations recursively, this doesn't remove repeated
//permutation
template<class T>
void Miscellany<T>::permutation_recursively(std::vector<T> &v, int start) {
	static int count = 0;
	if(start >= v.size()) {
		dumpVector(std::to_string(count++), v);
		return ;
	}
	for(int i = start; i < v.size(); i ++) {
		//set v[i] as first column, permutate other elements
		std::swap(v[i], v[start]);
		//permutate v[start + 1, v.size())
		permutation_recursively(v, start + 1);
		//go back to original vector
		std::swap(v[i], v[start]);
	}
}

//one possible implement of next_permutation with Compare as less
//or prev_permutation with with Compare as greater
template<class T>
template<class BidirIt, class Compare>
bool Miscellany<T>::permutation(BidirIt first, BidirIt last, Compare comp)
{
	if(first == last) {
		//list is empty
		return false;
	}

	BidirIt k0 = last;
	if(--k0 == first) {
		//there's only one element in list
		return false;
	}

	while(true) {
		BidirIt k1 = k0;
		//find last k, so list.at(k) < list.at(k+1) if comp is std::less
		if(comp(*--k0, *k1)) {
			BidirIt l = last;
			//find last l that list.at(l) > list.at(k)
			while(!comp(*k0, *--l));
			std::iter_swap(k0, l);
			std::reverse(k1, last);
			return true;
		}

		if(k0 == first) {
			//list is sorted as comp required
			return false;
		}
	}
}

//list permutations using next_permutation and prev_permutation in c++ library
//this removes the repeated permutations
template<class T>
void Miscellany<T>::permutation_cplusplus(std::vector<T> &v) {
	//sort array with descending order, so we can get all permutations
	//with prev_permutatioin

	std::cout << "stl permutating descending" << std::endl;
	std::sort(v.begin(), v.end(), std::greater<int>());
	do {
		dumpVector("", v);
	}while(std::prev_permutation(v.begin(), v.end()));
	std::cout << std::endl;

	//sort array with abscending order, so we can get all permutations
	//with next_permutation

	std::cout << "permutating abscending" << std::endl;
	std::sort(v.begin(), v.end());
	do {
		dumpVector("", v);
	}while(std::next_permutation(v.begin(), v.end()));
	std::cout << std::endl;
}

//list permutationi of v from start to the end of v
template<class T>
void Miscellany<T>::permutation(std::vector<T> &v) {
	std::cout << "permutating with STL" << std::endl;
	permutation_cplusplus(v);

	std::cout << "permutating recursively" << std::endl;
	permutation_recursively(v, 0);
	std::cout << std::endl;

	std::cout << "permutating abscending" << std::endl;
	std::sort(v.begin(), v.end(), std::less<T>());
	do {
		dumpVector("", v);
	}while(permutation(v.begin(), v.end(), std::less<T>()));
	std::cout << std::endl;

	std::cout << "permutating descending" << std::endl;
	std::sort(v.begin(), v.end(), std::greater<T>());
	do {
		dumpVector("", v);
	}while(permutation(v.begin(), v.end(), std::greater<T>()));
	std::cout << std::endl;
}
