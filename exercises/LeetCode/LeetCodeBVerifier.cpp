/*
 * LeetCodeBVerifier.cpp
 *
 *  Created on: Jul 27, 2017
 *      Author: sheldon
 */

#include <sys/time.h>
#include <climits>
#include <vector>
#include <iostream>
using namespace std;

#include "gtest/gtest.h"
#include "LeetCodeB.h"
#include "Common.h"


TEST(LeetCodeB, maxSubArray) {
	LeetCodeB A;
	vector<int> l = {-2,1,-3,4,-1,2,1,-5,4};
	EXPECT_EQ(A.maxSubArray(l), 6);

	l = {-1};
	EXPECT_EQ(A.maxSubArray(l), -1);

	l = { -1, -2};
	EXPECT_EQ(A.maxSubArray(l), -1);
}

TEST(LeetCodeB, lengthOfLastWord) {
	LeetCodeB A;
	EXPECT_EQ(A.lengthOfLastWord("hello world "), 5);
	EXPECT_EQ(A.lengthOfLastWord("hello world"), 5);
	EXPECT_EQ(A.lengthOfLastWord("world "), 5);
	EXPECT_EQ(A.lengthOfLastWord(" "), 0);
}

TEST(LeetCodeB, rotateRight) {
	LeetCodeB B;
	vector<int> v = {1, 2, 3, 4, 5, 6};
	List l(v);
	ListNode *res = B.rotateRight(l.head(), 0);
	//memory of res is from List l;
	//use setHead that doesn't free old memory
	l.setHead(res);
	List ll(v);
	EXPECT_TRUE(l.equal(ll));
	cout << "1:" << endl;

	v = {1};
	l = v;
	l.setHead(B.rotateRight(l.head(), 1));
	ll = v;
	EXPECT_TRUE(l.equal(ll));
	l.setHead(B.rotateRight(l.head(), 0));
	EXPECT_TRUE(l.equal(ll));

	cout << "2:" << endl;

	v = {1, 2};
	l = v;
	ll = v;
	l.setHead(B.rotateRight(l.head(), 2));
	l.dump();
	EXPECT_TRUE(l.equal(ll));

	cout << "3:" << endl;
	vector<int> va = {2, 1};
	l = v;
	ll = va;
	l.setHead(B.rotateRight(l.head(), 1));
	l.dump();
	EXPECT_TRUE(l.equal(ll));
}

TEST(LeetCodeB, isSameTree) {
	LeetCodeB A;
	vector<int> l = {1, 2, 3, 4, 5, 6};
	Tree a;
	Tree b;
	EXPECT_TRUE(A.isSameTree(a.root(), b.root()));

	a.buildBinarySearchTree(l);
	EXPECT_FALSE(A.isSameTree(a.root(), b.root()));
	b.buildBinarySearchTree(l);
	EXPECT_TRUE(A.isSameTree(a.root(), b.root()));

	a.reset();
	b.reset();
	l.clear();
	l = {0};
	a.buildBinarySearchTree(l);
	EXPECT_FALSE(A.isSameTree(a.root(), b.root()));
}

TEST(LeetCodeB, isSymmetric) {
	LeetCodeB B;
	vector<int> l = {1,2,2,3,4,4,3};
	Tree a;
	a.buildLevelOrder(l);
	a.dump();
	EXPECT_TRUE(B.isSymmetric(a.root()));

	l = {9,-42,-42,-1,76,76,-1,-1,13,-1,13};
	a.reset();
	a.buildLevelOrder(l);
	a.dump();
	EXPECT_FALSE(B.isSymmetric(a.root()));
}

TEST(LeetCodeB, levelOrder) {
	LeetCodeB B;
	vector<int> l = {1,2,2,3,4,4,3};
	Tree a;
	a.buildLevelOrder(l);
	vector<vector<int> > res = B.levelOrder(a.root());
	vector<vector<int> > ans = {{1}, {2, 2}, {3,4,4,3}};
	EXPECT_TRUE(equal(res.begin(), res.end(), ans.begin()));

	l = {9,-42,-42,-1,76,76,-1,-1,13,-1,13};
	a.reset();
	a.buildLevelOrder(l);

	res = B.levelOrder(a.root());
	ans = {{9}, {-42,-42}, {76,76}, {13, 13}};

	EXPECT_TRUE(equal(res.begin(), res.end(), ans.begin()));
}

TEST(LeetCodeB, levelOrderBottom) {
	LeetCodeB B;
	vector<int> l = {1,2,2,3,4,4,3};
	Tree a;
	a.buildLevelOrder(l);
	vector<vector<int> > res = B.levelOrderBottom(a.root());
	vector<vector<int> > ans = {{3,4,4,3}, {2, 2}, {1}};
	EXPECT_TRUE(equal(res.begin(), res.end(), ans.begin()));

	l = {9,-42,-42,-1,76,76,-1,-1,13,-1,13};
	a.reset();
	a.buildLevelOrder(l);

	res = B.levelOrderBottom(a.root());
	ans = { {13, 13}, {76,76}, {-42,-42}, {9}};

	EXPECT_TRUE(equal(res.begin(), res.end(), ans.begin()));
}
