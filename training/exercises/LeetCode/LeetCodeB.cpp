/*
 * LeetCodeB.cpp
 *
 *  Created on: Jul 27, 2017
 *      Author: sheldon
 */
#include <stdio.h>
#include <climits>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <stack>
#include <queue>
using namespace std;
#include "Common.h"
#include "LeetCodeB.h"


//53. Maximum Subarray
//Find the contiguous subarray within an array (containing at least one number) which has the largest sum.
int LeetCodeB::maxSubArray(vector<int>& nums) {
	int sum = 0;
	//TODO:finish
	return sum;
}

//58. Length of Last Word
//Given a string s consists of upper/lower-case alphabets and empty space characters ' ', return the length of last word in the string.
//If the last word does not exist, return 0.
int LeetCodeB::lengthOfLastWord(string s) {
	int len = 0;
	int pos = s.size() - 1;
	//ignore white space at the end of string
	while(pos >= 0 && s.at(pos) == ' ') pos --;
	//count non-white space character
	while(pos >= 0 && s.at(pos) != ' ') { pos --; len ++;}
	return len;
}


//61. Rotate List
//Given a list, rotate the list to the right by k places, where k is non-negative.
ListNode* LeetCodeB::rotateRight(ListNode* head, int k) {
    ListNode **t1 = &head;
    ListNode *t2 = head;
    if(k <= 0)
        return head;

    for(int i = 1; t2 && i < k; i ++) {
        t2 = t2->next;
    }
    while(t2 && t2->next != NULL) {
        t1 = &((*t1)->next);
        t2 = t2->next;
    }
    if(t2 != NULL && *t1 != head) {
        t2->next = head;
        head = *t1;
        *t1 = NULL;
    }
    return head;

}

//100. Same Tree
//Given two binary trees, write a function to check if they are equal or not.
//Two binary trees are considered equal if they are structurally identical and the nodes have the same value.
bool LeetCodeB::isSameTree(TreeNode* p, TreeNode* q) {
	if(!p || !q) {
		//one is empty while another one is not empty
		return p == q;
	}
	//all non-empty
	return p->val == q->val &&
			isSameTree(p->left, q->left) &&
			isSameTree(p->right, q->right);
}

void inorderTraverse(TreeNode* node, vector<int> &vals) {
	if(node == NULL)
		return ;
	inorderTraverse(node->left, vals);
	vals.push_back(node->val);
	inorderTraverse(node->right, vals);
}

//101. Symmetric Tree
//Given a binary tree, check whether it is a mirror of itself (ie, symmetric around its center).
bool LeetCodeB::isSymmetric(TreeNode* root) {
	if(!root)
		return true;

	bool symmetric = true;
	queue<TreeNode *> nodes;
	nodes.push(root->left);
	nodes.push(root->right);

	while(!nodes.empty()) {
		TreeNode *left = nodes.front();
		nodes.pop();
		TreeNode *right = nodes.front();
		nodes.pop();
		if(!left && !right) {
			continue;
		} else if(!left || !right) {
			symmetric = false;
			break;
		} else if (left->val != right->val) {
			symmetric = false;
			break;
		}
		nodes.push(left->left);
		nodes.push(right->right);
		nodes.push(left->right);
		nodes.push(right->left);
	}
	return symmetric;
}

//102. Binary Tree Level Order Traversal
//Given a binary tree, return the level order traversal of its nodes' values. (ie, from left to right, level by level).
vector<vector<int> > LeetCodeB::levelOrder(TreeNode* root) {
    vector<vector<int> > ans;
    if (root == nullptr)
        return ans;
	queue<TreeNode*> nodes;
	nodes.push(root);
	nodes.push(nullptr);
	vector<int> line;
	while(!nodes.empty()) {
		TreeNode *node = nodes.front();
		nodes.pop();
		if(node == nullptr) {
			ans.push_back(line);
			line.clear();
			if(nodes.size() > 0) {
				nodes.push(nullptr);
			}
			continue;
		} else {
			line.push_back(node->val);
		}
		if(node->left)
			nodes.push(node->left);
		if(node->right)
			nodes.push(node->right);
	}

	return ans;
}

//107. Binary Tree Level Order Traversal II
//Given a binary tree, return the bottom-up level order traversal of its nodes' values. (ie, from left to right, level by level from leaf to root).
vector<vector<int> > LeetCodeB::levelOrderBottom(TreeNode* root) {
    vector<vector<int> > ans;
    if (root == nullptr)
        return ans;
	queue<TreeNode*> nodes;
	nodes.push(root);
	nodes.push(nullptr);
	vector<int> line;
	while(!nodes.empty()) {
		TreeNode *node = nodes.front();
		nodes.pop();
		if(node == nullptr) {
			ans.push_back(line);
			line.clear();
			if(nodes.size() > 0) {
				nodes.push(nullptr);
			}
			continue;
		} else {
			line.push_back(node->val);
		}
		if(node->left)
			nodes.push(node->left);
		if(node->right)
			nodes.push(node->right);
	}
	std::reverse(ans.begin(), ans.end());
	return ans;
}





