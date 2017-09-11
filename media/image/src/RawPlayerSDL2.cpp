#define LOG_NDEBUG 0
#define LOG_TAG "RawPlayer"
#include "Log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#include "ColorConverter.h"
#include "waterMarkInterface.h"
#include "SDL.h"
#include "RawPlayer.h"

#define MAX_WINDOW_WIDTH  (1280)
#define MAX_WINDOW_HEIGHT (720)

static uint64_t getNowUs(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) tv.tv_sec * 1000000 + tv.tv_usec;
}

static void hexdump(const char *title, void * buf, uint32_t  size) {

	uint32_t i = 0;
	int32_t line ;

	int8_t tmp[32 + 16 + 11 + 1];
	uint8_t *data = (uint8_t *)buf;
	uint32_t  offset = 0;
	ALOGD("%s:", title);
	for(line = 0; offset < size; line ++ ) {
		sprintf((char*)&tmp[0], "0x%08x:", line*16);
		if(size - offset >= 16u) {
			for(i = 0; i < 16; i++) {
				sprintf((char*)&tmp[(i + 1) * 3 + 8], "%02x ", data[offset + i]);
			}
			offset += 16;
		} else {
			for(i = 0; i < size - offset; i++) {
				sprintf((char*)&tmp[(i + 1) * 3 + 8], "%02x ", data[offset + i]);
			}
			offset = size;
		}
		tmp[32 + 16 + 11] = '\0';
		ALOGD("%s", tmp);
	}
}

static void getTimeString(char *str, int len)
{
	time_t timep;
	struct tm *p;
	int year, month, day, hour, min, sec;

	time(&timep);
	p = localtime(&timep);
	year = p->tm_year + 1900;
	month = p->tm_mon + 1;
	day = p->tm_mday;
	hour = p->tm_hour;
	min = p->tm_min;
	sec = p->tm_sec;

	snprintf(str, len, "%d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, min, sec);
}

RawPlayer::RawPlayer()
	:mFd(-1),
	 mUri(NULL),
	 mWindow(NULL),
	 mRenderer(NULL),
	 mTexture(NULL),
	 mSourceBuffer(NULL),
	 mDestBuffer(NULL),
	 mRotateBuffer(NULL),
	 mLastFrameTimeUs(-1),
	 mStop(false),
	 mIsBitMap(false),
	 mTimeWaterMark(false),
	 mWaterMark(NULL) {
	ALOGV("RawPlayer");
}

RawPlayer::~RawPlayer() {
	ALOGV("~RawPlayer");
	reset();
}

bool RawPlayer::isBitMap(uint16_t header) {
	if(header == 0x4d42) {
		return 1;
	}
	return 0;
}

int RawPlayer::init(const char *path, int width, int height, int format, int rotate, bool timeWatermark) {
	ALOGV("init");
	if(width <= 0 || height <= 0) {
		ALOGE("invalid picture size (%d, %d)", width, height);
		return -1;
	}

	mFd = open(path, O_RDONLY);
	if(mFd < 0) {
		ALOGE("open file failed");
		return -1;
	}

	mUri = strdup(path);

	int flags = SDL_INIT_VIDEO;
	if (SDL_Init(flags) < 0) {
		ALOGE("can not initialize SDL");
		return -1;
	}
	atexit (SDL_Quit);

	uint16_t header;
	read(mFd, &header, 2);
	mIsBitMap = isBitMap(header);
	if(mIsBitMap) {
		lseek(mFd, 18, SEEK_SET);
		read(mFd, &width, 4);
		read(mFd, &height, 4);
		width = abs(width);
		height = abs(height);

		lseek(mFd, 28, SEEK_SET);
		read(mFd, &header, 2);
		ALOGV("bit map size (%d, %d)", width, height);
		if(header == 32) {
			format  = COLOR_FORMAT_ARGB8888;
		} else if(header == 24){
			format  = COLOR_FORMAT_RGB888;
		} else if(header == 16){
			format  = COLOR_FORMAT_ARGB16;
		} else if(header == 8){
			format  = COLOR_FORMAT_ARGB8;
		} else if(header == 4){
			format  = COLOR_FORMAT_ARGB4;
		} else if(header == 1){
			format  = COLOR_FORMAT_ARGB1;
		} else {
			ALOGE("invalid bitcount %d", header);
			return -1;
		}
	}

	lseek(mFd, 0, SEEK_SET);

	mSrcWidth = width;
	mSrcHeight = height;
	mFormat = format;
	mRotate = rotate;

	if(mRotate == 90 || mRotate == 270) {
		//switch width and height
		width = height - width;
		height = height - width;
		width = height + width;
	}
	mDestWidth = width;
	mDestHeight = height;

	if(mSrcWidth > MAX_WINDOW_WIDTH || mSrcHeight > MAX_WINDOW_HEIGHT) {
		mWindowWidth = MAX_WINDOW_WIDTH;
		mWindowHeight = MAX_WINDOW_HEIGHT;
	} else {
		mWindowWidth = mSrcWidth;
		mWindowHeight = mSrcHeight;
	}
	ALOGD("window size(%d, %d)", mWindowWidth, mWindowHeight);
	bool isFullScreen = false;
	bool borderless = false;
	flags = SDL_WINDOW_SHOWN;
	if (isFullScreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	if (borderless)
		flags |= SDL_WINDOW_BORDERLESS;
	else
		flags |= SDL_WINDOW_RESIZABLE;

	const char *windowTittle = path;
	mWindow = SDL_CreateWindow(windowTittle, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, mWindowWidth, mWindowHeight, flags);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	if (mWindow) {
		SDL_RendererInfo info;
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!mRenderer) {
			ALOGE("Failed to initialize a hardware accelerated renderer: %s\n", SDL_GetError());
			mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
		}
		if (mRenderer) {
			if (!SDL_GetRendererInfo(mRenderer, &info))
				ALOGI("Initialized %s renderer.\n", info.name);
		}
	}
    //alloc texture
    int sdl_pix_fmt = mFormat == COLOR_FORMAT_YUV420_PLANNAR ? SDL_PIXELFORMAT_YV12 : SDL_PIXELFORMAT_ARGB8888;
	void *pixels;
	int pitch;
	if (!(mTexture = SDL_CreateTexture(mRenderer, sdl_pix_fmt,
			SDL_TEXTUREACCESS_STREAMING, mDestWidth, mDestHeight)))
		return -1;
	if (SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_NONE) < 0)
		return -1;

	ALOGV("screen is (%d, %d), format %d, rotate %d", width, height, mFormat, mRotate);
	return 0;
}

static void printCommandLine() {
	printf("\ninput following character:\n");
	printf("\tq:exit demo\n");
	printf("\thit any other key to play next frame\n");
}

static int getInput() {
	char buf[3];
	ALOGV("loop");
	printCommandLine();
	buf[0] = '\0';
	//get keyboard input.
	fd_set rfds;
	struct timeval tv;
	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	/* Wait up to five seconds. */
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	int retval = select(1, &rfds, NULL, NULL, NULL);
	if (retval > 0) {
		//read
		read(0, buf, 3);
		switch (buf[0]) {
		case 'q':
			retval = 1;
			break;
		default:
			break;
		}
	}
	return retval;
}

int RawPlayer::getFrameSize(int format, int width, int height) {
	int frameSize = 0;
	if(format == COLOR_FORMAT_ARGB8888) {
		frameSize = 4 * width * height;
	} else if(mFormat > COLOR_FORMAT_YUV_MIN && mFormat < COLOR_FORMAT_YUV_MAX) {
		frameSize = width * height * 3 / 2;
	}
	return frameSize;
}

int RawPlayer::showYUVFrame() {
	ALOGV("showYUVFrame");
	int YSize = mSrcWidth * mSrcHeight;
	int USize = YSize/4;
	int frameSize = getFrameSize(mFormat, mSrcWidth, mSrcHeight);
	if(frameSize <= 0) {
		return -1;
	}

	uint8_t *yv12;
	if(mIsBitMap) {
		lseek(mFd, 54, SEEK_SET);
	}
	ssize_t readBytes = read(mFd, mSourceBuffer, frameSize);
	if(readBytes != frameSize) {
		ALOGE("read return %zd, wanted %d", readBytes, frameSize);
		return -1;
	}
	int err;
	//first: convert to YV12
	if(COLOR_FORMAT_YVU420_PLANNAR != mFormat &&
			COLOR_FORMAT_YUV420_PLANNAR != mFormat) {
		BitmapParams source(mSourceBuffer, mFormat, mSrcWidth, mSrcHeight, 0, 0, 0, 0);
		BitmapParams yuv(mDestBuffer, COLOR_FORMAT_YVU420_PLANNAR, mSrcWidth, mSrcHeight, 0, 0, 0, 0);
		err = colorConvert(&source, &yuv);
		ALOGV("convert %d", err);
		yv12 = mDestBuffer;
	} else {
		yv12 = mSourceBuffer;
	}
	//next: rotate
	if(mRotateBuffer) {
		//It's YV12 now.
		BitmapParams source(yv12, COLOR_FORMAT_YVU420_PLANNAR, mSrcWidth, mSrcHeight, 0, 0, 0, 0);
		BitmapParams rotated(mRotateBuffer, COLOR_FORMAT_YVU420_PLANNAR, mSrcHeight, mSrcWidth, 0, 0, 0, 0);
		err = pictureRotate(&source, &rotated, mRotate);
		yv12 = mRotateBuffer;
	}

    //SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mRenderer);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = mDestWidth;
    rect.h = mDestHeight;
    if(mFormat == COLOR_FORMAT_YUV420_PLANNAR) {
	    SDL_UpdateYUVTexture(mTexture, &rect,
	    		yv12, mDestWidth,
	    		yv12 + YSize, mDestWidth/2,
				yv12 + YSize + USize, mDestWidth/2);
	} else {
		//others converted to YV12
	    SDL_UpdateYUVTexture(mTexture, &rect,
	    		yv12, mDestWidth,
				yv12 + YSize + USize, mDestWidth/2,
	    		yv12 + YSize, mDestWidth/2);
	}
    SDL_RenderCopy(mRenderer, mTexture, NULL, NULL);
    SDL_RenderPresent(mRenderer);

	return 0;
}

int RawPlayer::setWarterMark(void *buf, int width, int height) {
	char buffer[128];
	getTimeString(buffer, 128);

	uint8_t *y = (uint8_t *)buf;
	mWaterMarkInData.y = y;
	mWaterMarkInData.c = y + width * height;
	mWaterMarkInData.width = width;
	mWaterMarkInData.height = height;
	mWaterMarkInData.main_channel = 1;
	//mWaterMarkInData.scale = (float)640/(float)1080;

	doWaterMarkMultiple(&mWaterMarkInData, mWaterMark, buffer);
	return 0;
}

void RawPlayer::waitEvent(SDL_Event *event) {
    SDL_PumpEvents();
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
        SDL_PumpEvents();
    }
}

/* handle an event sent by the GUI */
int RawPlayer::display() {
    SDL_Event event;
    ALOGV("displayYUV");
	int frameSize = getFrameSize(mFormat, mSrcWidth, mSrcHeight);
	mSourceBuffer = (uint8_t *)malloc(frameSize);
	CHECK(mSourceBuffer != NULL);

	if(COLOR_FORMAT_YUV420_PLANNAR != mFormat) {
		mDestBuffer= (uint8_t *)malloc(frameSize);
		CHECK(mDestBuffer != NULL);
	}

	if(mRotate == 90 || mRotate == 180 || mRotate == 270) {
		//we need rotate.
		mRotateBuffer= (uint8_t *)malloc(frameSize);
		CHECK(mRotateBuffer != NULL);
	}

	bool showFrame = true;//render first frame.
    SDL_PumpEvents();
    while (!mStop ) {
    	waitEvent(&event);
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
            case SDLK_q:
            	mStop = true;
                break;
            case SDLK_n:
            	showFrame = true;
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEMOTION:
            break;
        case SDL_QUIT:
        	mStop = true;
        	break;
        default:
            break;
        }
     	int64_t nowUs = getNowUs();
		if(mLastFrameTimeUs == -1 || (nowUs - mLastFrameTimeUs) >= 5000000LL) {
			//showFrame = true;
		}
        if(showFrame) {
        	mStop = showYUVFrame();
        	showFrame = false;
        }
        SDL_PumpEvents();
    }
	if(mDestBuffer) {
		free(mDestBuffer);
		mDestBuffer = NULL;
	}
	if(mRotateBuffer) {
		free(mRotateBuffer);
		mRotateBuffer = NULL;
	}
	if(mSourceBuffer) {
		free(mSourceBuffer);
		mSourceBuffer = NULL;
	}
    return 0;
}

int RawPlayer::showBitmap(const char *path) {

	ALOGV("showBitMap");

    return 0;
}

int RawPlayer::reset() {
	ALOGV("reset");

	if(mUri) {
		free(mUri);
		mUri = NULL;
	}

    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = NULL;
    }
    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = NULL;
    }
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
    }
	if(mFd) {
		close(mFd);
		mFd = -1;
	}

	if(mWaterMark) {
		releaseWaterMark(mWaterMark);
		mWaterMark = NULL;
	}
	return 0;
}

