#ifndef MOCK_FILE_IO_INTERFACE_H
#define MOCK_FILE_IO_INTERFACE_H
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "BaseInterface.h"

class MockFileIO: public FileIOInterface {
public:
	MockFileIO() {}
	MOCK_METHOD2(Open, FILE *(const char *filename, const char *mode));
	MOCK_METHOD4(Write, size_t(const void* data, size_t size, size_t num, FILE* file));
	MOCK_METHOD1(Close, int(FILE* file));
};

class MockTest: public TestBase {
public:
	MockTest() {}
	MOCK_METHOD1(test, int(int));
	MOCK_METHOD1(uninterestingTest, int(int));
};

#endif // MOCK_FILE_IO_INTERFACE_H
