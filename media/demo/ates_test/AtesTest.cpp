#define LOG_NDEBUG 0
#define LOG_TAG "AtesTest"
#include <utils/Log.h>

#include <DBCtrl.h>
#include <CedarXPlayer.h>
#include <CedarXRecorder.h>
#include <CamRecorderClient.h>

using namespace android;
extern "C" int atestb_check(void);

int check_libol() {
	int err = atestb_check();
	ALOGI("atestb_check return %d", err);
	return err;
}

int check_libCedarX() {
	CedarXRecorder *recorder = new CedarXRecorder();
	int err = recorder->isValid();
	delete recorder;

	CedarXPlayer *player = new CedarXPlayer();
	err = player->isValid();
	delete player;

	return err;
}

int check_libcamrecorder() {
	sp<CamRecorderClient> camrecorder = new CamRecorderClient();
	int err = camrecorder->isValid();
	camrecorder.clear();
	return err;
}

int check_libdatabase() {
	DBCtrl *database = new DBCtrl();
	int err = DBCtrl::isValid();
	delete database;
	return err;
}

#define CHECK_LIB(lib) \
	do { \
		ALOGV("checking "#lib"..."); \
		if (check_##lib() == 0) { \
			ALOGE("check "#lib" failed"); \
		} else { \
			ALOGV("check "#lib" success"); \
		} \
	}while(0)

int main()
{
	CHECK_LIB(libol);
	CHECK_LIB(libCedarX);
	CHECK_LIB(libcamrecorder);
	CHECK_LIB(libdatabase);

	return 0;
}
