#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
using namespace std;

struct Range {
	int seq;
	int tagId;
	int anchorId;
	int range;
};

static void printUsage(const char *app) {
	printf("Usage: %s [OPTION]\n"
			" -h     print this message\n"
			" -d     directory\n"
			" -f     file\n", app);
}

int handleFile(const char *file) {

	printf("handle file %s\n", file);
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		printf("cannot open file %s\n", file);
		return -1;
	}
	int total = 0;
	char resLine[128];
	char day_[32];
	char time_[32];
	vector<Range> ranges;

	//filter range information
	while (fgets(resLine, sizeof(resLine), fp)) {
		Range range;
		sscanf(resLine, "%s %s TOF{sequence: %d, tagid: %d, anchorid: %d, range: %d cm",
				day_, time_, &range.seq, &range.tagId, &range.anchorId, &range.range);
		total += range.range;
		ranges.push_back(range);
	}
	fclose(fp);

	//handle data
	if (ranges.size() <= 0) {
		return -1;
	}

	double average = 0;
	int expection = 0;
	const char *starter = strrchr(file, '/');
	if(starter == NULL) {
		starter = file;
	} else {
		starter ++;
	}
	expection = atoi(starter);

	average = (double) total / ranges.size();
	vector<Range>::iterator it;

	printf("count %zd, total %d, average:%.2f, expection:%d\n", ranges.size(), total, average, expection);
	double value = 0;
	for(int i = 0; i < 2; i ++) {
		value = i == 0 ? expection : average;

		int max = INT_MIN, min = INT_MAX;
		double sampleVariance = 0;
		double variance = 0;

		for (it = ranges.begin(); it != ranges.end(); ++it) {
			int delta = abs((*it).range - value);
			if (delta > max)
				max = delta;

			if (delta < min)
				min = delta;

			variance += powl((*it).range - value, 2);
			sampleVariance += powl((*it).range - value, 2);
		}
		if(ranges.size() > 1) {
			sampleVariance /= (ranges.size() - 1);
		}
		variance /= ranges.size();
//		printf("%s, min:%d, max:%d, variance:%.2f vs %.2f, standard deviation:%.2f vs %.2f\n",
//				i == 0 ? "expection" : "average", min, max, variance,
//				sampleVariance, sqrt(variance), sqrt(sampleVariance));
		if(i == 0) {
			printf("%d, %d, %.2f, %.2f\n",min, max, variance, sqrt(variance));
		} else {
			printf("%d, %d, %.2f, %.2f\n",min, max, sampleVariance, sqrt(sampleVariance));
		}
	}

	printf("\n");

	return 0;
}

int handleDirectory(const char *directory) {
	printf("handle directory %s\n", directory);
	DIR *dir;
	dir = opendir(directory);
	if (!dir) {
		return -1;
	}

	char path[256];
	struct dirent *entry;
	while (entry = readdir(dir)) {
		snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
		if(entry->d_type & DT_DIR) {
		if (strncmp(entry->d_name, ".", 1) == 0 || strncmp(entry->d_name, "..", 2) == 0)
			continue;

			handleDirectory(path);
		} else if (entry->d_type & DT_REG) {
			handleFile(path);
		}
	}
	closedir(dir);
	printf("\n\n");
	return 0;
}

int main(int argc, char *argv[]) {

	int opt;
	char path[128];
	if(argc < 2) {
		printUsage (argv[0]);
		exit(0);
	}
	while ((opt = getopt(argc, argv, "d:f:h")) != -1) {
		switch (opt) {
		case 'f':
		case 'd':
			strncpy(path, optarg, sizeof(path));
			break;
		case 'h':
		default:
			printUsage (argv[0]);
			exit(0);
		}
	}

	struct stat st;
	if(lstat(path, &st) == 0) {
		if(st.st_mode & S_IFREG) {
			handleFile(path);
		} else if(st.st_mode & S_IFDIR) {
			//remove last '/' if it exists
			if(!strncmp(path + strlen(path) - 1, "/", 1)) {
				path[strlen(path) - 1] = '\0';
			}
			handleDirectory(path);
		} else {
			printf("%s is neither a valid director nor a regular file\n", path);
		}
	}
	return 0;
}
