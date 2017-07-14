#ifndef DEVICE_MANAGER_H_
#define DEVICE_MANAGER_H_

#include <map>
#include <vector>
using namespace std;

#include "Mathematics.h"

enum DEVICE_TYPE {
	TYPE_UNKOWN = 0,
	TYPE_TAG ,
	TYPE_ANCHOR
};

struct Device {
public:
	Device()
		:mId(-1) {
	}
	Device(const Device& d) {
		mId = d.mId;
		mCoord = d.mCoord;
	}
	Device& operator=(const Device& d) {
		mId = d.mId;
		mCoord = d.mCoord;
		ALOGD("id %d, (%8.2f, %8.2f, %8.2f)", mId,
				mCoord.x, mCoord.y, mCoord.z);
		return *this;
	}
	//fixed paramters
	int mId;
	Coordinate mCoord;
private:
};

struct Distance {
	int mId;//anchor id
	double mDist;//distance between tag and anchor mId
};

class DeviceManager {
public:
	DeviceManager();
	~DeviceManager();

	//type:DEVICE_TYPE
	//id: device id
	bool addDevice(int type, Device& device);
	bool removeDevice(int type, int id);
	void clearDevice(int type);

	void updateAnchors(vector<Device>& anchors);
	//update tag positioin
	void updateTagPosition(int tagid, vector<Distance>& d);
	bool getTagPosition(int tagid, Coordinate& c);
	void dump();
private:
	//find device
	ssize_t find(vector<Device>& device, int id);
	vector<Device> mAnchors;
	vector<Device> mTags;
	DeviceManager(const DeviceManager&);
	DeviceManager& operator=(const DeviceManager&);
};

#endif //DEVICE_MANAGER_H_
