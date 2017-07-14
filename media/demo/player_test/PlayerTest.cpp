#define LOG_NDEBUG 0
#define LOG_TAG "PlayerTest"
#include <utils/Log.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>

#include <utils/List.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

#include <media/stagefright/foundation/ADebug.h>
#include "PlayerDemo.h"

#define SCREEN_ROTATE_DIGREE 90

class PlayerTest : public PlayerDemoListener{
public:
	PlayerTest();
	virtual ~PlayerTest();
	virtual void notify(int msg, int ext1);

	status_t init(const char *file);
	status_t start();
	status_t stop();
	status_t reset();
	status_t loop();
private:
	bool mStop;
	sp<PlayerDemo> mPlayer;
};
//////////////////////////////////////////////////////////
static void getScreenSize(int *x, int *y, int *w, int *h) {
	int fd = open("/dev/graphics/fb0", O_RDWR);
	struct fb_var_screeninfo var;

	ioctl(fd, FBIOGET_VSCREENINFO, &var);
	*x = 0;
	*y = 0;
	*w = var.xres;
	*h = var.yres;

	if(SCREEN_ROTATE_DIGREE == 90
		|| SCREEN_ROTATE_DIGREE == 270) {
		//we need rotate to orignal phsycial resolustion.
		*w = var.yres;
		*h = var.xres;
	}
	close(fd);
}

static void printCommandLine() {
	printf("\ninput following character:\n");
	printf("\tq:exit demo\n");
}

PlayerTest::PlayerTest()
	:mStop(false),
	 mPlayer(NULL) {
}

PlayerTest::~PlayerTest() {
	reset();
	mPlayer.clear();
}

void PlayerTest::notify(int msg, int ext1) {
	mStop = true;
}

status_t PlayerTest::init(const char *file) {
	mPlayer = new PlayerDemo();
	mPlayer->setDataSource(file);
	mPlayer->setListener(this);
	mPlayer->setRotateDegree(SCREEN_ROTATE_DIGREE);
	int x,y,w,h;
	getScreenSize(&x, &y, &w, &h);
	mPlayer->setScreenSize(w, h);
	int err = mPlayer->init();
	return err;
}

status_t PlayerTest::start() {
	return mPlayer->start();
}

status_t PlayerTest::stop() {
	return mPlayer->stop();
}

status_t PlayerTest::reset() {
	return mPlayer->reset();
}

status_t PlayerTest::loop() {
	int err;
	char buf[3];
	ALOGV("run");
	printCommandLine();

	while (!mStop) {
		buf[0] = '\0';
		//get keyboard input.
		fd_set rfds;
		struct timeval tv;
		int retval;
		/* Watch stdin (fd 0) to see when it has input. */
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
        /* Wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        retval = select(1, &rfds, NULL, NULL, &tv);
		if (retval > 0) {
			//read
			read(0, buf, 3);
			switch (buf[0]) {
			case 'q':
				mStop = true;
				break;
			default:
				printCommandLine();
				break;
			}
		}
	}
	return 0;
}

static void printUsage(const char *app) {
	printf("Usage: %s <-f filename>\n", app);
}
struct A{
	A() {

	}
	int a;
	int b;
};

int main(int argc, char *argv[]) {
	sp<ProcessState> proc(ProcessState::self());
	ProcessState::self()->startThreadPool();
	List<A > test;
	A a = *test.begin();
	printf("a %d %d\n", a.a, a.b);
	return 0;
    int opt;
    const char *file = NULL;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
        	file = optarg;
            break;
        default:
        	printUsage(argv[0]);
        	exit(0);
        }
    }
    if(file == NULL) {
    	printUsage(argv[0]);
    	return 0;
    }

	PlayerTest *playerTest = new PlayerTest();
	int err = playerTest->init(file);
	CHECK_EQ(err, 0);

	err = playerTest->start();
	CHECK_EQ(err, 0);

	err = playerTest->loop();
	CHECK_EQ(err, 0);

	err = playerTest->stop();
	CHECK_EQ(err, 0);

	err = playerTest->reset();
	CHECK_EQ(err, 0);

	delete playerTest;
	return 0;
}
