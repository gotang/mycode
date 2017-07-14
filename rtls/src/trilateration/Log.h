#ifndef LOG_H__
#define LOG_H__
#include <stdio.h>
#include <time.h>
#ifdef ANDROID
#include <cutils/log.h>
#ifndef ALOGH
#define ALOGH ALOGV("func:%s, line:%d", __func__, __LINE__);
#endif

#else//ANDROID
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

typedef int status_t;

enum {
	OK = 0,
	ERROR_UNSUPPORTED = -1,
};

#define LOG(level, fmt, arg...) \
	do {\
		time_t ti;\
		struct tm *tm; \
		ti = time(NULL); \
		tm = gmtime(&ti); \
		printf("%02d:%02d %02d:%02d:%02d ", tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec); \
		printf(#level"/%s(%d):"fmt, LOG_TAG, getpid(), ##arg); \
		printf("\n"); \
	} while(0)

#ifndef LOG_NDEBUG
#define LOG_NDEBUG 1
#endif

#if LOG_NDEBUG == 0
#define ALOGV(...) \
	LOG(V, __VA_ARGS__);

#define ALOGH ALOGV("func:%s, line:%d", __func__, __LINE__);
#else
#define ALOGV(...) \
	(void(0))

#define ALOGH
#endif

#define ALOGD(fmt, arg...) \
	LOG(D, fmt, ##arg)
#define ALOGI(fmt, arg...) \
	LOG(I, fmt, ##arg)
#define ALOGW(fmt, arg...) \
	LOG(W, fmt, ##arg)
#define ALOGE(fmt, arg...) \
	LOG(E, fmt, ##arg)

#endif

#define CHECK(condition) \
	do { \
		if(!(condition)) { \
			ALOGE("file:%s, line;%d, CHECK "#condition" failed", __FILE__, __LINE__); \
			abort(); \
		}\
	}while(0)

#define TRESPASS() \
	do { \
		ALOGE("Shouldn't be here !!!"); \
		abort(); \
	}while(0)

#endif
