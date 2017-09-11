/*
 * Miscellany.h
 *
 *  Created on: Aug 23, 2017
 *      Author: sheldon
 */

#ifndef EXERCISES_MISCELLANY_H_
#define EXERCISES_MISCELLANY_H_


class Miscellany {
public:
	Miscellany();
	~Miscellany();
	vector<int> findLongestOrderedSequence(vector<int> &);
private:
	Miscellany(const Miscellany &);
	Miscellany& operator=(const Miscellany&);
};



#endif /* EXERCISES_MISCELLANY_H_ */
