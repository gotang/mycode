/*
 * NowCoder.h
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */

#ifndef NOWCODER_H_
#define NOWCODER_H_

class NowCoder {
public:
	NowCoder() {}
	int buyHat(int nthCheapest);
	bool find(int target, vector<vector<int> > array);
    int countBitDiff(int m, int n);
    int calculateMax(vector<int> prices);
private:
	const NowCoder& operator=(const NowCoder&);
	NowCoder(const NowCoder&);
};


#endif /* NOWCODER_H_ */
