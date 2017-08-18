/*
 * Heap.cpp
 *
 *  Created on: Aug 16, 2017
 *      Author: sheldon
 */
#include <cstdio>
#include <cassert>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <iostream>
#include "base/Common.h"
#include "Heap.h"

template class Heap<char>;
template <typename T>
Heap<T>::Heap() {

}

template<typename T>
Heap<T>::~Heap() {

}

template<typename T>
void Heap<T>::build(std::vector<T>& heap) {
	int size = heap.size() - 1;
	for(int n = size/2; n >= 1; n--) {
		sink(heap, n, size);
		//dump(heap);
	}
}

template<typename T>
void Heap<T>::sort(std::vector<T>& heap) {

	int size = heap.size() - 1;
	for(int i = size; i > 1; i --) {
		//heap[1] is the largest one, move to the end
		std::swap(heap.at(i), heap.at(1));
		sink(heap, 1, i - 1);
	}
}

template<typename T>
void Heap<T>::sink(std::vector<T>& heap, int k, int n) {
	assert(n <= heap.size() -1);
	while(2 * k <= n) {
		int j = 2 * k;//left child
		if(j < n && heap.at(j) < heap.at(j + 1)) {
			j ++;//2*k + 1; right child
		}

		if(heap.at(k) < heap.at(j)) {
			std::swap(heap.at(k), heap.at(j));
		} else
			break;

		k = j;
	}
}

template<typename T>
void Heap<T>::sink(std::vector<T>& heap, int k) {
	if(heap.size() <= 1)
		return ;
	std::cout << "sinking heap[" << k << "]:" << heap.at(k) << std::endl;
	sink(heap, k, heap.size() - 1);
}

template<typename T>
void Heap<T>::swim(std::vector<T>& heap, int k) {
	if(heap.size() <= 1)
		return ;

	int size = heap.size() - 1;
	while(k > 1) {
		if(heap.at(k) > heap.at(k/2))
			std::swap(heap.at(k), heap.at(k/2));
		else
			break;
		k /= 2;
	}

}
template<typename T>
void Heap<T>::insert(std::vector<T>& heap, T val) {
	heap.push_back(val);
	swim(heap, heap.size() - 1);
}

template<typename T>
void Heap<T>::remove(std::vector<T>& heap) {
	if(heap.size() < 2)
		return ;

	int size = heap.size() - 1;
	std::swap(heap.at(size), heap.at(1));
	heap.erase(heap.begin() + size);
	sink(heap, 1);
}

static void printWhiteSpace(int start, int len) {
	for(int i = start; i < len; i ++) {
		std::cout << ' ';
	}
}

//static
template<typename T>
const int Heap<T>::sBottomNodeInterval = 3;

template<typename T>
void Heap<T>::dump(std::string title, std::vector<char>& heap) {
	std::cout << std::endl << title + ":" << std::endl;
	//heap height
	int height = (int)floor(log(heap.size() - 1)/log(2));
	int mostBottomNode = (int)powl(2, height);

	//node, three space, node
	//makes bottom width: mostBottomNode + mostBottomNode/2 * sBottomNodeInterval + mostBottomNode/2 - 1;
	int bottomWidth = mostBottomNode + mostBottomNode/2 * sBottomNodeInterval + mostBottomNode/2 - 1;
	if(height == 0) {
		bottomWidth = 1;
	}

	//(bottomWidth + 1)/2 - 1 - (bottomWidth/2 + 1)/2
	int line = (bottomWidth + 1)/2;
	int startPos = bottomWidth/2;

//	std::cout << "startPos:" << startPos << ",line:" << line << ", bottomWidth:"
//			<< bottomWidth <<", mostBottomNode:" << mostBottomNode <<", height:" <<height <<std::endl;
	//how many nodes at line i
	int nodeCount = 1;
	int slashCount = 0;
	for(int l = 0; l < line;) {

		//print node value
		printWhiteSpace(0, startPos);
		int charCount = 2 * l + 1;
		int spaceCount = 2 * slashCount + 1;
		int inter;
		if(nodeCount >= 4) {
			inter = (charCount - (spaceCount * nodeCount/2) - nodeCount)/(nodeCount/2 - 1);
		} else {
			inter = 0;
		}

		for(int n = 0; n < nodeCount; n ++) {
			if(nodeCount + n >= heap.size())
				break;
			std::cout << heap.at(nodeCount + n);
			if(n + 1 < nodeCount) {
				if(n%2 == 0) {
					printWhiteSpace(0, spaceCount);
				} else {
					printWhiteSpace(0, inter);
				}
			}
		}
		std::cout << std::endl;
		l++;
		startPos--;
		//print slash and back slash

		slashCount = (bottomWidth + 1)/2 - 1 - (bottomWidth/2 + 1)/2;
		for(int s = 0; s < slashCount; s++) {
			//minus one '\' and one '/'
			spaceCount -= 2 ;
			printWhiteSpace(0, startPos);
			for(int n = 0; n < nodeCount; n++) {
				//check if there's child node
				if(2* (nodeCount + n ) < heap.size()) {
					std::cout << '/';
					//check if there's right child node
					if(2*(nodeCount + n) +1 < heap.size()) {
						printWhiteSpace(0, 2 * s + 1);
						std::cout << '\\';
					}

					//move to next sibling node
					if(n + 1 < nodeCount) {
						printWhiteSpace(0, spaceCount);
					}
				}
			}
			std::cout << std::endl;
			l++;
			startPos--;
		}
		bottomWidth /= 2;

		nodeCount *= 2;
	}
}


