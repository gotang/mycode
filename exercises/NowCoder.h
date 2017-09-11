/*
 * NowCoder.h
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */

#ifndef NOWCODER_H_
#define NOWCODER_H_

struct ListNode {
      int val;
      struct ListNode *next;
      ListNode(int x) :
            val(x), next(NULL) {
      }
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class NowCoder {
public:
	NowCoder() {}
	/////////////////////////////////////////////////////
	int buyHat(int nthCheapest);
    int countBitDiff(int m, int n);
    int calculateMax(vector<int> prices);

	/////////////////////////////////////////////////////
    //Going after Offer
    //3:find target in 2-dimension array
	bool find(int target, vector<vector<int> > &array);

	//4:replace space in string str to "%20"
	//str memory size is length
	void replaceSpace(char *str,int length);

	//5:print list from tail to head
	vector<int> printListFromTailToHead(ListNode* head);

	//6: rebuild binary tree according given pre-order and in-order
	TreeNode* reconstructBinaryTree(vector<int> &pre,vector<int> &vin);

	//7:complete a queue with 2 stacks
    void push(int node);
    int pop();

    //8:find minimun number in a rotated array
    int minNumberInRotateArray(vector<int> rotateArray);

    //9:Fibonacci
    int Fibonacci(int n);
    //a froster can jump one step or two steps each time, given number steps
    //how many solutions are there for froster to jump to the top.
    int jumpFloor(int number);

    //
    void reOrderArray(vector<int> &array);

private:
    stack<int> stack1;
    stack<int> stack2;
private:
	const NowCoder& operator=(const NowCoder&);
	NowCoder(const NowCoder&);
};


#endif /* NOWCODER_H_ */
