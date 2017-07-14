#define LOG_NDEBUG 0
#define LOG_TAG "LEDTest"
#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

enum {
	LED_POWER = 10,
	LED_ERROR = 11,
	LED_VIDEO = 12,
	LED_WIFI = 13
};

struct LEDMap{
	int idex;
	const char *item;
};

static LEDMap ledMap[] = {
		{LED_POWER, "LED_POWER"},
		{LED_ERROR, "LED_ERROR"},
		{LED_VIDEO, "LED_VIDEO"},
		{LED_WIFI, "LED_WIFI"},
};

int main(int argc, char *argv[]) {
	int fd = open("/dev/light", O_RDWR);
	if(fd > 0) {
		int err = ioctl(fd, LED_POWER, 1);
		sleep(5);
		for(int i = LED_ERROR; i <= LED_WIFI; i ++) {
			for(int j = 1; j >= 0; j--) {
				ALOGV("test [%s:%d]", ledMap[i - LED_POWER].item, j);
				int err = ioctl(fd, i, j);
				if(err != 0) {
					ALOGE("set [%d][%d] failed %s", i, j, strerror(errno));
				}
				sleep(5);
			}
		}
		close(fd);
	}
	return 0;
}
