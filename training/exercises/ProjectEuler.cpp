//ProjectEuler
//https://projecteuler.net/archives

#include <vector>
#include <iostream>

//If we list all the natural numbers below 10 that are multiples of 3 or 5,
//we get 3, 5, 6 and 9. The sum of these multiples is 23.
//Find the sum of all the multiples of 3 or 5 below 1000.
int sum(int natural) {
	natural -= 1;

	int threes = natural/3;
	int fives = natural/5;
	std::cout << threes <<"," <<  fives << std::endl ;
	int sum = (3 + threes * 3) * threes/2 + (5 + fives * 5) * fives/2;
	//remove 3x*5y <= 1000
	std::cout << sum << std::endl;
	int fifteens = natural/15;
	sum -= (15 + fifteens * 15) * fifteens/2;
	return sum;
}

int ProjectEuler(int argc, char *argv[]) {
	std::cout<< sum(1000) << std::endl;
	return 0;
}
