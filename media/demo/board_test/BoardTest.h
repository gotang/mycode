#ifndef BOARD_TEST_H_
#define BOARD_TEST_H_

class BoardTest {
public:
	BoardTest();
	~BoardTest();
	int init();
	int reset();
	int loop();
	int boardTestProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
private:
	int test(HDC hdc);
	int mScreenWidth;
	int mScreenHeight;
	HWND mMainHandle;
};

#endif//BOARD_TEST_H_
