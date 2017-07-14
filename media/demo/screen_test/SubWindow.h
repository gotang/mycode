
#ifndef SUB_WINDOW_H_
#define SUB_WINDOW_H_

class SubWindow {
public:
	SubWindow(HWND parant);
	~SubWindow();
	HWND getHandle() {
		return mHwnd;
	}
private:
	HWND mHwnd;
};

int RegisterSubWindows(void);
void UnRegisterSubWindows(void);
#endif//SUB_WINDOW_H_
