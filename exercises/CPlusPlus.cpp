/*
 * CPlusPlus.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: sheldon
 */

#include <memory>
#include <vector>
#include <iostream>
#include <chrono> // for std::chrono functions
#include "gtest/gtest.h"
//#include "Common.h"

class Parent {
public:
	Parent() {
		std::cout << "Parent\n";
	}
	~Parent() {
		std::cout << "~Parent\n";
	}
	void set() {
		std::cout << "Parent::set\n";
	}

	virtual void get() {
		std::cout << "Parent::get\n";
	}

	virtual void what() {
		std::cout << "Parent::what\n";
	}
};

class Children: public Parent {
public:
	Children() {
		std::cout << "Children\n";
	}
	~Children() {
		std::cout << "~Children\n";
	}
	void get() {
		std::cout << "Children::get\n";
	}
	virtual void set() {
		std::cout << "Children::set\n";
	}
};

TEST(CPlusPlus, InheritancePart1) {
	Parent *p = new Parent;
	Children *c = new Children;

	std::cout << "Parent:" << sizeof(Parent) << ", Children " << sizeof(Children) << std::endl;
	Parent *p2 = dynamic_cast<Parent *> (c);

	p2->set();

	p->get();
	c->get();

	std::cout << "\ndynamic cast from children to parent\n";
	Parent *pp = dynamic_cast<Parent*>(c);
	std::cout << pp <<std::endl;
	if(pp) {
		pp->get();
	}
	std::cout << "\nstatic cast from children to parent\n";
	pp = static_cast<Parent*>(c);
	pp->get();

	std::cout << "\ndynamic cast from parent to children\n";
	//should be null
	Children *cc = dynamic_cast<Children*>(p);
	std::cout << cc <<std::endl;
	if(cc) {
		cc->get();
	}

	std::cout << "\nstatic cast from parent to children\n";
	cc = static_cast<Children*>(p);
	if(cc) {
		cc->get();
	}

	std::cout << std::endl;

	delete p;
	delete c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class DynamicArray
{
private:
	T* m_array;
	int m_length;

public:
	DynamicArray(int length)
		: m_length(length), m_array(new T[length])
	{
		std::cout<<"DynamicArray"<<std::endl;
	}

	~DynamicArray()
	{
		std::cout<<"!DynamicArray"<<std::endl;
		delete[] m_array;
	}

	// Copy constructor
	DynamicArray(const DynamicArray &arr) = delete;

	// Copy assignment
	DynamicArray& operator=(const DynamicArray &arr) = delete;

	// Move constructor
	DynamicArray(DynamicArray &&arr)
		: m_length(arr.m_length), m_array(arr.m_array)
	{

		std::cout<<"move constructor"<<std::endl;
		arr.m_length = 0;
		arr.m_array = nullptr;
	}

	// Move assignment
	DynamicArray& operator=(DynamicArray &&arr)
	{
		std::cout<<"move assignment"<<std::endl;
		if (&arr == this)
			return *this;

		delete[] m_array;

		m_length = arr.m_length;
		m_array = arr.m_array;
		arr.m_length = 0;
		arr.m_array = nullptr;

		return *this;
	}
	void print() {
		std::cout << "data address " << m_array << std::endl;
	}

	int getLength() const { return m_length; }
	T& operator[](int index) { return m_array[index]; }
	const T& operator[](int index) const { return m_array[index]; }

};

class Timer
{
private:
	// Type aliases to make accessing nested type easier
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer()
	:m_beg(clock_t::now()) {
	}

	void reset() {
		m_beg = clock_t::now();
	}

	double elapsed() const
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};


class CompanyA {
public:
	void sendCleartext(const std::string& msg) {

	};
	void sendEncrypted(const std::string& msg) {

	};
};
class CompanyB {
public:
	void sendCleartext(const std::string& msg) {

	};
	void sendEncrypted(const std::string& msg) {

	};
};
// classes for other companies
class MsgInfo {
};
// class for holding information
// used to create a message
template<typename Company>
class MsgSender {
public:
// ctors, dtor, etc.
	void sendClear(std::string str) {
		Company c;
		c.sendCleartext(str);
	}
	void sendSecret(std::string str) {
	}
};

template<typename Company>
class LoggingMsgSender: public MsgSender<Company> {
public:
// ctors, dtor, etc.
	void sendClearMsg(std::string str) {
		this->sendClear(str);
	}
};

TEST(CPlusPlus, AccessNamesInTemplatizedBaseClass) {
	LoggingMsgSender<CompanyB> b;
	b.sendClearMsg(std::string("adfad"));
}
