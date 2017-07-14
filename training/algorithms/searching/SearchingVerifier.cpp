/*
 * AlgorithmsVerifier.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: sheldon
 */

#include <sys/time.h>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "gtest/gtest.h"
#include "base/Common.h"

#include "./searching/HashTables.h"
#include "./searching/BinarySearchTree.h"
#include "./searching/BalancedSearchTree.h"

TEST(Searching, BinarySearchTree) {
	BinarySearchTree<char, int> *bst = new BinarySearchTree<char, int>();
	std::string str = "SORTEXAMPLE";

	int r;
	std::vector<char> element;

	std::unordered_map<char, int> map;
	for(char c = 'A'; c <= 'Z'; c++) {
		map[c] = c - 'A';
	}

	for(int i = 0; i < str.size(); i ++) {
		bst->put(str.at(i), map[str.at(i)]);
	}

	printDelimiterLine(std::string("Testing size"));
	std::cout << "search tree size is " << bst->size() << std::endl;
	EXPECT_EQ(bst->size(), std::set<char>(str.begin(), str.end()).size());

	printDelimiterLine(std::string("Testing contains"));
	EXPECT_EQ(bst->contains('A'), true);
	EXPECT_EQ(bst->contains('Z'), false);

	printDelimiterLine(std::string("Testing get"));
	BinarySearchTree<char, int>::Node* node = bst->get('A');
	EXPECT_TRUE(node != NULL);
	EXPECT_EQ(node->getKey(), 'A');
	EXPECT_EQ(node->getValue(), map['A']);

	node = bst->get('Z');
	EXPECT_TRUE(node == NULL);

	printDelimiterLine(std::string("Testing max and min"));
	node = bst->max();
	std::cout << "max element is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), *std::max_element(str.begin(), str.end()));
	node = bst->min();
	std::cout << "min element is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), *std::min_element(str.begin(), str.end()));

	printDelimiterLine(std::string("Testing floor and ceiling"));
	node = bst->floor('E');
	std::cout << "floor of E is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'E');
	node = bst->ceiling('E');
	std::cout << "ceiling of E is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'E');

	node = bst->floor('F');
	std::cout << "floor of F is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'E');
	node = bst->ceiling('F');
	std::cout << "ceiling of F is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'L');

	printDelimiterLine(std::string("Testing traverse"));
	bst->preoderTraverse(element);
	dumpVector("preorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("SOEAMLRPTX"), 0);

	element.clear();
	bst->inorderTraverse(element);
	dumpVector("inorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("AELMOPRSTX"), 0);

	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("ALMEPROXTS"), 0);


	printDelimiterLine(std::string("Testing select"));
	node = bst->select(0);
	EXPECT_TRUE(node != NULL);
	std::cout << "No.0 is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'A');

	node = bst->select(str.size());
	EXPECT_TRUE(node == NULL);

	printDelimiterLine(std::string("Testing rank"));
	r = bst->rank('A');
	EXPECT_EQ(r, 0);

	r = bst->rank('R');
	EXPECT_EQ(r, 6);

	r = bst->rank('Z');
	EXPECT_EQ(r, -1);

	printDelimiterLine(std::string("Testing range search"));
	element.clear();
	bst->keys('F', 'U', element);
	std::ostream_iterator<char> out(std::cout, "");
	std::copy(element.begin(), element.end(), out);
	std::cout << std::endl;
	EXPECT_TRUE(std::string(element.begin(), element.end()).compare("LMOPRST") == 0);

	printDelimiterLine(std::string("Testing delete"));
	bst->deleteMax();
	bst->deleteMin();
	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("LMEPROTS"), 0);

	bst->remove('L');

	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("MEPROTS"), 0);

	bst->remove('S');
	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("MEPROT"), 0);
	std::cout << std::endl;

	bst->put('X', map['X']);
	bst->put('Q', map['Q']);
	bst->remove('O');
	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("MEQRPXT"), 0);
	std::cout << std::endl;

	delete bst;
}

TEST(Searching, BalancedSearchTree) {
	/*
	 *           R
	 *          / \
	 *         /   \
	 *     E--O     T
	 *    / \  \   / \
	 *   /   \  \ /   \
	 *  A  L--M P S    X
	 */
	BalancedSearchTree<char, int> *bst = new BalancedSearchTree<char, int>();
	std::string str = "SORTEXAMPLE";

	int r;
	std::vector<char> element;

	std::unordered_map<char, int> map;
	for(char c = 'A'; c <= 'Z'; c++) {
		map[c] = c - 'A';
	}

	for(int i = 0; i < str.size(); i ++) {
		bst->put(str.at(i), map[str.at(i)]);
	}
	bst->dump(bst->getHead());
	printDelimiterLine(std::string("Testing size"));
	std::cout << "search tree size is " << bst->size() << std::endl;
	EXPECT_EQ(bst->size(), std::set<char>(str.begin(), str.end()).size());

	printDelimiterLine(std::string("Testing color"));
	BalancedSearchTree<char, int>::Node *node;
	for(int i = 0; i < str.size(); i ++) {
		node = bst->get(str.at(i));
		EXPECT_TRUE(node != NULL);
		if(str.at(i) == 'E' || str.at(i) == 'L') {
			EXPECT_TRUE((node->getColor() == BalancedSearchTree<char, int>::Node::RED));
		} else {
			EXPECT_TRUE((node->getColor() == BalancedSearchTree<char, int>::Node::BLACK));
		}
	}

	printDelimiterLine(std::string("Testing contains"));
	EXPECT_EQ(bst->contains('A'), true);
	EXPECT_EQ(bst->contains('Z'), false);

	printDelimiterLine(std::string("Testing get"));
	node = bst->get('A');
	EXPECT_TRUE(node != NULL);
	EXPECT_EQ(node->getKey(), 'A');
	EXPECT_EQ(node->getValue(), map['A']);

	node = bst->get('Z');
	EXPECT_TRUE(node == NULL);

	printDelimiterLine(std::string("Testing max and min"));
	node = bst->max();
	std::cout << "max element is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), *std::max_element(str.begin(), str.end()));
	node = bst->min();
	std::cout << "min element is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), *std::min_element(str.begin(), str.end()));

	printDelimiterLine(std::string("Testing floor and ceiling"));
	node = bst->floor('E');
	std::cout << "floor of E is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'E');
	node = bst->ceiling('E');
	std::cout << "ceiling of E is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'E');

	node = bst->floor('F');
	std::cout << "floor of F is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'E');
	node = bst->ceiling('F');
	std::cout << "ceiling of F is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'L');

	printDelimiterLine(std::string("Testing traverse"));
	bst->preoderTraverse(element);
	dumpVector("preorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("ROEAMLPTSX"), 0);

	element.clear();
	bst->inorderTraverse(element);
	dumpVector("inorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("AELMOPRSTX"), 0);

	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("ALMEPOSXTR"), 0);


	printDelimiterLine(std::string("Testing select"));
	node = bst->select(0);
	EXPECT_TRUE(node != NULL);
	std::cout << "No.0 is " << node->getKey() << ", value " << node->getValue() << std::endl;
	EXPECT_EQ(node->getKey(), 'A');

	node = bst->select(str.size());
	EXPECT_TRUE(node == NULL);

	printDelimiterLine(std::string("Testing rank"));
	r = bst->rank('A');
	EXPECT_EQ(r, 0);

	r = bst->rank('R');
	EXPECT_EQ(r, 6);

	r = bst->rank('Z');
	EXPECT_EQ(r, -1);

	printDelimiterLine(std::string("Testing range search"));
	element.clear();
	bst->keys('F', 'U', element);
	std::ostream_iterator<char> out(std::cout, "");
	std::copy(element.begin(), element.end(), out);
	std::cout << std::endl;
	EXPECT_TRUE(std::string(element.begin(), element.end()).compare("LMOPRST") == 0);

#if 0
	printDelimiterLine(std::string("Testing delete"));
	bst->deleteMax();
	bst->deleteMin();
	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("LMEPROTS"), 0);

	bst->remove('L');

	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("MEPROTS"), 0);

	bst->remove('S');
	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("MEPROT"), 0);
	std::cout << std::endl;

	bst->put('X', map['X']);
	bst->put('Q', map['Q']);
	bst->remove('O');
	element.clear();
	bst->postorderTraverse(element);
	dumpVector("postorder", element);
	EXPECT_EQ(std::string(element.begin(), element.end()).compare("MEQRPXT"), 0);
	std::cout << std::endl;
#endif
	delete bst;
}

TEST(Searching, HashTables) {
	HashTables<char, int> *hashTable = new HashTables<char, int>();

	delete hashTable;
}
