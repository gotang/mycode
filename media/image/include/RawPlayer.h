#ifndef RAW_PLAYER_
#define RAW_PLAYER_
#include <pthread.h>
class RawPlayer {
public:
	RawPlayer();
	~RawPlayer();
	int init(const char *path, int width, int height, int format, int rotate, bool timeWatermark);
	int display();
	int reset();
	int setWarterMark(void *buf, int width, int height);

	int showBitmap(const char *path);
private:
	bool isBitMap(uint16_t header);
	int showYUVFrame();
	int getFrameSize(int format, int width, int height);
	void waitEvent(SDL_Event *event);
	int mFd;
	int mSrcWidth;
	int mSrcHeight;
	int mDestWidth;
	int mDestHeight;
	int mFormat;
	int mRotate;
	char *mUri;

	int mWindowWidth;
	int mWindowHeight;
#if defined(SDL1)
	SDL_Surface* mScreen;
	SDL_Overlay* mOverlay;
#elif defined(SDL2)
	SDL_Window *mWindow;
	SDL_Renderer *mRenderer;
	SDL_Texture *mTexture;
#endif
	uint8_t *mSourceBuffer;
	uint8_t *mDestBuffer;
	uint8_t *mRotateBuffer;
	int64_t mLastFrameTimeUs;

	bool mStop;
	bool mIsBitMap;
	bool mTimeWaterMark;
	void *mWaterMark;
	WaterMarkInData mWaterMarkInData;

	pthread_mutex_t mLock;
};
#endif //RAW_PLAYER_
