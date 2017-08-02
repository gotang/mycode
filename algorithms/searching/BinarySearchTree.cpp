/*
 * Searching.cpp
 *
 *  Created on: Jun 27, 2017
 *      Author: sheldon
 */

#include <cassert>
#include <vector>
#include <iostream>
#include "BinarySearchTree.h"

////////////////////////////////////////////////////////////////////////////////////////
template class BinarySearchTree<char, int>;

template<class Key, class Value>
BinarySearchTree<Key,Value>::Node::Node(Node* parent, const Key &key, const Value &val, const int N) {
	this->key = key;
	this->val = val;
	this->N = N;
	this->parent = parent;
	left = right = NULL;
	std::cout << "create node " << this << std::endl;
}

template<class Key, class Value>
BinarySearchTree<Key,Value>::Node::~Node() {
	std::cout << "delete node " << this << std::endl;
}

template<class Key, class Value>
Key BinarySearchTree<Key,Value>::Node::getKey() {
	return key;
}

template<class Key, class Value>
Value BinarySearchTree<Key,Value>::Node::getValue() {
	return val;
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::Node::dump() {
	std::cout << "node is " << key;
	if(parent) {
		std::cout << ", parent is    " << parent->key;
	} else {
		std::cout << ", parent is NULL";
	}

	if(left) {
		std::cout << ", left child is    " << left->key;
	} else {
		std::cout << ", left child is NULL";
	}

	if(right) {
		std::cout << ", right child is    " << right->key;
	} else {
		std::cout << ", right child is NULL";
	}
	std::cout << std::endl;
}

template<class Key, class Value>
BinarySearchTree<Key,Value>::BinarySearchTree()
	:head(NULL){

}

template<class Key, class Value>
BinarySearchTree<Key,Value>::~BinarySearchTree() {
	while(!isEmpty()) {
		deleteMax();
	}
	assert(isEmpty() && "Search tree is not empty after destroy");
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isEmpty() {
	return size() == 0;
}

template<class Key, class Value>
int BinarySearchTree<Key,Value>::size() {
	return size(head);
}

template<class Key, class Value>
int BinarySearchTree<Key,Value>::size(const Node *node) {
	if(node == NULL)
		return 0;
//	return node->N;
	return 1 + size(node->left) + size(node->right);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::updateSize(Node *node) {
	if(node != NULL) {
		node->N = 1 + size(node->left) + size(node->right);
	}
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::contains(const Key &key) {
	return get(key) != NULL;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::get(const Key &key) {
	return get(head, key);
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node*
BinarySearchTree<Key,Value>::get(Node *node, const Key &key) {
	if(node == NULL)
		return NULL;

	if(node->key == key) {
		return node;
	} else if(key < node->key) {
		return get(node->left, key);
	} else {
		return get(node->right, key);
	}
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::put(const Key &key, const Value &val) {
	std::cout << "putting [" << key <<", " << val <<"]" << std::endl;
	head = put(head, NULL, key, val);
	assert(check() && "Check search tree failed after put");
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node*
BinarySearchTree<Key,Value>::put(Node *node, Node *parent, const Key &key, const Value &val) {
	if(node == NULL)
		return new Node(parent, key, val, 1);

	if(key > node->key) {
		//put to right subtree
		node->right = put(node->right, node, key, val);
	} else if(key < node->key) {
		//put to left subtree
		node->left = put(node->left, node, key, val);
	} else {
		//update value
		node->val = val;
	}
	updateSize(node);
	return node;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::min() {
	return min(head);
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::min(Node *node) {
	if(node == NULL)
		return NULL;

	if(node->left != NULL)
		return min(node->left);

	return node;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::max() {
	return max(head);
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::max(Node *node) {
	if(node == NULL)
		return NULL;

	if(node->right != NULL)
		return max(node->right);

	return node;
}

template<class Key, class Value>
typename BinarySearchTree<Key,Value>::Node*
BinarySearchTree<Key,Value>::floor(const Key &key) {
	return floor(head, key);
}

template<class Key, class Value>
typename BinarySearchTree<Key,Value>::Node*
BinarySearchTree<Key,Value>::floor(Node *node, const Key &key) {
	if(node == NULL)
		return NULL;

	if(node->key == key) {
		return node;
	} else if(key < node->key) {
		return floor(node->left, key);
	}

	Node *t = floor(node->right, key);
	if(t != NULL)
		return t;
	else
		return node;
}

template<class Key, class Value>
typename BinarySearchTree<Key,Value>::Node*
BinarySearchTree<Key,Value>::ceiling(const Key &key) {
	return ceiling(head, key);
}

template<class Key, class Value>
typename BinarySearchTree<Key,Value>::Node*
BinarySearchTree<Key,Value>::ceiling(Node *node, const Key &key) {
	if(node == NULL)
		return NULL;

	if(node->key == key) {
		return node;
	} else if (key > node->key) {
		return ceiling(node->right, key);
	}
	Node *t = ceiling(node->left, key);
	if(t != NULL)
		return t;
	else
		return node;
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::deleteMin() {
	head = deleteMin(head);
	assert(check() && "Check search tree failed after deleteMin");
}

template<class Key, class Value>
typename BinarySearchTree<Key,Value>::Node*
BinarySearchTree<Key,Value>::deleteMin(Node *node) {
	if(node == NULL)
		return NULL;

	if(node->left == NULL) {
		Node *right = node->right;
		if(right) {
			right->parent = node->parent;
		}
		delete node;
		return right;
	}

	node->left = deleteMin(node->left);
	updateSize(node);
	return node;
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::deleteMax() {
	head = deleteMax(head);
	assert(check() && "Check search tree failed after deleteMax");
}

template<class Key, class Value>
typename BinarySearchTree<Key,Value>::Node*
BinarySearchTree<Key,Value>::deleteMax(Node *node) {
	if(node == NULL)
		return NULL;

	if(node->right == NULL) {
		Node *left = node->left;
		if(left) {
			left->parent = node->parent;
		}
		delete node;
		return left;
	}
	node->right = deleteMax(node->right);
	updateSize(node);
	return node;
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::remove(const Key &key) {
	std::cout << "removing " << key << std::endl;
	head = remove(head, key);//update head
	assert(check());
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node*
BinarySearchTree<Key,Value>::remove(Node *node, const Key &key) {
	if(node == NULL)
		return NULL;

	if(key < node->key) {
		node->left = remove(node->left, key);
	} else if(key > node->key){
		node->right = remove(node->right, key);
	} else {
		//find node we want to remove
		if(node->left == NULL) {
			Node *right = node->right;
			if(right) {
				right->parent = node->parent;
			}
			delete node;
			return right;
		} else if(node->right == NULL) {
			Node *left = node->left;
			if(left) {
				left->parent = node->parent;
			}
			delete node;
			return left;
		} else {
			Node *t = node;
			node = min(t->right);

			if(t->right->left) {
				//if node is a left child of t->right, we need to set
				//his right child as left child of his parent
				node->parent->left = node->right;
				if(node->right)
					node->right->parent = node->parent;

				//look for parent
				node->parent = t->parent;
				node->right = t->right;
				node->left = t->left;

				Node *right = node->right;
				while(right) {
					updateSize(right);
					right = right->right;
				}
			} else {
				//right child has no left child, right child is the minimium one.
				//node is t->right
				//node->right = t->right->right;
				node->left = t->left;
				node->parent = t->parent;
			}
			node->left->parent = node;
			if(node->right) {
				node->right->parent = node;
			}
			delete t;
		}
	}
	updateSize(node);
	return node;
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::select(int k) {
	return select(head, k);
}

template<class Key, class Value>
typename BinarySearchTree<Key, Value>::Node* BinarySearchTree<Key,Value>::select(Node *node, int k) {
	if(node == NULL) {
		return NULL;
	}

	int t = size(node->left);
	if(t > k) {
		return select(node->left, k);
	} else if(t < k) {
		return select(node->right, k - t -1 );
	} else {
		return node;
	}
}

template<class Key, class Value>
int BinarySearchTree<Key,Value>::rank(const Key &k) {
	if(!contains(k)) {
		return -1;
	}
	return rank(head, k);
}

template<class Key, class Value>
int BinarySearchTree<Key,Value>::rank(Node *node, const Key &k) {
	if(node == NULL) {
		return -1;
	}
	if(node->key == k) {
		return size(node->left);
	} else if(k < node->key) {
		return rank(node->left, k);
	} else {
		return 1 + size(node->left) + rank(node->right, k);
	}
	return 0;
}

template<class Key, class Value>
int BinarySearchTree<Key,Value>::height() {
	return height(head);
}

template<class Key, class Value>
int BinarySearchTree<Key,Value>::height(Node *node) {
	if(node == NULL)
		return -1;
	return 1 + std::max(height(node->left), height(node->right));
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::keys(const Key &lo, const Key &hi, std::vector<Key>& q){
	keys(head, lo, hi, q);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::keys(Node *node, const Key &lo, const Key &hi, std::vector<Key>& q){
	if(node == NULL)
		return ;
	if(node->key > lo)
		keys(node->left, lo, hi, q);

	if(node->key >= lo && node->key <= hi)
		q.push_back(node->key);

	if(node->key < hi)
		keys(node->right, lo, hi, q);
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::check() {
	bool ifBinarySearchTree = isBinarySearchTree();
	bool ifRankConsistent = isRankConsistent();
	bool ifSizeConsistent = isSizeConsistent();
	bool ifLinkConsistent = isLinkConsistent();
	if(!ifBinarySearchTree) {
		std::cout << "Binary search tree check failed" << std::endl;
	}
	if(!ifRankConsistent) {
		std::cout << "Rank consistent check failed" << std::endl;
	}
	if(!ifSizeConsistent) {
		std::cout << "Size consistent check failed" << std::endl;
	}
	if(!ifLinkConsistent) {
		std::cout << "Link consistent check failed" << std::endl;
	}

	return ifBinarySearchTree && ifRankConsistent && ifSizeConsistent && ifLinkConsistent;
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isRankConsistent() {
	for(int i = 0; i < size(); i ++) {
		Node *node = select(i);
		if(node == NULL || i != rank(node->key)) {
			std::cout << "rank of " << i << " is " << ((node == NULL) ? -1 : rank(node->key)) << std::endl;
			return false;
		}
	}
	if(isEmpty())
		return true;

	std::vector<Key> keyQueue;

	keys(min()->key, max()->key, keyQueue);

	typename std::vector<Key>::iterator it;
	for(it = keyQueue.begin(); it != keyQueue.end(); ++it) {
		Node *node = select(rank(*it));
		if(node == NULL || node->key != *it) {
			std::cout << "key of " << *it << " is " << ((node == NULL) ? -1 : node->key) << std::endl;
			return false;
		}
	}

	return true;
}

// are the size fields correct?
template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isSizeConsistent() {
	return isSizeConsistent(head);
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isSizeConsistent(const Node *node) {
	if(node == NULL)
		return true;
	if(node->N != (size(node->left) + size(node->right) + 1)) {
		std::cout << "node is " << node->key << ", N:" << node->N <<", size of left:" << size(node->left)
				<< ", size of right:" << size(node->right) << std::endl;
		return false;
	}
	return isSizeConsistent(node->left) && isSizeConsistent(node->right);
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isBinarySearchTree() {
	return isBinarySearchTree(head);
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isBinarySearchTree(const Node *node) {
	if(node == NULL)
		return true;
	if(node->left && node->left->key >= node->key) {
		dumpNode(node);
		dumpNode(node->left);
		return false;
	}
	if(node->right && node->right->key <= node->key) {
		dumpNode(node);
		dumpNode(node->right);
		return false;
	}

	return isBinarySearchTree(node->left) && isBinarySearchTree(node->right);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::preoderTraverse(std::vector<Key>& q) {
	preoderTraverse(head, q);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::preoderTraverse(const Node *node, std::vector<Key>& q) {
	if(node == NULL)
		return ;
	q.push_back(node->key);
	preoderTraverse(node->left, q);
	preoderTraverse(node->right, q);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::inorderTraverse(std::vector<Key>& q) {
	inorderTraverse(head, q);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::inorderTraverse(const Node *node, std::vector<Key>& q) {
	if(node == NULL)
		return ;
	inorderTraverse(node->left, q);
	q.push_back(node->key);
	inorderTraverse(node->right, q);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::postorderTraverse(std::vector<Key>& q) {
	postorderTraverse(head, q);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::postorderTraverse(const Node *node, std::vector<Key>& q) {
	if(node == NULL)
		return ;
	postorderTraverse(node->left, q);
	postorderTraverse(node->right, q);
	q.push_back(node->key);
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isLinkConsistent() {
	return isLinkConsistent(head);
}

template<class Key, class Value>
bool BinarySearchTree<Key,Value>::isLinkConsistent(const Node *node) {
	if(node == NULL)
		return true;
	Node *left = node->left;
	Node *right = node->right;
	if(left && left->parent != node) {
		dumpNode(node);
		dumpNode(left);
		return false;
	}

	if(right && right->parent != node) {
		dumpNode(node);
		dumpNode(right);
		return false;
	}
	return isLinkConsistent(left) && isLinkConsistent(right);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::dump(const Node *node) {
	if(node == NULL)
		return ;

	(const_cast<BinarySearchTree<Key,Value>::Node *>(node))->dump();
	dump(node->left);
	dump(node->right);
}

template<class Key, class Value>
void BinarySearchTree<Key,Value>::dumpNode(const Node *node) {
	if(node != NULL)
		(const_cast<BinarySearchTree<Key,Value>::Node *>(node))->dump();
	else {
		std::cout << "trying dumping a NULL node " << std::endl;
	}
}
