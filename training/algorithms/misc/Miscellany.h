/*
 * Miscellany.h
 *
 *  Created on: Jun 27, 2017
 *      Author: sheldon
 */

#ifndef MISCELLANY_H_
#define MISCELLANY_H_

template<typename T>
class Miscellany {
public:
	void permutation(std::vector<T> &v);
private:
	void permutation_recursively(std::vector<T> &v, int start);
	void permutation_cplusplus(std::vector<T> &v);

	template<class BidirIt, class Compare>
	bool permutation(BidirIt first, BidirIt last, Compare comp);
};

#endif /* MISCELLANY_H_ */
