#define LOG_NDEBUG 0
#define LOG_TAG "SubWindow"
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

#define WINDOW_SUB_WINDOW		"SubWindow"

int SubWindowProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	SubWindow* stb = (SubWindow*) GetWindowAdditionalData(hWnd);
	switch (message) {
	case MSG_PAINT:
	{
		ALOGV("MSG_PAINT");
		HDC hdc = BeginPaint (hWnd);
	//	drawImage(hdc);
		//drawLine(hdc);
		int c = GetWindowBkColor(hWnd);
		ALOGD("background color 0x%x", c);
		gal_pixel color;
		if(0xD02B2B2B == c) {
			color = 0xFFff0000;
		} else {
			color = 0xD02B2B2B;
		}
		RECT rect;
		GetClientRect(hWnd, &rect);
		ALOGV("client (%d, %d, %d, %d)", rect.left, rect.top, RECTW(rect), RECTH(rect));

		SetWindowBkColor(hWnd, color);

		SetPenColor(hdc, RGBA2Pixel(hdc, 0xff, 0x00, 0x00, 0xff));
		LineEx(hdc, 0, 20, 960, 20);
		EndPaint (hWnd, hdc);
	}
		break;
	case MSG_SHOWWINDOW:
		ALOGV("MSG_SHOWWINDOW");
		break;
	case MSG_FONTCHANGED:
		break;
	case MSG_CREATE:
	{
		ALOGV("MSG_CREATE");
		gal_pixel color = 0xd02b2b2b;
		SetWindowBkColor(hWnd, color);
		break;
	}
	case MSG_TIMER:
		break;
	default:
		break;
	}
	return DefaultWindowProc(hWnd, message, wParam, lParam);
}

SubWindow::SubWindow(HWND parant) {
	RegisterSubWindows();
	ALOGV("SubWindow");
	RECT rect;
	GetWindowRect(parant, &rect);
	ALOGV("parent (%d, %d, %d, %d)", rect.left, rect.top, RECTW(rect), RECTH(rect));
	rect.left = 0;
	rect.top = 0;
	rect.right = 960;
	rect.bottom = 43;
	ALOGV("sub window (%d, %d, %d, %d)", rect.left, rect.top, RECTW(rect), RECTH(rect));

	mHwnd = CreateWindowEx(WINDOW_SUB_WINDOW, "",
	                       WS_VISIBLE | WS_CHILD,
	                       WS_EX_NONE | WS_EX_USEPARENTFONT,
	                       4,
	                       rect.left, rect.top, RECTW(rect), RECTH(rect),
	                       parant, (DWORD)this);

	if(mHwnd == HWND_INVALID) {
		ALOGE("create subwindow failed\n");
		return;
	}

//	gal_pixel color = 0xd02b2b2b;
	gal_pixel color = 0xd02b2b2b;
	SetWindowBkColor(mHwnd, color);
}

SubWindow::~SubWindow() {
	ALOGV("~SubWindow");
	UnRegisterSubWindows();
}


int RegisterSubWindows(void)
{
	WNDCLASS WndClass = {
		NULL,
		0,
		WS_CHILD | WS_VISIBLE,
		WS_EX_USEPARENTFONT,
		GetSystemCursor(0),
		COLOR_lightgray,
		NULL,
		0
	};

	WndClass.spClassName = WINDOW_SUB_WINDOW;
	WndClass.WinProc = SubWindowProc;
	WndClass.iBkColor = RGBA2Pixel(HDC_SCREEN, 0xFF, 0x00, 0x00, 0x00);

	if (RegisterWindowClass(&WndClass) == FALSE) {
		ALOGE("register %s failed\n", WINDOW_SUB_WINDOW);
		return -1;
	}
	return 0;
}

void UnRegisterSubWindows(void)
{
	UnregisterWindowClass(WINDOW_SUB_WINDOW);
}
