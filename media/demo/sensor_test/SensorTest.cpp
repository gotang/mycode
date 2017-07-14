#define LOG_NDEBUG 0
#define LOG_TAG "SensorTest"
#include <utils/Log.h>

#include <assert.h>

#include "SensorDemo.h"
int main(int argc, char *argv[]) {
	SensorDemo *sensor = new SensorDemo();
	int err = sensor->init();
	assert(err != 0);

	err = sensor->run();
	assert(err != 0);

	err = sensor->reset();
	assert(err != 0);

	delete sensor;
	return 0;
}
