#define LOG_NDEBUG 0
#define LOG_TAG "SensorDemo"
#include <utils/Log.h>

#include <errno.h>
#include <utils/Timers.h>

#include "SensorDemo.h"

static const char* GetSensorName(int type) {
	switch (type) {
	case SENSOR_TYPE_ACCELEROMETER:
		return "Acc";
	case SENSOR_TYPE_MAGNETIC_FIELD:
		return "Mag";
	case SENSOR_TYPE_ORIENTATION:
		return "Ori";
	case SENSOR_TYPE_GYROSCOPE:
		return "Gyr";
	case SENSOR_TYPE_LIGHT:
		return "Lgt";
	case SENSOR_TYPE_PRESSURE:
		return "Pres";
	case SENSOR_TYPE_TEMPERATURE:
		return "Temp";
	case SENSOR_TYPE_PROXIMITY:
		return "Prx";
	case SENSOR_TYPE_GRAVITY:
		return "Grv";
	case SENSOR_TYPE_LINEAR_ACCELERATION:
		return "LAcc";
	case SENSOR_TYPE_ROTATION_VECTOR:
		return "Rot";
	case SENSOR_TYPE_RELATIVE_HUMIDITY:
		return "RelHum";
	case SENSOR_TYPE_AMBIENT_TEMPERATURE:
		return "AmbTemp";
	default:
		return "others";
	}
}

SensorDemo::SensorDemo()
	:mErrorCode(0),
	 mSensordata(NULL),
	 mDevice(NULL),
	 mModule(NULL) {
	ALOGV("SensorDemo");

}

SensorDemo::~SensorDemo() {
	ALOGV("~SensorDemo");
}

int SensorDemo::init() {
	ALOGV("init");
	const struct sensor_t *list;

	int err = hw_get_module(SENSORS_HARDWARE_MODULE_ID, (const hw_module_t **) &mModule);
	if (0 != err) {
		ALOGE("get sensors module failed(%s)\n", strerror(errno));
		return -1;
	}

	err = sensors_open(&mModule->common, &mDevice);
	if (0 != err) {
		return -1;
	}

	mSensorCount = mModule->get_sensors_list(mModule, &list);
	if (0 == mSensorCount) {
		ALOGE("no sensors founded!\n");
		return -1;
	}

	for (int i = 0; i < mSensorCount; i++) {
		err = mDevice->activate(mDevice, list[i].handle, 0);
		if (0 != err)
			ALOGE("deactive sensor(%s) failed(%s)\n", list[i].name, strerror(errno));
	}
	for (int i = 0; i < mSensorCount; i++) {
		err = mDevice->activate(mDevice, list[i].handle, 1);
		if (0 != err)
			ALOGE("active sensor(%s) failed(%s)\n", list[i].name, strerror(errno));
		mDevice->setDelay(mDevice, list[i].handle, ms2ns(10));
	}

	sensors_event_t *mSensordata = (sensors_event_t*)malloc(mSensorCount * sizeof(sensors_event_t));
	if (NULL == mSensordata) {
		ALOGE("malloc failed, memory low!\n");
		return -1;
	}
	return 0;
}

int SensorDemo::reset() {
	ALOGV("reset");
	if (mSensordata) {
		free(mSensordata);
		mSensordata = NULL;
	}
	return 0;
}

int SensorDemo::run() {
	ALOGV("run");

	int err = mDevice->poll(mDevice, mSensordata, mSensorCount);
	if (err < 0) {
		ALOGE("poll failed(%s)\n", strerror(errno));
		return -1;
	}
	for (int i = 0; i < mSensorCount; i++) {
		if (mSensordata[i].version != sizeof(sensors_event_t)) {
			ALOGE("incorrect event(version = %d)\n", mSensordata[i].version);
		}
		ALOGV("sensor[%d]:%f %f %f", i, mSensordata[i].data[0], mSensordata[i].data[1], mSensordata[i].data[2]);
	}
	return 0;
}

//static
const char *SensorDemo::strerror(int error) {
	return NULL;
}

