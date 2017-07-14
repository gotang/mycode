#define LOG_NDEBUG 0
#define LOG_TAG "DisplayTest"
#include <utils/Log.h>

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>

#include <hwdisplay.h>
#include <memoryAdapter.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define BUFFER_COUNT 2

using namespace android;
struct RenderBuffer {
	void *mVirtAddr;
	void *mPhyAddr;
};

class DisplayBase {
public:
	DisplayBase();
	virtual ~DisplayBase() {}
	virtual int init(const char *path, int width, int height, int format);
	virtual int loop() = 0;
	virtual int reset();
protected:
	struct view_info mViewInfo;
	size_t mMemorySize;
	int mIndex;
	int mFormat;
	int mWidth;
	int mHeight;
	RenderBuffer mBuffer[BUFFER_COUNT];
};

class DirectDisplay :public DisplayBase{
public:
	DirectDisplay();
	~DirectDisplay();
	virtual int init(const char *path, int width, int height, int format);
	virtual int loop();
	virtual int reset();
private:
	void layerInit();
	void layerUpdate();
	void layerClose();
	void layerOpen();
	//call display driver;
	int layerConfig(int cmd, void *pinfo);
	int layerSetParameter(void *pinfo);
	int layerGetParameter(void *pinfo);
	int mDisplayFd;
};

class SystemDisplay :public DisplayBase {
public:
	SystemDisplay();
	~SystemDisplay();
	virtual int init(const char *path, int width, int height, int format);
	virtual int loop();
	virtual int reset();
private:
	HwDisplay *mDisplay;
	int mLayerId;
};

DisplayBase::DisplayBase()
	:mMemorySize(0),
	 mIndex(0) {

}

int DisplayBase::init(const char *path, int width, int height, int format) {
	ALOGV("DisplayBase::init");
	MemAdapterOpen();
	if(format == 0) {
		mMemorySize = width * height * 4;//AGRB8888
	} else {
		mMemorySize = width * height * 3/2;//YUV420
	}
	int fd = open(path, O_RDONLY);
	for(int i = 0; i < BUFFER_COUNT; i ++) {
		mBuffer[i].mVirtAddr = (unsigned long *) MemAdapterPalloc(mMemorySize);
		ssize_t bytes = read(fd, mBuffer[i].mVirtAddr, mMemorySize);
		assert(bytes == mMemorySize);
		mBuffer[i].mPhyAddr = (unsigned long *) MemAdapterGetPhysicAddressCpu((void*) mBuffer[i].mVirtAddr);
		ALOGV("buffer[%d], virt %p, phy %p", i, mBuffer[i].mVirtAddr, mBuffer[i].mPhyAddr);
	}
	close(fd);
	mViewInfo.x = 0;
	mViewInfo.y = 0;
	mViewInfo.w = SCREEN_WIDTH - mViewInfo.x;
	mViewInfo.h = SCREEN_HEIGHT - mViewInfo.y;
	mFormat = format;
	return 0;
}

int DisplayBase::reset() {
	ALOGV("DisplayBase::reset");
	for(int i = 0; i < BUFFER_COUNT; i ++) {
		if(mBuffer[i].mVirtAddr) {
			MemAdapterPfree(mBuffer[i].mVirtAddr);
			mBuffer[i].mVirtAddr = NULL;
		}
	}
	MemAdapterClose();
	return 0;
}

SystemDisplay::SystemDisplay ()
	:mDisplay(NULL),
	 mLayerId(0) {

}

SystemDisplay::~SystemDisplay () {

}

int SystemDisplay::init (const char *path, int width, int height, int format) {
	DisplayBase::init(path, width, height, format);
	mDisplay = HwDisplay::getInstance();
	mDisplay->hwd_layer_open(8);
	mDisplay->hwd_layer_close(8);
	mLayerId = mDisplay->hwd_layer_request(&mViewInfo);
	struct src_info src;
	src.w = width;
	src.h = height;
	src.crop_x = 0;
	src.crop_y = 0;
	if(mFormat == 0) {
		src.format = HWC_FORMAT_RGBA_8888;
		src.crop_w = mViewInfo.w;
		src.crop_h = mViewInfo.h;
	} else {
		src.format = HWC_FORMAT_YUV420VUC;
		src.crop_w = width;
		src.crop_h = height;
	}
	mWidth = width;
	mHeight = height;
	mDisplay->hwd_layer_set_src(mLayerId, &src);
	return 0;
}

int SystemDisplay::reset () {
	DisplayBase::reset();
	if(mDisplay) {
		mDisplay->hwd_layer_close(mLayerId);
		mDisplay->hwd_layer_release(mLayerId);
		mDisplay = NULL;
	}
	return 0;
}

int SystemDisplay::loop () {

	libhwclayerpara_t overlay_para;
	int count = 0;
	while(count ++ < 60 * 30) {
		RenderBuffer *buffer = &mBuffer[mIndex++];
		mIndex = mIndex % BUFFER_COUNT;
		MemAdapterFlushCache(buffer->mVirtAddr, mMemorySize);

		memset(&overlay_para, 0, sizeof(libhwclayerpara_t));
		overlay_para.top_y = (unsigned long)buffer->mPhyAddr;
		overlay_para.top_c = (unsigned long)buffer->mPhyAddr + mWidth * mHeight;
		mDisplay->hwd_layer_render(mLayerId, &overlay_para);
		if(count == 1) {
			mDisplay->hwd_layer_open(mLayerId);
		}
		usleep(30*1000);
	};

	return 0;
}

DirectDisplay::DirectDisplay ()
	:mDisplayFd(-1) {

}

DirectDisplay::~DirectDisplay () {

}

int DirectDisplay::init(const char *path, int width, int height, int format) {
	DisplayBase::init(path, width, height, format);
	mDisplayFd = ::open("/dev/disp", O_RDWR);

	disp_layer_config layer, ui_config;

	ui_config.channel = 2;
	ui_config.layer_id = 0;
	layerGetParameter(&ui_config); //get ui para
	memcpy(&layer, &ui_config, sizeof(layer));

	layer.channel = 2;
	layer.layer_id = 1;
	layer.enable = 1;
	layer.info.zorder = 11;

	layer.info.fb.addr[0] = (long long unsigned int) mBuffer[0].mPhyAddr;
	layer.info.screen_win.x = mViewInfo.x;
	layer.info.screen_win.y = mViewInfo.y;
	layer.info.screen_win.width = mViewInfo.w;
	layer.info.screen_win.height = mViewInfo.h;

	layer.info.fb.crop.x = 0;
	layer.info.fb.crop.y = 0;
	layer.info.fb.crop.width = mViewInfo.w;
	layer.info.fb.crop.height = mViewInfo.h;

	layer.info.fb.crop.x <<= 32;
	layer.info.fb.crop.y <<= 32;
	layer.info.fb.crop.width <<= 32;
	layer.info.fb.crop.height <<= 32;

	layer.info.fb.size[0].width = mViewInfo.w;
	layer.info.fb.size[0].height = mViewInfo.h;

	ui_config.info.zorder = 10;
	layerSetParameter(&ui_config);
	layerSetParameter(&layer);

	layerOpen();
	return 0;
}

int DirectDisplay::reset() {
	layerClose();
	if(mDisplayFd >= 0) {
		::close(mDisplayFd);
		mDisplayFd = -1;
	}

	DisplayBase::reset();
	return 0;
}

int DirectDisplay::loop() {
	int count = 0;
	while(count ++ < 60 * 30) {
		layerUpdate();
		usleep(30*1000);
	};
	return 0;
}

int DirectDisplay::layerConfig(int cmd, void *pinfo) {
	unsigned long args[4] = { 0 };
	uint32_t ret = 0;

	args[0] = 0;
	args[1] = (unsigned long) pinfo;
	args[2] = 1;
	ret = ioctl(mDisplayFd, (__DISP_t ) cmd, args);
	return ret;
}

int DirectDisplay::layerSetParameter(void *pinfo) {
	return layerConfig(DISP_LAYER_SET_CONFIG, pinfo);
}

int DirectDisplay::layerGetParameter(void *pinfo) {
	return layerConfig(DISP_LAYER_GET_CONFIG, pinfo);
}

void DirectDisplay::layerOpen() {
	ALOGV("layerOpen");
	disp_layer_config carLayerConfig;
	carLayerConfig.channel = 2;
	carLayerConfig.layer_id = 1;
	layerGetParameter(&carLayerConfig);
	carLayerConfig.enable = 1;
	layerSetParameter(&carLayerConfig);
}

void DirectDisplay::layerClose() {
	ALOGV("layerClose");
	disp_layer_config carLayerConfig;
	carLayerConfig.channel = 2;
	carLayerConfig.layer_id = 1;
	layerGetParameter(&carLayerConfig);
	carLayerConfig.enable = 0;
	layerSetParameter(&carLayerConfig);
}

void DirectDisplay::layerUpdate() {
	//ALOGV("layerUpdate");
	RenderBuffer *buffer = &mBuffer[mIndex++];
	mIndex = mIndex % BUFFER_COUNT;
	MemAdapterFlushCache(buffer->mVirtAddr, mMemorySize);
	disp_layer_config carLayerConfig;
	carLayerConfig.channel = 2;
	carLayerConfig.layer_id = 1;
	layerGetParameter(&carLayerConfig);
	carLayerConfig.info.fb.addr[0] = (uint64_t) buffer->mPhyAddr;
	layerSetParameter(&carLayerConfig);

}
/////////////////////////////////////////////////////////////////
static void printUsage(const char *app) {
	printf("Usage: %s [OPTION]\n"
			" -p     raw data file path\n"
			" -w     width\n"
			" -h     height\n"
			" -f     format\n"
			" -r     rotate degree\n"
			, app);
}

int main(int argc, char *argv[]) {
	int opt;
	int width = 0;
	int height = 0;
	int format = 0;
	int rotate = 0;
	const char *path = NULL;
	while ((opt = getopt(argc, argv, "p:w:h:f:r:")) != -1) {
		switch (opt) {
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
			height = atoi(optarg);
			break;
		case 'p':
			path = strdup(optarg);
			break;
		case 'f':
			format = atoi(optarg);
			break;
		case 'r':
			rotate = atoi(optarg);
			break;
		default:
			printUsage (argv[0]);
			exit(0);
		}
	}
	if(path == NULL) {
		printUsage (argv[0]);
		exit(0);
	}
	DisplayBase *display = new SystemDisplay();
	display->init(path, width, height, format);
	display->loop();
	display->reset();
	delete display;
	return 0;
}
