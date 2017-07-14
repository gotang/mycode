#ifndef MIC_DEMO_H_
#define MIC_DEMO_H_

using namespace android;

enum MIC_DEMO_STATUS{
	OK = 0,
	ERROR_xxx,
};

class MICDemo {
public:
	MICDemo();
	~MICDemo();
	int run();
	void setDataSource(const char *path);
	void setDuration(int sec);
	int32_t getErrorCode() { return mErrorCode; }
	static const char *strerror(int error);
private:
	char *mUri;
	int mErrorCode;
	int mDuration;
};

#endif //MIC_DEMO_H_
