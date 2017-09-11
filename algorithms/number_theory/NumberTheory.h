/*
 * NumberTheory.h
 *
 *  Created on: Aug 27, 2017
 *      Author: sheldon
 */

#ifndef ALGORITHMS_NUMBER_THEORY_NUMBERTHEORY_H_
#define ALGORITHMS_NUMBER_THEORY_NUMBERTHEORY_H_
#include <vector>
#include <map>
using namespace std;

class NumberTheory {
public:
	NumberTheory();
	~NumberTheory();
	int leastCommonMultiple(int a, int b);
	int greatestCommonDivisor(int a, int b);
	map<int, int>integerFactorization(int a);

private:
	NumberTheory(const NumberTheory&);
	NumberTheory& operator=(const NumberTheory&);
};



#endif /* ALGORITHMS_NUMBER_THEORY_NUMBERTHEORY_H_ */
