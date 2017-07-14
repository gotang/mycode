#define LOG_NDEBUG 0
#define LOG_TAG "RTCDemo"
#include <utils/Log.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

#include "RTCDemo.h"

#define RTC_DEV   "/dev/rtc0"

RTCDemo::RTCDemo()
	:mErrorCode(0) {

}

RTCDemo::~RTCDemo() {

}

int RTCDemo::getRTCTime(char *point, int len) {
	struct rtc_time rtc_tm;
	int fd = open(RTC_DEV, O_RDWR);
	if(fd >= 0) {
		if (!ioctl(fd, RTC_RD_TIME, &rtc_tm)) {
			snprintf(point, len, "%.2d:%.2d:%.2d", rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
		} else {
			mErrorCode = ERROR_GET_RTC_TIME_FAILED;
		}
		close(fd);
	} else {
		mErrorCode = ERROR_GET_RTC_TIME_FAILED;
	}
	return 0;
}

//static
const char *RTCDemo::strerror(int error) {
	return NULL;
}
