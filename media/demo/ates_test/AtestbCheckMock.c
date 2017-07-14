#define LOG_NDEBUG 0
#define LOG_TAG "atestb_check_mock"
#include <utils/Log.h>

#include <ol/ol.h>

int atestb_check() {
	ALOGD("atestb_check start");
	int err = 1;
	ALOGD("atestb_check return %d", err);
	return err;
}
