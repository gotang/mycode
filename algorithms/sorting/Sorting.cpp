/*
 * Algorithms.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: sheldon
 */

#include <cstdio>
#include <cassert>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <iostream>
#include "base/Common.h"
#include "Sorting.h"


template class Sorting<int>;
template class Sorting<char>;
template class Heap<char>;

///////////////////////////////////////////////////////////////////////
//Simple sorts:

/*
 * Time complexity:Average:O(n^2/2),Worst:O(n^2/2), Best:O(n^2/2)
 * Space complexity:O(1)
 * Stability:false
 */
template<typename T>
void Sorting<T>::selection(std::vector<T>& in) {
	for(int i = 0; i < in.size(); i ++) {
		int index = i;
		for(int j = i + 1; j < in.size(); j ++) {
			if(in.at(j) < in.at(index)) {
				index = j;
			}
		}
		std::swap(in[i], in[index]);
	}
}

/*
 * Time complexity:Average:O(n^2/4),Worst:O(n^2/2), Best:O(n-1)
 * Space complexity:O(1)
 * Stability:true
 */
template<typename T>
void Sorting<T>::insertion(std::vector<T>& in) {
	for(int i = 1; i < in.size(); i ++) {
		for(int j = i; j > 0; j --) {
			if(in.at(j) < in.at(j - 1)) {
				std::swap(in[j], in[j-1]);
			} else {
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
/*
 * Time complexity:Average:O(n^2),Worst:O(n^2), Best:O(n^2)
 * Space complexity:O(1)
 * Stability:true
 */
template<typename T>
void Sorting<T>::bubble(std::vector<T>& in) {
	for(int i = in.size(); i >= 0; i --) {
		for(int j = 1; j < i; j ++) {
			if(in[j] < in[j - 1]) {
				std::swap(in[j], in[j-1]);
			}
		}
	}
}

/*
 * Time complexity:Average:O(),Worst:O(), Best:O()
 * Space complexity:O(1)
 * Stability:false
 */
template<typename T>
void Sorting<T>::shell(std::vector<T>& in) {
	int len = in.size();
	int increment = 5;
	while(increment >= 1) {
		for(int i = increment; i < len; i ++ ) {
			for(int j = i; j >= increment; j -= increment) {
				if(in[j] < in[j - increment]) {
					std::swap(in[j], in[j-increment]);
				} else {
					break;
				}
			}
		}
		increment /=2;
	}
}

///////////////////////////////////////////////////////////////////////

/*
 * Time complexity:Average:O(),Worst:O(), Best:O()
 * Space complexity:O()
 * Stability:
 */
template<typename T>
void Sorting<T>::heap(std::vector<T>& in) {
	int size = in.size() - 1;
	//build heap
	for(int i = size/2; i >= 1; i --) {
		//do sink on every single parent node
		int k = i;
		while(2 * k <= size) {
			int j = 2 * k;
			if(j < size && in.at(j) < in.at(j + 1)) {
				j++;
			}
			if(in.at(k) < in.at(j)) {
				std::swap(in.at(k), in.at(j));
			} else
				break;
			k = j;
		}
	}
	//sorting
	for(int i = size; i > 1; i --) {
		std::swap(in.at(i), in.at(1));

		int k = 1;
		while(2 * k <= (i - 1)) {
			int j = 2 * k;
			if(j < (i - 1) && in.at(j) < in.at(j + 1))
				j ++;
			if(in.at(k) < in.at(j))
				std::swap(in.at(j), in.at(k));
			else
				break;
			k = j;
		}
	}
}

/*
 * Time complexity:Average:O(nlogn),Worst:O(nlogn), Best:O(nlogn)
 * Space complexity:O(n)
 * Stability:true
 */
template<typename T>
void Sorting<T>::merge(std::vector<T>& in, int start, int mid, int end, std::vector<T>& auxiliary) {

	//i:in[start, mid] vs temp[0, mid - start]
	int i = 0;
	//j:in[mid + 1, end] vs temp[mid + 1 - start, end - start]
	int j = mid + 1 - start;

	auxiliary.clear();
	auxiliary.assign(in.begin() + start, in.begin() + end + 1);

	//merge [start, mid] and [mid + 1, end]
	for(int step = start; step < end + 1; step++) {
		if(i > mid - start) {
			//left half exhausted
			in[step] = auxiliary[j++];
		} else if(j > end - start) {
			//right half exhausted
			in[step] = auxiliary[i++];
		} else if(auxiliary.at(i) > auxiliary.at(j)) {
			in[step] = auxiliary[j++];
		} else {
			in[step] = auxiliary[i++];
		}
	}
}

template<typename T>
void Sorting<T>::mergeTopDownRecursive(std::vector<T>& in, int start, int end, std::vector<T>& auxiliary) {
	if(start < end) {
		int mid = (start + end)/2;
		//sort left half
		mergeTopDownRecursive(in, start, mid, auxiliary);
		//sort right half
		mergeTopDownRecursive(in, mid + 1, end, auxiliary);
		merge(in, start, mid, end, auxiliary);
	}
}

//bottom-up merge
template<typename T>
void Sorting<T>::mergeBottomUp(std::vector<T>& in) {
	int count = in.size();
	std::vector<T> auxiliary;
	auxiliary.resize(in.size());

	//size:subarray size
	for(int size = 1; size < count; size += size) {
		for(int index = 0; index < count - size/*amount of remained elements is less than size, break*/;
				index += size + size/*move to next subarray pair*/) {

			int mid = index + size - 1;//equals to (index + index + size + size - 1)/2
			int end = std::min(index + size + size - 1, count -1);
			merge(in, index, mid, end, auxiliary);
		}
	}
}

template<typename T>
void Sorting<T>::merge(std::vector<T>& in) {
#if RECURSIVE
	std::vector<T> auxiliary;
	auxiliary.resize(in.size());
	mergeTopDownRecursive(in, 0, in.size() - 1, auxiliary);
#else
	mergeBottomUp(in);
#endif
}

/*
 * Time complexity:Average:O(),Worst:O(), Best:O()
 * Space complexity:O()
 * Stability:
 */
template<typename T>
void Sorting<T>::quickRecursive(std::vector<T>& in, int start, int end) {

	if(start >= end) {
		return ;
	}
	T partition = in.at(start);
	int i = start;
	int j = end;

	while(i < j) {
		for(i++; i < end + 1; i++) {
			if(in[i] > partition)
				break;
		}
		for(;j > start; j --) {
			if(in[j] < partition)
				break;
		}
		if(i >= j)
			break;

		std::swap(in[j], in[i]);
	}
	std::swap(in[j], in[start]);

	quickRecursive(in, start, j - 1);
	quickRecursive(in, j + 1, end);
}

template<typename T>
void Sorting<T>::quick(std::vector<T>& in) {
	quickRecursive(in, 0, in.size() - 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////
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
