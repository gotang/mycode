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
