/*
 * Heap.h
 *
 *  Created on: Aug 16, 2017
 *      Author: sheldon
 */

#ifndef ALGORITHMS_SORTING_HEAP_H_
#define ALGORITHMS_SORTING_HEAP_H_

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


#endif /* ALGORITHMS_SORTING_HEAP_H_ */
