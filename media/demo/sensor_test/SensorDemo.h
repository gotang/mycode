#ifndef SENSOR_DEMO_H_
#define SENSOR_DEMO_H_

using namespace android;

#include <hardware/sensors.h>

enum SENSOR_DEMO_STATUS{
	OK = 0,
	ERROR_xxx,
};

enum SENSOR_DEMO_EVENT {
	SENSOR_DEMO_ERROR,
};

class SensorDemo {
public:
	SensorDemo() ;
	~SensorDemo() ;
	int init();
	int run();
	int reset();
	int32_t getErrorCode() { return mErrorCode; }
	static const char *strerror(int error);
private:
	int32_t mErrorCode;
	int mSensorCount;
	sensors_event_t *mSensordata;
	struct sensors_poll_device_t* mDevice;
	struct sensors_module_t* mModule;
};

#endif //SENSOR_DEMO_H_
