#define LOG_NDEBUG 0
#define LOG_TAG "ScreenTest"
#include <utils/Log.h>

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#ifdef ROP_XOR
#undef ROP_XOR
#endif
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <sunxi_display2.h>
#include <SubWindow.h>

#pragma pack(push)
#pragma pack(2)
struct BitMapFileHeader {
	uint16_t mType;
	uint32_t mSize;
	uint16_t mReserved1;
	uint16_t mReserved2;
	uint32_t mOffBits;
};

struct BitMapInfoHeader {
	uint32_t mSize;
	uint32_t mWidth;
	uint32_t mHeight;
	uint16_t mPlanes;
	uint16_t mBitCount;
	uint32_t mCompression;
	uint32_t mSizeImage;
	uint32_t mXPelsPerMeter;
	uint32_t mYPelsPerMeter;
	uint32_t mClrUsed;
	uint32_t mClrImportant;
};
#pragma pack(pop)

class ScreenTest {
public:
	ScreenTest();
	~ScreenTest();
	int init(int screenWidth, int screenHeight, const char *path);
	int draw();
	int drawLine(HDC hdc);
	int drawImage(HDC hdc);
	int reset();
	void loop();
	void closeLayer();
	void timer();
private:
	BITMAP mBgImage;
	int mScreeWidth;
	int mScreenHeight;
	BitMapFileHeader mFileHeader;
	BitMapInfoHeader mInfoHeader;
	uint8_t *mData;
	HWND mHandle;
	int mDisplayFd;
	bool mClosed;
	SubWindow *mSubWindow;
};

static int HelloWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
	ScreenTest *screen = (ScreenTest*)GetWindowAdditionalData(hWnd);
	ALOGV("message 0x%x, wParam 0x%x, 0x%lx\n", message, wParam, lParam);
	switch (message) {
	case MSG_PAINT:
		ALOGV("MSG_PAINT");
		screen->draw();
		break;
	case MSG_CREATE:
		break;
	case MSG_TIMER:
		screen->timer();
		break;
	case MSG_KEYDOWN:
		break;
	case MSG_KEYUP:
		screen->closeLayer();
		break;
	case MSG_SHOWWINDOW:
		ALOGV("MSG_SHOWWINDOW");
		break;
	case MSG_CLOSE:
		DestroyMainWindow(hWnd);
		PostQuitMessage(hWnd);
		return 0;

	}
	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

ScreenTest::ScreenTest() {

}

ScreenTest::~ScreenTest() {

}

void ScreenTest::closeLayer() {
	ALOGV("closeLayer start");
	disp_layer_config layer;
	layer.channel = 2;
	layer.layer_id = 0;
	unsigned long args[4] = { 0 };
	uint32_t ret = 0;

	args[0] = 0;
	args[1] = (unsigned long) &layer;
	args[2] = 1;
	ret = ioctl(mDisplayFd, DISP_LAYER_GET_CONFIG, args);
	if(mClosed) {
		layer.enable = 1;
	} else {
		layer.enable = 0;
	}
	mClosed = !mClosed;
	args[0] = 0;
	args[1] = (unsigned long) &layer;
	args[2] = 1;
	ret = ioctl(mDisplayFd, DISP_LAYER_SET_CONFIG, args);
	ALOGV("closeLayer finished");
	sleep(1);
	UpdateWindow(mHandle, TRUE);
}

int ScreenTest::drawLine(HDC hdc) {
	SetPenColor(hdc, RGBA2Pixel(hdc, 0xff, 0xff, 0xff, 0xff));
	SetPenWidth(hdc,10);
	LineEx(hdc, 0, 0, 0,  mScreenHeight);
	LineEx(hdc, mScreeWidth/2, 0, mScreeWidth/2, mScreenHeight);
	LineEx(hdc, mScreeWidth, 0, mScreeWidth, mScreenHeight);
	return 0;
}

void dumpBITMAP(BITMAP *bitmap) {
	ALOGD("bmType:%d, bmBitsPerPixel:%d, bmBytesPerPixel:%d, bmAlpha:%d, bmColorKey:%d, "
			"bmWidth:%d, bmHeight:%d, bmPitch:%d, bmBits:%p, bmAlphaMask:%p, bmAlphaPitch:%d",
			bitmap->bmType, bitmap->bmBitsPerPixel, bitmap->bmBytesPerPixel, bitmap->bmAlpha,
			bitmap->bmColorKey, bitmap->bmWidth, bitmap->bmHeight, bitmap->bmPitch,
			bitmap->bmBits, bitmap->bmAlphaMask, bitmap->bmAlphaPitch);
}

int ScreenTest::drawImage(HDC hdc) {
#if TEST_BITMAP
	dumpBITMAP(&mBgImage);
	mBgImage.bmType = BMP_TYPE_NORMAL;
	ALOGD("%u, %u, %u, %u", mFileHeader.mSize, mFileHeader.mOffBits, mInfoHeader.mWidth, mInfoHeader.mHeight);
	mBgImage.bmBytesPerPixel = (mFileHeader.mSize - mFileHeader.mOffBits)/(mInfoHeader.mWidth * mInfoHeader.mHeight);
	mBgImage.bmBitsPerPixel = mBgImage.bmBytesPerPixel * 8;
	mBgImage.bmAlpha = 0;
	mBgImage.bmColorKey = 0;
	mBgImage.bmWidth = mInfoHeader.mWidth;
	mBgImage.bmHeight = mInfoHeader.mHeight;
	mBgImage.bmPitch = mBgImage.bmWidth * mBgImage.bmBytesPerPixel;
	mBgImage.bmBits = mData;
	mBgImage.bmAlphaMask = 0;
	mBgImage.bmAlphaPitch = 0;
	dumpBITMAP(&mBgImage);
#endif
	//FillBoxWithBitmap(hdc, 0, 0, mScreeWidth, mScreenHeight, &mBgImage);
	//Rectangle (hdc, 0, 0, mScreeWidth, mScreenHeight);
	return 0;
}

void ScreenTest::timer() {
	RECT r;
	GetWindowRect(mSubWindow->getHandle(), &r);
//	InvalidateRect(mSubWindow->getHandle(), &r, true);
//	UpdateWindow(mSubWindow->getHandle(), true);
	//UpdateWindow(mHandle, true);
}

int ScreenTest::draw() {
	ALOGV("draw");
	HDC hdc = BeginPaint (mHandle);
//	drawImage(hdc);
	//drawLine(hdc);
	gal_pixel color = 0xff2B2B2B;
	SetWindowBkColor(mHandle, color);


	color = 0xFFff0000;
	SetWindowElementAttr(mHandle, WE_FGC_WINDOW, Pixel2DWORD(hdc, color));
	EndPaint (mHandle, hdc);
	return 0;
}

int ScreenTest::init(int screenWidth, int screenHeight, const char *path) {
	mScreeWidth = screenWidth;
	mScreenHeight = screenHeight;
	system("dd if=/dev/zero of=dev/graphics/fb0");

	MAINWINCREATE CreateInfo;
	CreateInfo.dwStyle = WS_VISIBLE;
	CreateInfo.dwExStyle = WS_EX_NONE | WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT;
	CreateInfo.spCaption = "";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.hIcon = 0;
	CreateInfo.MainWindowProc = HelloWinProc;
	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = mScreeWidth;
	CreateInfo.by = mScreenHeight;
	CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xff, 0xff, 0xff, 0xff);
	ALOGD("PIXEL_lightwhite 0x%x", PIXEL_lightwhite);
	CreateInfo.dwAddData = (DWORD)this;
	CreateInfo.hHosting = HWND_DESKTOP;
	mHandle = CreateMainWindow(&CreateInfo);

	if (mHandle == HWND_INVALID)
		return -1;

//	if (LoadBitmap(HDC_SCREEN, &mBgImage, path))
//		return -1;
#if TEST_BITMAP
	int fd = open(path, O_RDONLY);
	if(fd >= 0) {
		read(fd, &mFileHeader, sizeof(mFileHeader));
		read(fd, &mInfoHeader, sizeof(mInfoHeader));
		ALOGD("size:0x%x, offset:0x%x", mFileHeader.mSize, mFileHeader.mOffBits);
		mData = new uint8_t[mFileHeader.mSize - mFileHeader.mOffBits];
		read(fd, mData, mFileHeader.mSize - mFileHeader.mOffBits);
		close(fd);
	}
#endif
	ALOGV("create subwindow");
	ShowWindow(mHandle, SW_SHOWNORMAL);
	UpdateWindow(mHandle, true);
	ALOGV("create subwindow");
	mSubWindow = new SubWindow(mHandle);
	ALOGV("show subwindow");
	ShowWindow(mSubWindow->getHandle(), SW_SHOWNORMAL);


#define _ID_TIMER 100
	SetTimer (mHandle, _ID_TIMER, 100);
	mDisplayFd = ::open("/dev/disp", O_RDWR);
	if(mDisplayFd < 0) {
		return -1;
	}
	mClosed = false;
	return 0;
}

void ScreenTest::loop() {
	MSG Msg;
	while (GetMessage(&Msg, mHandle)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

int ScreenTest::reset() {
	UnloadBitmap(&mBgImage);
	MainWindowThreadCleanup(mHandle);
	if(mDisplayFd >= 0) {
		close(mDisplayFd);
		mDisplayFd = -1;
	}
	return 0;
}

static void getScreenSize(int *x, int *y, int *w, int *h) {
	int fd = open("/dev/graphics/fb0", O_RDWR);
	struct fb_var_screeninfo var;

	ioctl(fd, FBIOGET_VSCREENINFO, &var);
	*x = 0;
	*y = 0;
	*w = var.xres;
	*h = var.yres;

	close(fd);
}

int MiniGUIMain(int argc, const char* argv[]) {
	ScreenTest *screen = new ScreenTest();
	int w,h,x,y;
	int opt;
	if (argc != 3 || strncmp(argv[1], "-f", 2) || argv[2] == NULL) {
		printf("Usage:%s -f file\n", argv[0]);
//		exit(0);
//		return 0;
	}
	const char *file = argv[2];
	getScreenSize(&x, &y, &w, &h);
	screen->init(w, h, file);
	screen->loop();
	screen->reset();
	delete screen;
	return 0;
}

#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif

