#define LOG_NDEBUG 0
#define LOG_TAG "MICTest"
#include <utils/Log.h>

#include <assert.h>

#include "MICDemo.h"

int main(int argc, char *argv[]) {
	MICDemo *mic = new MICDemo();
	mic->setDuration(10);
	mic->setDataSource("/mnt/extsd/mictest.wav");
	int err = mic->run();
	assert(err != 0);
	delete mic;
	return 0;
}
