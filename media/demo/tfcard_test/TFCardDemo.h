#ifndef TF_CARD_DEMO_H_
#define TF_CARD_DEMO_H_

using namespace android;

enum TFCARD_DEMO_STATUS{
	OK = 0,
	ERROR_NOT_MOUNTED,
	ERROR_WRITE_FAILED,
	ERROR_READ_FAILED,
	ERROR_TFCARD_NOT_INSERTED,
};

class TFCardDemo {
public:
	TFCardDemo();
	virtual ~TFCardDemo();
	int init(const char *point);
	int run();
	int reset();
	void write();
	int32_t getErrorCode() { return mErrorCode; }
	static const char *strerror(int error);
private:
	bool isMounted();
	int32_t mErrorCode;
	char *mMountPoint;
};

#endif //TF_CARD_DEMO_H_
