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
	 mScreen(NULL),
	 mOverlay(NULL),
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

	mWidth = width;
	mHeight = height;
	mFormat = format;
	mRotate = rotate;
	mTimeWaterMark = timeWatermark;

	if(mRotate == 90 || mRotate == 270) {
		//switch width and height
		width = height - width;
		height = height - width;
		width = height + width;
	}

	if(mWidth > MAX_WINDOW_WIDTH || mHeight > MAX_WINDOW_HEIGHT) {
		mDisplayWidth = MAX_WINDOW_WIDTH;
		mDisplayHeight = MAX_WINDOW_HEIGHT;
	} else {
		mDisplayWidth = mWidth;
		mDisplayHeight = mHeight;
	}
	ALOGD("display size(%d, %d)", mDisplayWidth, mDisplayHeight);
	mScreen = SDL_SetVideoMode(mDisplayWidth, mDisplayHeight, 0, 0);
	ALOGI("screen creat success");
	if (mScreen == NULL) {
		ALOGE("create surface error!");
		return -1;
	}
	mOverlay = SDL_CreateYUVOverlay(width, height, SDL_YV12_OVERLAY, mScreen);
	ALOGI("overlay creat success");
	if (mOverlay == NULL) {
		ALOGE("create overlay error!");
		return -1;
	}
	const char *watermark = "108,48,1";
	int rotation = 0;
	int watermarkHeight = 540;
	if(rotate == 90 || rotate == 270) {
		if(height < 540) {
			watermarkHeight = 320;
		}
	} else {
		if(width < 540) {
			watermarkHeight = 320;
		}
	}
	mWaterMark = initialwaterMark(540, 270);
	waterMarkSetMultiple(mWaterMark, watermark);
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

	int YSize = mWidth * mHeight;
	int USize = YSize/4;//VSize is the same.
	int frameSize = getFrameSize(mFormat, mWidth, mHeight);
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
		BitmapParams source(mSourceBuffer, mFormat, mWidth, mHeight, 0, 0, 0, 0);
		BitmapParams yuv(mDestBuffer, COLOR_FORMAT_YVU420_PLANNAR, mWidth, mHeight, 0, 0, 0, 0);
		err = colorConvert(&source, &yuv);
		ALOGV("convert %d", err);
		yv12 = mDestBuffer;
	} else {
		yv12 = mSourceBuffer;
	}
	//next: rotate
	if(mRotateBuffer) {
		//It's YV12 now.
		BitmapParams source(yv12, COLOR_FORMAT_YVU420_PLANNAR, mWidth, mHeight, 0, 0, 0, 0);
		BitmapParams rotated(mRotateBuffer, COLOR_FORMAT_YVU420_PLANNAR, mHeight, mWidth, 0, 0, 0, 0);
		err = pictureRotate(&source, &rotated, mRotate);
		yv12 = mRotateBuffer;
	}
	if(mTimeWaterMark) {
		setWarterMark(yv12, mWidth, mHeight);
	}

	SDL_LockSurface(mScreen);
	SDL_LockYUVOverlay(mOverlay);
	memcpy(mOverlay->pixels[0], yv12, YSize);

	//we created an overlay with format YV12
	if(mFormat == COLOR_FORMAT_YUV420_PLANNAR) {
		memcpy(mOverlay->pixels[2], yv12 + YSize, USize);
		memcpy(mOverlay->pixels[1], yv12 + YSize + USize, USize);
	} else {
		//others converted to YV12
		memcpy(mOverlay->pixels[1], yv12 + YSize, USize);
		memcpy(mOverlay->pixels[2], yv12 + YSize + USize, USize);
	}

	SDL_Rect rectSrc;
	if(mRotate == 90 || mRotate == 270) {
		rectSrc.w = mDisplayHeight;
		rectSrc.h = mDisplayWidth;
	} else {
		rectSrc.w = mDisplayWidth;
		rectSrc.h = mDisplayHeight;
	}
	rectSrc.x = 0;
	rectSrc.y = 0;

	SDL_UnlockYUVOverlay(mOverlay);
	SDL_UnlockSurface(mScreen);
	SDL_DisplayYUVOverlay(mOverlay, &rectSrc);

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
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_ALLEVENTS)) {
        SDL_PumpEvents();
    }
}

/* handle an event sent by the GUI */
int RawPlayer::display() {
    SDL_Event event;
    ALOGV("displayYUV");
	int frameSize = getFrameSize(mFormat, mWidth, mHeight);
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
        case SDL_VIDEOEXPOSE:
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEMOTION:
            break;
        case SDL_VIDEORESIZE:
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

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return -1;
	}

	atexit(SDL_Quit);

	SDL_Surface *screen;

	int width,height;
	int fd = open(path, O_RDONLY);
	if(fd < 0) {
		ALOGE("open file %s failed %s", path, strerror(errno));
		SDL_Quit();
		return -1;

	}
	lseek(fd, 18, SEEK_SET);
	read(fd, &width, 4);
	read(fd, &height, 4);
	width = abs(width);
	height = abs(height);
	close(fd);

	screen = SDL_SetVideoMode(width, height, 16, SDL_DOUBLEBUF);
	if (screen == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Surface *image = SDL_LoadBMP(path);
	if (image == NULL) {
		printf("Unable to load bitmap: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Rect src, dest;

	src.x = 0;
	src.y = 0;
	src.w = image->w;
	src.h = image->h;

	dest.x = 0;
	dest.y = 0;
	dest.w = image->w;
	dest.h = image->h;

	SDL_BlitSurface(image, &src, screen, &dest);

	SDL_Flip(screen);

	SDL_Delay(5000);
	SDL_FreeSurface(image);
    SDL_Quit();
    return 0;
}

int RawPlayer::reset() {
	ALOGV("reset");

	if(mUri) {
		free(mUri);
		mUri = NULL;
	}

	if (mOverlay) {
		SDL_FreeYUVOverlay(mOverlay);
		mOverlay = NULL;
	}

	if (mScreen) {
		SDL_FreeSurface(mScreen);
		mScreen = NULL;
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

