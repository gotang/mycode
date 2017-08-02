/*
 * Algorithms.h
 *
 *  Created on: Jun 7, 2017
 *      Author: sheldon
 */

#ifndef SORTING_H_
#define SORTING_H_

template<typename T>
class Sorting {
public:

	//Simple sorts:
	void selection(std::vector<T>& in);
	void insertion(std::vector<T>& in);

	//Bubble sort and variants
	void bubble(std::vector<T>& in);
	void shell(std::vector<T>& in);

	//Efficient sorts
	void merge(std::vector<T>& in);
	void quick(std::vector<T>& in);
	void heap(std::vector<T>& in);
private:
	void merge(std::vector<T>& in, int start, int mid, int end, std::vector<T>& auxiliary);
	void mergeTopDownRecursive(std::vector<T>& in, int start, int end, std::vector<T>& auxiliary);
	void mergeBottomUp(std::vector<T>& in);

	void quickRecursive(std::vector<T>& in, int start, int end);
};

template<typename T>
class Heap {
public:
	Heap();
	~Heap();
	void build(std::vector<T>& heap);
	void sort(std::vector<T>& heap);
	void sink(std::vector<T>& heap, int k);
	void swim(std::vector<T>& heap, int k);

	void insert(std::vector<T>& heap, T val);
	void remove(std::vector<T>& heap);

	void dump(std::string title, std::vector<char>& heap);
	int getSlashCount(int height);
	static const int sBottomNodeInterval;
private:
	void sink(std::vector<T>& heap, int k, int n);
};


#endif /* SORTING_H_ */
