#define LOG_NDEBUG 0
#define LOG_TAG "test"
#include "Log.h"

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <map>
#include <vector>
#include <assert.h>

#include "Trilateration.h"
#include "LinearLeastSquare.h"
#include "DeviceManager.h"

#include "gtest/gtest.h"
using namespace std;

const char *resultFile = "/home/sheldon/Documents/Share/data/2d moving.txt";

//2017-04-20 16:49:00.423 TOF{sequence: 11, tagid: 2014, anchorid: 9037, range: 8246 cm}
//2017-04-20 16:49:00.444 POS{seq: 11, id: 2014, x: 4049, y: 2053, z: 0, a: 0}

struct Range {
	int seq;
	int tagId;
	int anchorId;
	int range;
};

struct Tag {
	int id;
	int seq;
};

struct Anchor {
	int id;
	double x, y, z;
};

const Anchor Anchors[] = {
#if 0
		{9035, 0.00,0.00,0.00},
		{9036, 5751,0.00,0.00},
		{9037, 5338,10273,0.00},
		{9038, 693,10404,0.00},
#else
		{9035, 1890,0.00,0.00},
		{9036, 158, 10254,0.00},
		{9037, 0.00,0.00,0.00},
#endif
};

static void dumpRanges(vector<Range> &ranges) {
	printf("dump ranges:%zd\n", ranges.size());
	for(size_t i = 0; i < ranges.size(); i ++) {
		printf("TOF:seq:%d, tag:%d, anchor:%d, range:%d\n", ranges[i].seq,
				ranges[i].tagId, ranges[i].anchorId, ranges[i].range);
	}
}

void woxuTest() {

	FILE *fp = fopen(resultFile, "rb");
	EXPECT_TRUE(fp != NULL);

	char resLine[128];
	int range;
	int x, y, z;

	DeviceManager manager;
	Device device;
	vector<Device> anchors;

	//setup anchors
	for(size_t i = 0; i < sizeof(Anchors)/sizeof(Anchors[0]); i ++) {
		device.mId = Anchors[i].id;
		device.mCoord.x = Anchors[i].x;
		device.mCoord.y = Anchors[i].y;
		device.mCoord.z = Anchors[i].z;
		anchors.push_back(device);
	}
	manager.updateAnchors(anchors);
	manager.dump();

	vector<Range> ranges;
	const char *delimiter = NULL;

	while(fgets(resLine, sizeof(resLine), fp)) {

		bool over = false;
		size_t index = 0;
		if(delimiter = strstr(resLine, "TOF")) {
			Range d;
			sscanf(delimiter, "TOF{sequence: %d, tagid: %d, anchorid: %d, range: %d cm",
					&d.seq, &d.tagId, &d.anchorId, &d.range);
			ranges.push_back(d);
		} else if (delimiter = strstr(resLine, "POS")) {
			Tag tag;

			dumpRanges(ranges);

			sscanf(delimiter, "POS{seq: %d, id: %d, x: %d, y: %d, z: %d, a: 0}", &tag.seq, &tag.id, &x, &y, &z);
			printf("POS:seq:%d, tag:%d, x:%d, y:%d, z:%d\n", tag.seq, tag.id, x, y, z);

			vector<Distance> distances;
			vector<Range>::iterator it;
			for (it = ranges.begin(); it != ranges.end(); ) {
				Range r = (*it);
				if (r.tagId == tag.id) {
					if(tag.seq == r.seq) {
						Distance dist;
						dist.mId = r.anchorId;
						dist.mDist = r.range;
						distances.push_back(dist);
						ranges.erase(it);
					} else if(tag.seq > r.seq) {
						//useless range data, delete it
						ranges.erase(it);
					} else {
						++it;
					}
				}
			}
			manager.updateTagPosition(0, distances);
			//manager.dump();
			Coordinate c;
			manager.getTagPosition(0, c);
			printf("pos:seq:%d, tag:%d, x:%ld, y:%ld, z:%ld\n", tag.seq, tag.id, (long)c.x, (long)c.y, (long)c.z);
#if 1
			if(distances.size() >= 3) {
				vec3d solution;
				vec3d anchors[3];
				int distance[4] = {0};
				for(int i = 0; i < 3; i ++) {
					anchors[i].x = Anchors[distances[i].mId - 9035].x;
					anchors[i].y = Anchors[distances[i].mId - 9035].y;
					anchors[i].z = Anchors[distances[i].mId - 9035].z;
					distance[i] = distances[i].mDist;
				}

				int err = GetLocation(&solution, 0, &anchors[0], &distance[0]);
				printf("solution:(%.2f, %.2f, %.2f)\n\n", solution.x, solution.y, solution.z);
			}
#endif

		}
	}

	fclose(fp);
}
