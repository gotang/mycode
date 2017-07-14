#ifndef BASE_INTERFACE_H
#define BASE_INTERFACE_H

#include <stdio.h>
#include <string>

class FileIOInterface {
public:
  virtual ~FileIOInterface() {}

  virtual FILE * Open(const char* filename, const char* mode) = 0;
  virtual size_t Write(const void* data, size_t size, size_t num, FILE* file) = 0;
  virtual int Close(FILE* file) = 0;
};


class TestBase {
public:
	virtual ~TestBase() {}
	virtual int test(int in) = 0;
	virtual int uninterestingTest(int in) = 0;
};

#endif // BASE_INTERFACE_H
