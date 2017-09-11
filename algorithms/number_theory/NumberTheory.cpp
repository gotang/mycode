/*
 * NumberTheory.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: sheldon
 */




#include <iostream>
using namespace std;

#include "NumberTheory.h"
NumberTheory::NumberTheory() {

}

NumberTheory::~NumberTheory() {

}

int NumberTheory::leastCommonMultiple(int a, int b) {
	return a * b /greatestCommonDivisor(a, b);
}

int NumberTheory::greatestCommonDivisor(int a, int b) {
	if(b) {
		while((a %= b) && (b %= a)) {
			cout << "a:" << a << ", b:" << b << endl;
		};
	}
	return a + b;
}

map<int, int> NumberTheory::integerFactorization(int n) {
	map<int, int> factors;
	if(n <= 0)
		return factors;

	int divisorNums = 1;
	for(int k = 2; k <= n; k ++) {
		int num = 0;
		while(n % k == 0) {
			n /= k;
			num ++;
		}
		divisorNums *= (num + 1);
		factors[k] = num;
	}
	cout << "divisorNums:" << divisorNums <<endl;
	return factors;
}
