#define LOG_NDEBUG 0
#define LOG_TAG "BoardTest"
#include <utils/Log.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/ctrl/edit.h>
#include <assert.h>
#include <BoardTest.h>
#define ID_TIMER_KEY 0x100

static int boardTestProcWrapper(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
	BoardTest *boardTest = (BoardTest *)(GetWindowAdditionalData(hWnd));
	return boardTest->boardTestProc(hWnd, message, wParam, lParam);
}

static void drawTrapezoid(HDC hdc) {
	struct AdasPoint {
		int x, y;
	};
	AdasPoint leftStart, leftEnd;
	AdasPoint rightStart, rightEnd;

	leftStart.x = 110;
	leftStart.y = 70;
	leftEnd.x = 60;
	leftEnd.y = 170;

	rightStart.x = 210;
	rightStart.y = 70;
	rightEnd.x = 260;
	rightEnd.y = 170;

}

int BoardTest::test(HDC hdc) {
	//distance
#if 0
	char text[4]= {0};

	int x = 100;
	int y = 50;
	int w = 120;
	int h = 100;
	int len = w/6;
	int distance = 10;

	SetPenWidth(hdc, 5);
	SetPenColor(hdc, RGBA2Pixel(hdc, 0x00, 0xFF, 0x00, 0xff));

	//first half on top line
	int x0 = x;
	int y0 = y;
	int x1 = x + len;
	int y1 = y;
	LineEx(hdc, x0, y0, x1, y1);

	//first half on left line
	x0 = x;
	y0 = y;
	x1 = x;
	y1 = y + len;
	LineEx(hdc, x0, y0, x1, y1);

	//second half on left line
	x0 = x + w - len;
	y0 = y;
	x1 = x + w;
	y1 = y;
	LineEx(hdc, x0, y0, x1, y1);

	//first half on right line
	x0 = x + w;
	y0 = y;
	x1 = x + w;
	y1 = y + len;
	LineEx(hdc, x0, y0, x1, y1);

	//second half on bottom line
	x0 = x + w - len;
	y0 = y + h;
	x1 = x + w;
	y1 = y + h;
	LineEx(hdc, x0, y0, x1, y1);

	//second half on bottom line
	x0 = x + w;
	y0 = y + h - len;
	x1 = x + w;
	y1 = y + h;
	LineEx(hdc, x0, y0, x1, y1);

	//first half on bottom line
	x0 = x;
	y0 = y + h;
	x1 = x + len;
	y1 = y + h;
	LineEx(hdc, x0, y0, x1, y1);

	//sencond half on left line
	x0 = x;
	y0 = y + h - len;
	x1 = x;
	y1 = y + h;
	LineEx(hdc, x0, y0, x1, y1);

	RECT rect;
	rect.left = x + len;
	rect.top = y - w - 2 * len;
	rect.right = x + w - len;
	rect.bottom = y;
	if(rect.top < 0)
		rect.top = 0;

	SetTextColor(hdc, RGBA2Pixel(hdc, 0xFF, 0x00, 0x00, 0x00));
	snprintf(text, 4, "%dM", distance);
	DrawText(hdc, text, -1, &rect, DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	return 0;
#else
	struct AdasPoint {
		int x, y;
	};
	AdasPoint leftStart, leftEnd;
	AdasPoint rightStart, rightEnd;

	leftStart.x = 110;
	leftStart.y = 70;
	leftEnd.x = 60;
	leftEnd.y = 170;

	rightStart.x = 210;
	rightStart.y = 70;
	rightEnd.x = 260;
	rightEnd.y = 170;

	int length = leftEnd.y - leftStart.y;
	int lines = 7;
	//six lines and five intervals
	int total = 0;
	for(int i = 1; i < lines; i ++) {
		total += i * 2;
	}
	total += lines;

	int factor = length / total;

	int xDelta = (rightEnd.x - leftEnd.x - rightStart.x + leftStart.x)/2;
	assert(xDelta == 50);
	xDelta /= lines;

	gal_pixel old_color = SetBrushColor (hdc, RGBA2Pixel(hdc, 0x00, 0xFF, 0x00, 0xff));
	int x = leftStart.x + (rightStart.x - leftStart.x)/3;
	int y = leftStart.y;
	int w = (rightStart.x - leftStart.x)/3;
	int h = 5;
	while(y + h <= rightEnd.y) {
		ALOGV("fill box(%d, %d, %d, %d)", x, y, w, h);
		FillBox(hdc, x, y, w, h);
		x -= 5;
		y += h + 5;
		w += 5*2;
		h += 5;
	}

	ALOGV("fill box(%d, %d, %d, %d)", x, y, w, h);
	SetBrushColor(hdc, old_color);
	return 0;
#endif
}

int BoardTest::boardTestProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	switch(message) {
	case MSG_CREATE:
		ALOGV("MSG_CREATE");
		//SetTimer(hWnd, ID_TIMER_KEY, 100);
		break;
	case MSG_PAINT:
		hdc = BeginPaint (hWnd);
		ALOGV("MSG_PAINT");
		test(hdc);
		EndPaint (hWnd, hdc);
		break;
	case MSG_KEYDOWN:
		break;
	case MSG_KEYUP:
		break;
	case MSG_KEYLONGPRESS:
		break;
	case MSG_TIMER:
		break;
	case MSG_DESTROY:
		break;
	case MSG_CLOSE:
		ALOGV("MSG_CLOSE");
		KillTimer(hWnd, ID_TIMER_KEY);
		DestroyMainWindow (hWnd);
		PostQuitMessage (hWnd);
		break;
	default:
		break;
	}
	return DefaultWindowProc(hWnd, message, wParam, lParam);
}

BoardTest::BoardTest()
	:mMainHandle(HWND_INVALID) {
	ALOGV("BoardTest");
}

BoardTest::~BoardTest() {
	ALOGV("~BoardTest");
}

int BoardTest::init() {
	ALOGV("init");
	// get screen size
	mScreenWidth = GetGDCapability(HDC_SCREEN, GDCAP_MAXX) + 1;
	mScreenHeight = GetGDCapability(HDC_SCREEN, GDCAP_MAXY) + 1;
	ALOGV("screen size (%d, %d)\n", mScreenWidth, mScreenHeight);

	MAINWINCREATE CreateInfo;
	CreateInfo.dwStyle = WS_VISIBLE | WS_CAPTION | WS_BORDER;
	CreateInfo.dwExStyle = WS_EX_NONE | WS_EX_AUTOSECONDARYDC | WS_EX_NOCLOSEBOX;
	CreateInfo.spCaption = "BoardTest";
	CreateInfo.hMenu = 0;
	CreateInfo.hCursor = GetSystemCursor(0);
	CreateInfo.hIcon = 0;
	CreateInfo.hHosting = HWND_DESKTOP;
	CreateInfo.MainWindowProc = boardTestProcWrapper;

	CreateInfo.lx = 0;
	CreateInfo.ty = 0;
	CreateInfo.rx = mScreenWidth;
	CreateInfo.by = mScreenHeight;

	CreateInfo.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xff, 0xff, 0xff, 0xff);
	CreateInfo.dwAddData = (DWORD)this;
	mMainHandle = CreateMainWindow(&CreateInfo);
	if (mMainHandle == HWND_INVALID) {
		ALOGE("CreateMainWindow error");
		return -1;
	}

	ShowWindow (mMainHandle, SW_SHOWNORMAL);
	return 0;
}

int BoardTest::reset() {
	if(mMainHandle) {
		MainWindowCleanup(mMainHandle);
		mMainHandle = HWND_INVALID;
	}
	return 0;
}

int BoardTest::loop() {
	MSG Msg;
	while (GetMessage(&Msg, mMainHandle)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}

int MiniGUIMain(int argc, const char **argv)
{
	BoardTest * boardTest = new BoardTest();
	boardTest->init();
	boardTest->loop();
	boardTest->reset();
	delete boardTest;
	return 0;
}

#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif
