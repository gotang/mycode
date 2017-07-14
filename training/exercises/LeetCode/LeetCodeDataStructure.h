/*
 * LeetCodeDataStructure.cpp
 *
 *  Created on: Jul 25, 2017
 *      Author: sheldon
 */

#ifndef TRAINING_EXERCISES_LEETCODEDATASTRUCTURE_CPP_
#define TRAINING_EXERCISES_LEETCODEDATASTRUCTURE_CPP_

struct ListNode {
	int val;
	ListNode *next;
	ListNode(int x):
			val(x),
			next(NULL) {
	}
	void dump();
};

class List {
public:
	List();
	List(vector<int> &l);
	List(ListNode *nodes);
	~List();
	void reset();
	ListNode* head();
	void setHead(ListNode *head) ;
	void dump();
	bool equal(List &l);
	bool equal(List *l);
	List& operator=(ListNode *nodes);
	List& operator=(vector<int> &l);

private:
	ListNode mHead;

	List(const List &t);
	List& operator=(const List &t);
};

//Definition for a binary tree node.
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {
    }
};

//build a binary search tree for test
class Tree {
public:
	Tree();
	Tree(vector<int> &l);
	~Tree();

	void reset();
    void buildBinarySearchTree(vector<int> &l);
    void buildLevelOrder(vector<int> &l);
    //inorder traverse
    void dump();
    TreeNode* root();
    static void inorderTraverse(TreeNode *node, vector<int> &vals);
private:
	Tree(const Tree &t);
	Tree& operator=(const Tree &t);

    void dump(TreeNode *node);
    TreeNode* remove(TreeNode *node);

    TreeNode* buildBinarySearchTree(TreeNode* node, int val);

    TreeNode *mRoot;
};



#endif /* TRAINING_EXERCISES_LEETCODEDATASTRUCTURE_CPP_ */
