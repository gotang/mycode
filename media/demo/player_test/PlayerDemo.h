#ifndef PLAYER_DEMO_H_
#define PLAYER_DEMO_H_

#include <media/mediaplayer.h>
#include <display/Display.h>

using namespace android;

enum PLAYER_DEMO_STATUS{
	OK = 0,
	ERROR_PREPARE_FAILED,
	ERROR_INVALID_FILE,
	ERROR_CONNECT_DISPLAY_FAILED,
	ERROR_PLAYBACK_FAILED,
};

enum PLAYER_DEMO_EVENT {
	PLAYER_DEMO_ERROR,
	PLAYER_DEMO_PREPARED,
	PLAYER_DEMO_COMPLETED
};

class PlayerDemoListener {
public:
	virtual void notify(int msg, int ext1) = 0;
	virtual ~PlayerDemoListener() {}
};

class PlayerDemo :public MediaPlayerListener{
public:
	PlayerDemo();
	virtual ~PlayerDemo();
	status_t init();
	status_t start();
	status_t stop();
	status_t reset();
	status_t run();
	status_t setDataSource(const char *file);
	status_t setScreenSize(int width, int height);
	void setListener(PlayerDemoListener *listener);
	void setRotateDegree(int degree);//0,90,180,270.
	//herited from MediaPlayerListener
	virtual void notify(int msg, int ext1, int ext2, const Parcel *obj);

	int32_t getErrorCode() { return mErrorCode; }
	static const char *strerror(int error);
private:
	sp<MediaPlayer> mPlayer;
	sp<Display> mDisplay;
	int32_t mHalLayer;
	int32_t mStatus;
	int32_t mScreenWidth;
	int32_t mScreenHeight;
	char *mUri;
	int32_t mRotateDegree;

	PlayerDemoListener *mListener;
	pthread_mutex_t mListenerLock;

	int32_t mErrorCode;
};

#endif //PLAYER_DEMO_H_
