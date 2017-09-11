/*
 * Others.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: sheldon
 */

// using atomic_flag as a lock
#include <iostream>       // std::cout
#include <atomic>         // std::atomic_flag
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <sstream>        // std::stringstream

using namespace std;

#include "gtest/gtest.h"

std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;
std::stringstream stream;

void append_number(int x) {
  cout << "thread:" << x << " starts" << endl;
  while (lock_stream.test_and_set()) {}
  cout << "thread #" << x << '\n';
  lock_stream.clear();
  cout << "thread:" << x << " ends" << endl;
}

TEST(MultiThreading, limits) {
  std::vector<std::thread> threads;
  for (int i=1; i<=10; ++i) threads.push_back(std::thread(append_number,i));
  for (auto& th : threads) th.join();

  std::cout << stream.str();
}

