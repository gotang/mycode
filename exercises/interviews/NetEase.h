/*
 * NetEase.h
 *
 *  Created on: Sep 4, 2017
 *      Author: sheldon
 */

#ifndef EXERCISES_INTERVIEWS_NETEASE_H_
#define EXERCISES_INTERVIEWS_NETEASE_H_


class NetEase {
public:
	NetEase();
	~NetEase();
	vector<int> operationSequece(vector<int> &s);
	int independentYi(int x, int f, int d, int p);
private:
	NetEase(const NetEase&);
	NetEase & operator=(const NetEase&);
};


#endif /* EXERCISES_INTERVIEWS_NETEASE_H_ */
