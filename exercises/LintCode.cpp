/*
 * LintCode.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: sheldon
 */




#include "LintCode.h"

LintCode::LintCode() {

}

LintCode::~LintCode(){

}

int LintCode::digitCounts(int k, int n) {
    // write your code here
	   int sum = 0;
	   for(int i = 0; i <= n; i ++) {
		   int value = i;

		    do{
			   int remain = value % 10;
			   if(remain == k) {
				   sum ++;
			   }
			   value /= 10;
		   }while(value != 0);
	   }
	   return sum;
}

// param n : description of n
// return: description of return
long LintCode::trailingZeros(long n) {
	if (n < 5)
		return 0;

	long sum = 0;
	sum += n / 5;
	sum += trailingZeros(sum);
	return sum;
}

int LintCode::aplusb(int a, int b) {
	// write your code here, try to do it without arithmetic operators.
	int carry = 0;
	while (b) {
		carry = a & b;
		a ^= b;
		b = carry << 1;
	}
	return a;
}
