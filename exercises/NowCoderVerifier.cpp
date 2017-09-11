/*
 * NowCoderVerifier.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <stack>

#include "gtest/gtest.h"
using namespace std;
#include "Common.h"
#include "NowCoder.h"

TEST(NowCoder, BuyHats){
}

TEST(NowCoder, countBitDiff){
	NowCoder n;
	EXPECT_EQ(n.countBitDiff(1999, 2299), 7);
}

TEST(NowCoder, find){
	NowCoder n;
	vector<vector<int> > v;
	v.push_back( {1, 2, 8,  9});
	v.push_back( {2, 4, 9,  12});
	v.push_back( {4, 7, 8,  13});
	v.push_back( {6, 8, 11, 15});
	EXPECT_TRUE(n.find(1,v));
	EXPECT_TRUE(n.find(11,v));
	EXPECT_FALSE(n.find(200,v));
}

TEST(NowCoder, replaceSpace){
	NowCoder n;
	char str[200] = {"We are happy"};
	n.replaceSpace(str, 200 -1);
	EXPECT_TRUE(strcmp(str, "We%20are%20happy") == 0);
	cout << str << endl;

	strcpy(str, " helloworld");
	n.replaceSpace(str, 200 -1);
	EXPECT_TRUE(strcmp(str, "%20helloworld") == 0);
	cout << str << endl;
}


TEST(NowCoder, printListFromTailToHead) {
	ListNode dummyHead(0);
	ListNode *head = &dummyHead;
	for(int i = 1; i < 6; i ++) {
		head->next = new ListNode(i);
		head = head->next;
	}
	NowCoder n;
	vector<int> res = n.printListFromTailToHead(dummyHead.next);
	vector<int> ans = {5, 4,3,2,1};
	EXPECT_TRUE(std::equal(res.begin(), res.end(), ans.begin()));
}

TEST(NowCoder, queueCompletedWithTwoStacks) {
	NowCoder n;
	for(int i = 0; i < 5; i ++) {
		n.push(i);
	}
	for(int i = 0; i < 5; i ++) {
		EXPECT_EQ(n.pop(), i);
	}
}

TEST(NowCoder, reOrderArray) {
	NowCoder n;
	vector<int> array = {1, 2, 3, 4, 5, 6, 7};
	n.reOrderArray(array);
	dumpVector("array", array);
}
