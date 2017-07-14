#define LOG_NDEBUG 0
#define LOG_TAG "PlayerDemo"
#include <utils/Log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <media/hwdisp_def.h>

#include "PlayerDemo.h"

using namespace android;

enum MEDIA_TYPE {
	MEDIA_TYPE_UNKOWN = 0,
	MEDIA_TYPE_VIDEO = 1,
	MEDIA_TYPE_AUDIO = 2,
};

PlayerDemo::PlayerDemo()
	:mPlayer(0),
	 mDisplay(0),
	 mScreenWidth(0),
	 mScreenHeight(0),
	 mUri(NULL),
	 mRotateDegree(0),
	 mListener(NULL),
	 mErrorCode(0) {
	ALOGV("PlayerDemo");
	pthread_mutex_init(&mListenerLock, NULL);
}

PlayerDemo::~PlayerDemo() {
	ALOGV("~PlayerDemo");
	reset();
	pthread_mutex_destroy(&mListenerLock);
	mPlayer.clear();
}

void PlayerDemo::notify(int msg, int ext1, int ext2, const Parcel *obj) {
	int event = -1;
	switch(msg) {
	case MEDIA_PREPARED:
		break;
	case MEDIA_PLAYBACK_COMPLETE:
		event = PLAYER_DEMO_COMPLETED;
		break;
	case MEDIA_SEEK_COMPLETE:
		break;
	case MEDIA_ERROR:
		event = PLAYER_DEMO_ERROR;
		break;
	case MEDIA_INFO:
		break;
	default:
		break;
	}
	pthread_mutex_lock(&mListenerLock);
	if(event != -1 && mListener)
		mListener->notify(event, 0);
	pthread_mutex_unlock(&mListenerLock);
}

static int checkMediaType(const char *file) {
	int type;
	const char *suffix = strrchr(file, '.');
	if(suffix == NULL) {
		type = MEDIA_TYPE_UNKOWN;
	} else if(!strncasecmp(suffix, ".mp4", 4)) {
		type = MEDIA_TYPE_VIDEO;
	} else if(!strncasecmp(suffix, ".mov", 4)) {
		type = MEDIA_TYPE_VIDEO;
	} else if(!strncasecmp(suffix, ".ts", 3)) {
		type = MEDIA_TYPE_VIDEO;
	} else if(!strncasecmp(suffix, ".mp3", 4)) {
		type = MEDIA_TYPE_AUDIO;
	} else {
		type = MEDIA_TYPE_UNKOWN;
	}
	return type;
}

status_t PlayerDemo::init() {
	ALOGV("init");
	mPlayer = new MediaPlayer();
	mPlayer->setListener(this);

	if(access(mUri, F_OK)) {
		ALOGE("Access file %s denied, error %s", mUri, strerror(errno));
		mErrorCode = ERROR_INVALID_FILE;
		return -1;
	}

	ALOGV("file is %s", mUri);
	int fd = open(mUri, O_RDONLY);
	if(fd < 0) {
		ALOGE("open file %s failed", mUri);
		mErrorCode = ERROR_INVALID_FILE;
		return -1;
	}

	int err = mPlayer->setDataSource(fd, 0, 0);
	close(fd);

	int type = checkMediaType(mUri);
	if(type == MEDIA_TYPE_VIDEO) {
		view_info surface;
		surface.x = 0;
		surface.y = 0;
		surface.w = mScreenWidth;
		surface.h = mScreenHeight;

		mDisplay = Display::connect(1);
		if(mDisplay == NULL) {
			ALOGE("connect display failed");
			mErrorCode = ERROR_CONNECT_DISPLAY_FAILED;
			return -1;
		}
		mHalLayer = mDisplay->requestSurface((unsigned int)&surface);
		ALOGV("mHalLayer %d", mHalLayer);

		mPlayer->setVideoSurfaceTexture(mHalLayer);

		mPlayer->generalInterface(MEDIAPLAYER_CMD_SET_INIT_ROTATION, mRotateDegree/90, 0, 0, NULL);
	}
	//mediaplayer will prepare synchronously.
	err = mPlayer->prepare();
	if(err) {
		mErrorCode = ERROR_PREPARE_FAILED;
	}
	return err;
}

status_t PlayerDemo::start() {
	ALOGV("start");
	if(mDisplay != NULL) {
		mDisplay->open(8, 0);
	}
	mPlayer->setVolume(1.0, 1.0);
	int err = mPlayer->start();
	if(err) {
		mErrorCode = ERROR_PLAYBACK_FAILED;
	}
	return err;
}

status_t PlayerDemo::stop() {
	ALOGV("stop");
	int err = mPlayer->stop();
	if(mDisplay != NULL) {
		mDisplay->open(8, 1);
	}
	return err;
}

status_t PlayerDemo::reset() {
	ALOGV("reset");
	if(mDisplay != 0) {
		mDisplay->releaseSurface(mHalLayer);
		mDisplay.clear();
	}

	int err = 0;
	if(mPlayer != 0) {
		err = mPlayer->reset();
		mPlayer.clear();
	}

	if(mUri) {
		free(mUri);
		mUri = NULL;
	}
	return err;
}

status_t PlayerDemo::setDataSource(const char *file) {
	ALOGV("setDataSource");
	mUri = strdup(file);
	return 0;
}

status_t PlayerDemo::setScreenSize(int width, int height) {
	ALOGV("setScreenSize");
	mScreenWidth = width;
	mScreenHeight = height;
	return 0;
}

void PlayerDemo::setListener(PlayerDemoListener *listener) {
	pthread_mutex_lock(&mListenerLock);
	mListener = listener;
	pthread_mutex_unlock(&mListenerLock);
}

void PlayerDemo::setRotateDegree(int degree) {
	ALOGV("setRotateDegree");
	mRotateDegree = degree;
}

//static
const char *PlayerDemo::strerror(int error) {
	return NULL;
}
