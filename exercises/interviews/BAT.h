/*
 * BAT.h
 *
 *  Created on: Sep 3, 2017
 *      Author: sheldon
 */

#ifndef EXERCISES_INTERVIEWS_BAT_H_
#define EXERCISES_INTERVIEWS_BAT_H_

class BAT {
public:
	BAT();
	~BAT();
	void shuffle(vector<int> &cards);
private:
	BAT(const BAT&);
	BAT& operator=(const BAT&);
};


#endif /* EXERCISES_INTERVIEWS_BAT_H_ */
