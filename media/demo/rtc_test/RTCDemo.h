#ifndef RTC_DEMO_H_
#define RTC_DEMO_H_

using namespace android;

enum RTC_DEMO_STATUS{
	OK = 0,
	ERROR_GET_RTC_TIME_FAILED,
};

class RTCDemo {
public:
	RTCDemo();
	~RTCDemo();
	int getRTCTime(char *point, int len);
	int32_t getErrorCode() { return mErrorCode; }
	static const char *strerror(int error);
private:
	int mErrorCode;
};

#endif //RTC_DEMO_H_
