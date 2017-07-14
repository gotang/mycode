//#define LOG_NDEBUG 0
#define LOG_TAG "DeviceManager"
#include "Log.h"

#include "Trilateration.h"
#include "LinearLeastSquare.h"
#include "DeviceManager.h"

DeviceManager::DeviceManager() {
	mAnchors.clear();
	mTags.clear();
}

DeviceManager::~DeviceManager() {
	mAnchors.clear();
	mTags.clear();
}

bool DeviceManager::addDevice(int type, Device& device) {
	ALOGV("addDevice");

	vector<Device> *devices = &mTags;
	if(type == TYPE_ANCHOR) {
		devices = &mAnchors;
	} else if (type != TYPE_TAG) {
		return false;
	}
	int index = find(*devices, device.mId);
	if(index < 0) {
		//didn't find
		devices->push_back(device);
	}
	return true;
}

bool DeviceManager::removeDevice(int type, int id) {
	ALOGV("removeDevice");

	vector<Device> *devices = &mTags;
	if(type == TYPE_ANCHOR) {
		devices = &mAnchors;
	} else if (type != TYPE_TAG) {
		return false;
	}
	vector<Device>::iterator it;
	while(!devices->empty()) {
		it = devices->begin();
		if((*it).mId == id) {
			devices->erase(it);
			break;
		}
	}
	return true;
}

void DeviceManager::clearDevice(int type) {
	if(type == TYPE_ANCHOR) {
		mAnchors.clear();
	} else if (type != TYPE_TAG) {
		mTags.clear();
	}
}

void DeviceManager::updateAnchors(vector<Device>& anchors) {
	ALOGV("updateAnchors");

	for(size_t i = 0; i < anchors.size(); i ++) {
		int index = find(mAnchors, anchors[i].mId);
		if(index >= 0) {
			//find anchor
			mAnchors[index].mCoord = anchors[i].mCoord;
		} else {
			//TODO: add this anchor ????
			mAnchors.push_back(anchors[i]);
		}
	}
}

//update tag positioin
void DeviceManager::updateTagPosition(int tagid, vector<Distance>& d) {
	ALOGV("updateTagPosition");

	int index;
	Sphere sphere;
	vector<Sphere> spheres;
	for(size_t i = 0; i < d.size(); i ++) {
		index = find(mAnchors, d[i].mId);
		if(index >= 0) {
			//find anchor
			sphere.r = d[i].mDist;
			sphere.c = mAnchors[index].mCoord;
			spheres.push_back(sphere);
		}
	}

	Coordinate coord;
	if(trilateration(spheres, coord)) {
		//update tag position
		index = find(mTags, tagid);
		if(index >= 0) {
			mTags[index].mCoord = coord;
		} else {
			//new tag
			Device t;
			t.mId = tagid;
			t.mCoord = coord;
			mTags.push_back(t);
		}
	}
}

bool DeviceManager::getTagPosition(int tagid, Coordinate& c) {
	ALOGV("getTagPosition");
	ssize_t index = find(mTags, tagid);
	if(index < 0) {
		return false;
	}
	c = mTags[index].mCoord;
	return true;
}

//find device
ssize_t DeviceManager::find(vector<Device>& device, int id) {
	ssize_t index = -1;
	for (size_t i = 0; i < device.size(); i ++) {
		if(device[i].mId == id) {
			index = i;
			break;
		}
	}
	return index;
}

void DeviceManager::dump() {
	for(size_t i = 0; i < mTags.size(); i ++) {
		ALOGV("Tag[%zd]:%d, (%8.2f, %8.2f, %8.2f)", i, mTags[i].mId,
				mTags[i].mCoord.x, mTags[i].mCoord.y, mTags[i].mCoord.z);
	}

	for(size_t i = 0; i < mAnchors.size(); i ++) {
		ALOGV("Anchor[%zd]:%d, (%8.2f, %8.2f, %8.2f)", i, mAnchors[i].mId,
				mAnchors[i].mCoord.x, mAnchors[i].mCoord.y, mAnchors[i].mCoord.z);
	}
}
