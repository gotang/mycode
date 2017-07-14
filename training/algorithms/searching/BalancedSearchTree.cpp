/*
 * BalancedSearchTree.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: sheldon
 */
#include <cassert>
#include <vector>
#include <iostream>

#include "BalancedSearchTree.h"

template class BalancedSearchTree<char, int>;

template<class Key, class Value>
BalancedSearchTree<Key,Value>::Node::Node(Node* parent, const Key &key,
		const Value &val, const int N, const int color) {
	this->key = key;
	this->val = val;
	this->N = N;
	this->color = color;
	this->parent = parent;
	left = right = NULL;
	std::cout << "create node " << this << std::endl;
}

template<class Key, class Value>
BalancedSearchTree<Key,Value>::Node::~Node() {
	std::cout << "delete node " << this << std::endl;
}

template<class Key, class Value>
Key BalancedSearchTree<Key,Value>::Node::getKey() {
	return key;
}

template<class Key, class Value>
Value BalancedSearchTree<Key,Value>::Node::getValue() {
	return val;
}

template<class Key, class Value>
int BalancedSearchTree<Key,Value>::Node::getColor() {
	return color;
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::Node::dump() {
	std::cout << "node is " << key << ", color is " << color;
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

////////////////////////////////////////////////////////////////////////////////////////
template<class Key, class Value>
BalancedSearchTree<Key,Value>::BalancedSearchTree()
	:head(NULL) {

}

template<class Key, class Value>
BalancedSearchTree<Key,Value>::~BalancedSearchTree() {
	destroy(head);
	head = NULL;
	assert(isEmpty() && "Search tree is not empty after destroy");
}


template<class Key, class Value>
void BalancedSearchTree<Key,Value>::destroy(Node *node) {
	if(node == NULL)
		return ;
	Node *left = node->left;
	Node *right = node->right;
	delete node;
	destroy(left);
	destroy(right);
}

template<class Key, class Value>
const typename BalancedSearchTree<Key,Value>::Node* BalancedSearchTree<Key,Value>::getHead() {
	return head;
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isEmpty() {
	return size() == 0;
}

template<class Key, class Value>
int BalancedSearchTree<Key,Value>::size() {
	return size(head);
}

template<class Key, class Value>
int BalancedSearchTree<Key,Value>::size(const Node *node) {
	if(node == NULL)
		return 0;
//	return node->N;
	return 1 + size(node->left) + size(node->right);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::updateSize(Node *node) {
	if(node != NULL) {
		node->N = 1 + size(node->left) + size(node->right);
	}
}
template<class Key, class Value>
void BalancedSearchTree<Key,Value>::flipColor(Node *node) {
	if(node == NULL)
		return ;
	node->color = Node::RED;
	if(node->left)
		node->left->color = Node::BLACK;
	if(node->right)
		node->right->color = Node::BLACK;
}

//rotate right red link to left read link
template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node* BalancedSearchTree<Key,Value>::rotateToLeft(Node *node) {
	if(node == NULL)
		return NULL;
	Node *n = node->right;
	node->right = n->left;
	if(node->right) {
		node->right->parent = node;
	}
	n->parent = node->parent;
	n->left = node;
	n->color = node->color;
	n->N = node->N;

	node->parent = n;
	node->color = Node::RED;
	updateSize(node);
	return n;
}
//rotate left red link to right read link
template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node* BalancedSearchTree<Key,Value>::rotateToRight(Node *node) {
	if(node == NULL)
		return NULL;
	Node *n = node->left;
	node->left = n->right;
	if(node->left) {
		node->left->parent = node;
	}
	n->right = node;
	n->parent = node->parent;
	n->N = node->N;
	n->color = node->color;

	node->color = Node::RED;
	node->parent = n;
	updateSize(node);
	return n;
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isRed(Node *node) {
	if(node == NULL)
		return false;
	return node->color == Node::RED;
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::contains(const Key &key) {
	return get(key) != NULL;
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::get(const Key &key) {
	return get(head, key);
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node*
BalancedSearchTree<Key,Value>::get(Node *node, const Key &key) {
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
void BalancedSearchTree<Key,Value>::put(const Key &key, const Value &val) {
	std::cout << "putting [" << key <<", " << val <<"]" << std::endl;
	head = put(head, NULL, key, val);
	if(head) {
		head->color = Node::BLACK;
	}
	assert(check() && "Check search tree failed after put");
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node*
BalancedSearchTree<Key,Value>::put(Node *node, Node *parent, const Key &key, const Value &val) {
	if(node == NULL)
		return new Node(parent, key, val, 1, Node::RED);

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
	if(isRed(node->right) && !isRed(node->left)) {
		node = rotateToLeft(node);
	}

	if(node->left && isRed(node->left) && isRed(node->left->left)) {
		node = rotateToRight(node);
	}

	if(isRed(node->left) && isRed(node->right)) {
		//set both left and right as BLACK node
		//node as RED node
		flipColor(node);
	}

	updateSize(node);
	return node;
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::min() {
	return min(head);
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::min(Node *node) {
	if(node == NULL)
		return NULL;

	if(node->left != NULL)
		return min(node->left);

	return node;
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::max() {
	return max(head);
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::max(Node *node) {
	if(node == NULL)
		return NULL;

	if(node->right != NULL)
		return max(node->right);

	return node;
}

template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node*
BalancedSearchTree<Key,Value>::floor(const Key &key) {
	return floor(head, key);
}

template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node*
BalancedSearchTree<Key,Value>::floor(Node *node, const Key &key) {
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
typename BalancedSearchTree<Key,Value>::Node*
BalancedSearchTree<Key,Value>::ceiling(const Key &key) {
	return ceiling(head, key);
}

template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node*
BalancedSearchTree<Key,Value>::ceiling(Node *node, const Key &key) {
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
void BalancedSearchTree<Key,Value>::deleteMin() {
	head = deleteMin(head);
	assert(check() && "Check search tree failed after deleteMin");
}

template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node*
BalancedSearchTree<Key,Value>::deleteMin(Node *node) {
	if(node == NULL)
		return NULL;
	//TODO: finish
	return node;
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::deleteMax() {
	head = deleteMax(head);
	assert(check() && "Check search tree failed after deleteMax");
}

template<class Key, class Value>
typename BalancedSearchTree<Key,Value>::Node*
BalancedSearchTree<Key,Value>::deleteMax(Node *node) {
	if(node == NULL)
		return NULL;
	//TODO: finish
	return node;
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::remove(const Key &key) {
	std::cout << "removing " << key << std::endl;
	head = remove(head, key);//update head
	assert(check());
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node*
BalancedSearchTree<Key,Value>::remove(Node *node, const Key &key) {
	if(node == NULL)
		return NULL;
	//TODO: finish
	return node;
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::select(int k) {
	return select(head, k);
}

template<class Key, class Value>
typename BalancedSearchTree<Key, Value>::Node* BalancedSearchTree<Key,Value>::select(Node *node, int k) {
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
int BalancedSearchTree<Key,Value>::rank(const Key &k) {
	if(!contains(k)) {
		return -1;
	}
	return rank(head, k);
}

template<class Key, class Value>
int BalancedSearchTree<Key,Value>::rank(Node *node, const Key &k) {
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
int BalancedSearchTree<Key,Value>::height() {
	return height(head);
}

template<class Key, class Value>
int BalancedSearchTree<Key,Value>::height(Node *node) {
	if(node == NULL)
		return -1;
	return 1 + std::max(height(node->left), height(node->right));
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::keys(const Key &lo, const Key &hi, std::vector<Key>& q){
	keys(head, lo, hi, q);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::keys(Node *node, const Key &lo, const Key &hi, std::vector<Key>& q){
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
void BalancedSearchTree<Key,Value>::preoderTraverse(std::vector<Key>& q) {
	preoderTraverse(head, q);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::preoderTraverse(const Node *node, std::vector<Key>& q) {
	if(node == NULL)
		return ;
	q.push_back(node->key);
	preoderTraverse(node->left, q);
	preoderTraverse(node->right, q);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::inorderTraverse(std::vector<Key>& q) {
	inorderTraverse(head, q);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::inorderTraverse(const Node *node, std::vector<Key>& q) {
	if(node == NULL)
		return ;
	inorderTraverse(node->left, q);
	q.push_back(node->key);
	inorderTraverse(node->right, q);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::postorderTraverse(std::vector<Key>& q) {
	postorderTraverse(head, q);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::postorderTraverse(const Node *node, std::vector<Key>& q) {
	if(node == NULL)
		return ;
	postorderTraverse(node->left, q);
	postorderTraverse(node->right, q);
	q.push_back(node->key);
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::check() {
	bool ifBinarySearchTree = isBinarySearchTree();
	bool ifRankConsistent = isRankConsistent();
	bool ifSizeConsistent = isSizeConsistent();
	bool ifLinkConsistent = isLinkConsistent();
	bool ifColorConsistent = isColorConsistent();
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
	if(!ifColorConsistent) {
		std::cout << "Color consistent check failed" << std::endl;
	}
	return ifBinarySearchTree && ifRankConsistent && ifSizeConsistent && ifLinkConsistent && ifColorConsistent;
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isRankConsistent() {
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

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isColorConsistent() {
	return isColorConsistent(head);
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isColorConsistent(const Node *node) {
	if(node == NULL)
		return true;
	if(node->color != Node::RED && node->color != Node::BLACK)
		return false;
	return isColorConsistent(node->left) && isColorConsistent(node->right);
}

// are the size fields correct?
template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isSizeConsistent() {
	return isSizeConsistent(head);
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isSizeConsistent(const Node *node) {
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
bool BalancedSearchTree<Key,Value>::isBinarySearchTree() {
	return isBinarySearchTree(head);
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isBinarySearchTree(const Node *node) {
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
bool BalancedSearchTree<Key,Value>::isLinkConsistent() {
	return isLinkConsistent(head);
}

template<class Key, class Value>
bool BalancedSearchTree<Key,Value>::isLinkConsistent(const Node *node) {
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
void BalancedSearchTree<Key,Value>::dump(const Node *node) {
	if(node == NULL)
		return ;

	(const_cast<BalancedSearchTree<Key,Value>::Node *>(node))->dump();
	dump(node->left);
	dump(node->right);
}

template<class Key, class Value>
void BalancedSearchTree<Key,Value>::dumpNode(const Node *node) {
	if(node != NULL)
		(const_cast<BalancedSearchTree<Key,Value>::Node *>(node))->dump();
	else {
		std::cout << "trying dumping a NULL node " << std::endl;
	}
}

