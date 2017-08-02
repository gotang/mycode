/*
 * LeetCodeDataStructure.cpp
 *
 *  Created on: Jul 25, 2017
 *      Author: sheldon
 */

#include <queue>
#include <vector>
#include <iostream>
using namespace std;

#include "LeetCodeDataStructure.h"
void ListNode::dump() {
	ListNode *n = this;
	while(n) {
		cout << n->val << " ";
		n = n->next;
	}
	cout << endl;
}

List::List()
	:mHead(-1) {
}

List::List(vector<int> &l)
	:mHead(-1) {
	ListNode *head = &mHead;
	for(auto &i :l) {
		head->next = new ListNode(i);
		head = head->next;
	}
}

List::List(ListNode *nodes)
	:mHead(-1) {
	ListNode *head = &mHead;
	while(nodes != NULL) {
		head->next = new ListNode(head->val);
		nodes = nodes->next;
	}
}

List::~List() {
	reset();
}

void List::reset() {
	ListNode *head = mHead.next;
	ListNode *tmp;
	while(head){
		tmp = head;
		head = tmp->next;
		delete tmp;
	}
	mHead.next = NULL;
}

ListNode* List::head() {
	return mHead.next;
}

void List::setHead(ListNode *head) {
	mHead.next = head;
}

void List::dump() {
	mHead.dump();
}

bool List::equal(List &l) {
	return equal(&l);
}

bool List::equal(List *l) {
	ListNode *n = head();
	ListNode *m = l->head();
	while (n && m) {
		if (n->val != m->val) {
			return false;
		}
		n = n->next;
		m = m->next;
	}
	if(m || n)
		return false;
	return true;
}

List& List::operator=(vector<int> &l) {
	ListNode *head = &mHead;
	for(auto &i :l) {
		head->next = new ListNode(i);
		head = head->next;
	}
}

List& List::operator=(ListNode *nodes) {
	reset();
	ListNode *head = &mHead;
	while(nodes != NULL) {
		head->next = new ListNode(head->val);
		nodes = nodes->next;
	}
	return *this;
}

////////////////////////////////////////////////
//static
void Tree::inorderTraverse(TreeNode* node, vector<int> &vals) {
	if(node == NULL)
		return ;
	inorderTraverse(node->left, vals);
	vals.push_back(node->val);
	inorderTraverse(node->right, vals);
}

Tree::Tree() {
	mRoot = NULL;
}

Tree::~Tree() {
	reset();
}

void Tree::reset() {
	mRoot = remove(mRoot);
	cout << "mRoot after reset " << mRoot << endl;
}

TreeNode* Tree::root() {
	return mRoot;
}

TreeNode* Tree::remove(TreeNode *node) {
	if(node == NULL)
		return NULL;
	node->left = remove(node->left);
	node->right = remove(node->right);
	delete node;
	return NULL;
}


void Tree::buildLevelOrder(vector<int> &l) {
	if(l.empty())
		return ;
	int i = 0;
	mRoot = new TreeNode(l.at(i));
	queue<TreeNode *> nodes;
	nodes.push(mRoot);
	i++;
	while(i < l.size() && !nodes.empty()) {
		TreeNode *node = nodes.front();
		nodes.pop();
		if(node == NULL) continue;
		for(int j = 0; j < 2 && i < l.size() ; j ++, i++) {
			int val = l.at(i);
			TreeNode * n = NULL;
			if(val != -1)
				n = new TreeNode(val);
			nodes.push(n);
			if(j) {
				node->right = n;
			} else {
				node->left = n;
			}
		}
	}
}

//build a binary search tree for test
void Tree::buildBinarySearchTree(vector<int> &vals) {
	for(int &i :vals)
		mRoot = buildBinarySearchTree(mRoot, i);
}

TreeNode* Tree::buildBinarySearchTree(TreeNode* node, int val) {
	if(node == NULL)
		return new TreeNode(val);
	if(val < node->val)
		node->left = buildBinarySearchTree(node->left, val);
	else if(val > node->val)
		node->right = buildBinarySearchTree(node->right, val);
	return node;
}

void Tree::dump() {
	dump(mRoot);
	cout << endl;
}

void Tree::dump(TreeNode *node) {
	if(node == NULL)
		return ;
	dump(node->left);
	cout << node->val << " ";
	dump(node->right);
}
