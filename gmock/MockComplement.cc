#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include "MockComplement.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using namespace std;

bool WriteFile(FileIOInterface* fio,
               const char* filename, const void* data, size_t size) {
	FILE* file = fio->Open(filename, "wb");
	if (!file) {
		return false;
	}

	if (fio->Write(data, 1, size, file) != size) {
		return false;
	}

	if (fio->Close(file) != 0) {
		return false;
	}

	return true;
}

TEST(WriteFileTest, SuccessWorks) {
	static char data[] = "hello";
	const char* kName = "test";

	MockFileIO fio;

	FILE test_file;
	// Tell the mock to expect certain calls and what to
	// return on those calls.
	EXPECT_CALL(fio, Open(kName, "wb"))
		.WillOnce(Return(&test_file));

	EXPECT_CALL(fio, Write(&data[0], 1, sizeof(data), &test_file))
		.WillOnce(Return(sizeof(data)));

	EXPECT_CALL(fio, Close(&test_file))
		.WillOnce(Return(0));

	EXPECT_TRUE(WriteFile(&fio, kName, &data, sizeof(data)));
}

TEST(WriteFileTest, FailsIfOpenFails) {
	MockFileIO fio;

	static char data[] = "hello";
	const char* kName = "test";
	FILE test_file;

	// Tell the mock to expect certain calls and what to
	// return on those calls.
	EXPECT_CALL(fio, Open(kName, "wb"))
		.WillOnce(Return((FILE*)NULL));
	EXPECT_FALSE(WriteFile(&fio, kName, &data, sizeof(data)));
}

TEST(WriteFileTest, FailsIfWriteFails) {
	MockFileIO fio;

	static char data[] = "hello";
	const char* kName = "test";
	FILE test_file;
	// Tell the mock to expect certain calls and what to
	// return on those calls.
	EXPECT_CALL(fio, Open(kName, "wb"))
		.WillOnce(Return(&test_file));
	EXPECT_CALL(fio, Write(&data, 1, sizeof(data), &test_file))
		.WillOnce(Return(0));

	EXPECT_FALSE(WriteFile(&fio, kName, &data, sizeof(data)));
}

TEST(WriteFileTest, FailsIfCloseFails) {
	MockFileIO fio;

	static char data[] = "hello";
	const char* kName = "test";
	FILE test_file;
	// Tell the mock to expect certain calls and what to
	// return on those calls.
	EXPECT_CALL(fio, Open(kName, "wb"))
		.WillOnce(Return(&test_file));
	EXPECT_CALL(fio, Write(&data, 1, sizeof(data), &test_file))
		.WillOnce(Return(sizeof(data)));
	EXPECT_CALL(fio, Close(&test_file))
		.WillOnce(Return(EOF));

	EXPECT_FALSE(WriteFile(&fio, kName, &data, sizeof(data)));
}

//////////////////////////////////////////////////////////////////////////////

bool MyTest(MockTest *test) {

	for(int i = 0; i < 3; i ++) {
		printf("test[%d]:%d\n", i, test->test(i));
	}
	test->uninterestingTest(0);
	return true;
}

TEST(MyTest, MultipleExpectations) {
	NiceMock<MockTest> test;

//	EXPECT_CALL(test, test(_))
//		.WillOnce(Return(0))
//		.WillOnce(Return(1))
//		.WillRepeatedly(Return(2));
	{
		InSequence s;
		for (int i = 3; i > 0; i--) {
		  EXPECT_CALL(test, test(_))
			  .WillOnce(Return(10*i));
		}
	}

	EXPECT_TRUE(MyTest(&test));
}
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
	// Since Google Mock depends on Google Test, InitGoogleMock() is
	// also responsible for initializing Google Test.  Therefore there's
	// no need for calling testing::InitGoogleTest() separately.
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}
