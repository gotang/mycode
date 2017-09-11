/*
 * BAT.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: sheldon
 */

#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>

#include <vector>
#include <iostream>
using namespace std;
#include "BAT.h"


BAT::BAT() {

}

BAT::~BAT() {

}

void BAT::shuffle(vector<int> &cards) {
	srandom(time(0) + getpid());

	for(int i = cards.size() - 1; i >= 0; i --) {
		int randomIndex = random()%(i + 1);
		swap(cards[randomIndex], cards[i]);
	}
}
