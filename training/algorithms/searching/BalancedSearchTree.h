/*
 * BalancedSearchTree.h
 *
 *  Created on: Jun 30, 2017
 *      Author: sheldon
 */

#ifndef BALANCEDSEARCHTREE_H_
#define BALANCEDSEARCHTREE_H_

template<class Key, class Value>
class BalancedSearchTree {
public:
	struct Node;
	BalancedSearchTree();
	~BalancedSearchTree();
	const Node *getHead();
    // is the symbol table empty?
    bool isEmpty();

    // return number of key-value pairs in BST
    int size();
    // does there exist a key-value pair with given key?
    bool contains(const Key &key);
    // return value associated with the given key, or null if no such key exists
    Node* get(const Key &key);
   /***********************************************************************
	*  Insert key-value pair into BST
	*  If key already exists, update with new value
	***********************************************************************/
	void put(const Key &key, const Value &val);

	Node* max();
	Node* min();
	Node* floor(const Key &key);
	Node* ceiling(const Key &key);

	void deleteMin();
	void deleteMax();
	void remove(const Key &key);

    // Return key of rank k, k starts with 0.
	Node* select(int key);
    // Number of keys in the subtree less than x.key, i.e size(left child)
	int rank(const Key &key);

	// height of this BST (one-node tree has height 0)
	int height();

	//range search
	void keys(const Key &lo, const Key &hi, std::vector<Key>& q);

	void preoderTraverse(std::vector<Key>& q);
	void inorderTraverse(std::vector<Key>& q);
	void postorderTraverse(std::vector<Key>& q);

	void dump(const Node *node);

private:
	Node *head;
	void destroy(Node *node);
	bool isRed(Node *node);

	void flipColor(Node *node);
	//rotate right red link to left read link
	Node *rotateToLeft(Node *node);
	//rotate left red link to right read link
	Node *rotateToRight(Node *node);

	void dumpNode(const Node *node);

	void updateSize(Node *node);

	//define some functions we can do the relative operations recursively
	Node* get(Node *node, const Key &key);
	Node* put(Node *node, Node *parent, const Key &key, const Value &val);
	int size(const Node *node);

	Node* max(Node *node);
	Node* min(Node *node);

	Node* floor(Node *node, const Key &key);
	Node* ceiling(Node *node, const Key &key);

	Node* deleteMin(Node *node);
	Node* deleteMax(Node *node);
	Node* remove(Node *node, const Key &key);
	Node* select(Node *node, int k);
	int rank(Node *node, const Key &k);
	int height(Node *node);
	void keys(Node *node, const Key &lo, const Key &hi, std::vector<Key>& q);

	bool check();
    // check that ranks are consistent
    bool isRankConsistent();
    // are the size fields correct?
    bool isSizeConsistent();
    bool isSizeConsistent(const Node *node);

    bool isLinkConsistent();
    bool isLinkConsistent(const Node *node);

    // does this binary tree satisfy symmetric order?
    // Note: this test also ensures that data structure is a binary tree since order is strict
    bool isBinarySearchTree();
    bool isBinarySearchTree(const Node *node);

    bool isColorConsistent();
    bool isColorConsistent(const Node *node);

	void preoderTraverse(const Node *node, std::vector<Key>& q);
	void inorderTraverse(const Node *node, std::vector<Key>& q);
	void postorderTraverse(const Node *node, std::vector<Key>& q);

	BalancedSearchTree(const BalancedSearchTree&);
	BalancedSearchTree& operator=(const BalancedSearchTree&);
};

template<class Key, class Value>
struct BalancedSearchTree<Key, Value>::Node {
	Node(Node* parent, const Key &key, const Value &val, const int N, const int color);
	~Node();
	Key getKey();
	Value getValue();
	int getColor();
	friend class BalancedSearchTree<Key, Value>;
	void dump();
	enum COLOR{
		UNKOWN = 0,
		RED,
		BLACK,
	};
private:
	Key key;
	Value val;
	Node *parent;
	Node *left;
	Node *right;
	int color;
	int N;
	Node(const Node&);
	Node& operator=(const Node&);
};
#endif /* BALANCEDSEARCHTREE_H_ */
