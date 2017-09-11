/*
 * NowCoder.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */
#include <vector>
#include <set>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stack>
using namespace std;
#include "Common.h"
#include "NowCoder.h"

int NowCoder::buyHat(int nthCheapest) {
	int n = 0;
	std::cout << "How many hats:";
	std::cin >> n;
	std::cout << "There's " << n << " hats" << std::endl;

	int p;
	std::set<int> prices;
	std::cout << "Prices:";
	for(int i = 0; i < n; i ++) {
		std::cin >> p;
		auto ret = prices.insert(p);
		if(ret.second == false) {
			std::cout << "prices " << p << " has already existed\n";
		}
	}
	if(prices.size() >= nthCheapest) {
		auto it = prices.begin();
		std::advance(it, 2);
		return *(it);
	} else {
		return -1;
	}
}

int NowCoder::countBitDiff(int m, int n) {
	m = m^n;
	int count = 0;
	while(m) {
		count ++;
		m = m & (m-1);
	}
	return count;
}

int NowCoder::calculateMax(vector<int> prices) {
	return 0;
}

bool NowCoder::find(int target, vector<vector<int> > &array) {
	bool found = false;
	if(array.empty())
		return found;

	int rows = array.size();
	int cols = array[0].size();
	//scan from top-right to bottom-left
	int r = 0;
	int c = cols - 1;
	while(r < rows && c >= 0) {
		if (array[r][c] == target) {
			found = true;
			break;
		} else if (target > array[r][c]) {
			//ignore top row
			r++;
		} else {
			//array[r][c] < target, ignore right column
			c--;
		}
	}

	return found;
}


void NowCoder::replaceSpace(char *str, int length) {
	if(str == NULL)
		return ;
	int spaceNum = 0;
	int len = strlen(str);
	for(int i = 0; i < len; i ++) {
		if(str[i] == ' ') {
			spaceNum ++;
		}
	}
	if(spaceNum == 0)
		return ;
	//check if there's sufficient memeory
	if(len + spaceNum * 2 + 1 > length)
		return ;

	char *tail = str + len - 1;
	char *newTail = tail + spaceNum * 2 + 1;
	*newTail-- = '\0';
	while(tail >= str) {
		if(*tail == ' ') {
			*newTail-- = '0';
			*newTail-- = '2';
			*newTail-- = '%';
		} else {
			*newTail-- = *tail;
		}
		tail--;
	}
}


vector<int> NowCoder::printListFromTailToHead(ListNode* head) {
	vector<int> res;
	ListNode *h = head;
	while(h != NULL) {
		res.push_back(h->val);
		h = h->next;
	}
	std::reverse(res.begin(), res.end());
	return res;
}


TreeNode* NowCoder::reconstructBinaryTree(vector<int> &pre,vector<int> &vin) {
	TreeNode dummyRoot(0);
	return dummyRoot.left;
}

void NowCoder::push(int node) {
	stack1.push(node);
}

int NowCoder::pop() {
	if(stack2.empty()) {
		while(!stack1.empty()) {
			stack2.push(stack1.top());
			stack1.pop();
		}
	}
	int value = -1;
	if(!stack2.empty()) {
		value = stack2.top();
		stack2.pop();
	}
	return value;
}

int NowCoder::Fibonacci(int n) {
	int fibnocci0 = 0;
	int fibnocci1 = 1;
	if(n == 0)
		return fibnocci0;
	if(n == 1)
		return fibnocci1;

	int fibnocci = 0;
	for(int i = 2; i <= n; i++) {
		fibnocci = fibnocci1 + fibnocci0;
		fibnocci0 = fibnocci1;
		fibnocci1 = fibnocci;
	}
	return fibnocci;
}

int NowCoder::jumpFloor(int number) {
    int fibnocci1 = 1;
    int fibnocci2 = 2;

    if(number == 1)
        return fibnocci1;
    if(number == 2)
        return fibnocci2;

    int fibnocci = 0;
    for(int i = 3; i <= number; i++) {
        fibnocci = fibnocci2 + fibnocci1;
        fibnocci1 = fibnocci2;
        fibnocci2 = fibnocci;
    }
    return fibnocci;
}


void NowCoder::reOrderArray(vector<int> &array) {
	if(array.empty())
		return ;
	int size = array.size();
	int head = 0;
	int tail = size - 1;
	while(head < tail) {
		while((array[head] & 0x1) && (head < tail)) head ++;
		while(!(array[tail] & 0x1) && (head < tail)) tail --;
		std::swap(array[head], array[tail]);
	}
}


int NowCoder::minNumberInRotateArray(vector<int> rotateArray) {
	if(rotateArray.empty())
		return -1;
	int min = 0;
	int start = 0;
	int end = rotateArray.size() - 1;
	while(start < end) {
		int mid = start + (end - start)/2;

		if(rotateArray[mid] > rotateArray[start]) {
			//must be in second half
			start = mid + 1;
		} else if(rotateArray[mid] < rotateArray[start]) {
			end = mid - 1;
		} else {

		}
	}
	return min;
}


