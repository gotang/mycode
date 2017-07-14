#ifndef LOG_H__
#define LOG_H__
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#ifndef likely
#define likely(x) __builtin_expect ((x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect ((x), 0)
#endif

#ifdef ANDROID
#include <cutils/log.h>
#ifndef ALOGH
#define ALOGH ALOGV("func:%s, line:%d", __func__, __LINE__);
#endif

#else//not android ANDROID
#include <stdio.h>
#include <stdint.h>

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
		if(unlikely(!(condition))) { \
			ALOGE("file:%s, line:%d, CHECK "#condition" failed", __FILE__, __LINE__); \
			abort(); \
		}\
	}while(0)

#define CHECK_EQ(x,y)   CHECK((x == y))
#define CHECK_NE(x,y)   CHECK((x != y))
#define CHECK_LE(x,y)   CHECK((x <= y))
#define CHECK_LT(x,y)   CHECK((x < y))
#define CHECK_GE(x,y)   CHECK((x >= y))
#define CHECK_GT(x,y)   CHECK((x > y))

#define TRESPASS() \
	do { \
		ALOGE("file:%s, line:%d, Should not be here", __FILE__, __LINE__); \
		abort(); \
	}while(0)

#endif
