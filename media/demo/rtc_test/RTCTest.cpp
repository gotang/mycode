#define LOG_NDEBUG 0
#define LOG_TAG "RTCTest"
#include <utils/Log.h>

#include "RTCDemo.h"

int main(int argc, char *argv[]) {
	RTCDemo *rtc = new RTCDemo();
	int size = 128;
	char tm[128];
	rtc->getRTCTime(tm, 128);
	ALOGV("time %s", tm);
	delete rtc;
	return 0;
}

