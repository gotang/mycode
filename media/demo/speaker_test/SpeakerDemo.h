#ifndef SPEAKER_DEMO_H_
#define SPEAKER_DEMO_H_

using namespace android;

enum SPEAKER_DEMO_STATUS{
	OK = 0,
	ERROR_xxx,
};

class SpeakerDemo {
public:
	SpeakerDemo();
	~SpeakerDemo();
	int run();
	void setDataSource(const char *path);
	int32_t getErrorCode() { return mErrorCode; }
	static const char *strerror(int error);
private:
	char *mUri;
	int mErrorCode;
};

#endif //SPEAKER_DEMO_H_
