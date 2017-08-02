#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <climits>
#include <cmath>
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
			" -f     file\n", app);
}

int main(int argc, char *argv[]) {

	int opt;
	char path[128];
	if(argc < 2) {
		printUsage (argv[0]);
		exit(0);
	}
	while ((opt = getopt(argc, argv, "f:h")) != -1) {
		switch (opt) {
		case 'f':
			strncpy(path, optarg, sizeof(path));
			break;
		case 'h':
		default:
			printUsage (argv[0]);
			exit(0);
		}
	}

	DIR *dir;
	dir = opendir(path);
	if (!dir) {
		return -1;
	}

	struct dirent *entry;
	while (entry = readdir(dir)) {
		if (!(entry->d_type & DT_REG)) {
			continue;
		}
		printf("file:%s\n", entry->d_name);
		continue;
		FILE *fp = fopen(entry->d_name, "rb");
		if (fp != NULL) {
			int max = INT_MIN, min = INT_MAX;
			int total = 0;
			double average = 0;
			double sampleVariance = 0;
			double variance = 0;
			int expection;

			char resLine[128];
			vector<Range> ranges;

			//filter range information
			while (fgets(resLine, sizeof(resLine), fp)) {
				Range range;
				sscanf(resLine, "TOF{sequence: %d, tagid: %d, anchorid: %d, range: %d cm",
						&range.seq, &range.tagId, &range.anchorId, &range.range);
				total += range.range;
				ranges.push_back(range);
			}
			fclose(fp);

			//handle data
			if (ranges.size() > 0) {
				average = (double) total / ranges.size();
				vector<Range>::iterator it;
				for (it = ranges.begin(); it != ranges.end(); ++it) {
					int delta = abs((*it).range - expection);
					if (delta > max)
						max = delta;

					if (delta < min)
						min = delta;

					variance += powl((*it).range - expection, 2);
					sampleVariance += powl((*it).range - expection, 2);
				}
				sampleVariance /= (ranges.size() - 1);
				variance /= ranges.size();
				printf("expection, min:%d, max:%d, average:%.2f, variance:%.2f vs %.2f, "
						"standard deviation:%.2f vs %.2f\n", min, max, average, variance,
						sampleVariance, sqrt(variance),	sqrt(sampleVariance));

				max = INT_MIN, min = INT_MAX;
				average = 0;
				sampleVariance = 0;
				variance = 0;
				for (it = ranges.begin(); it != ranges.end(); ++it) {
					int delta = abs((*it).range - average);
					if (delta > max)
						max = delta;

					if (delta < min)
						min = delta;

					variance += powl((*it).range - average, 2);
					sampleVariance += powl((*it).range - average, 2);
				}
				sampleVariance /= (ranges.size() - 1);
				variance /= ranges.size();
				printf("average, min:%d, max:%d, average:%.2f, variance:%.2f vs %.2f, "
						"standard deviation:%.2f vs %.2f\n", min, max, average, variance,
						sampleVariance, sqrt(variance),	sqrt(sampleVariance));

			}
		}
	}
	closedir(dir);
	return 0;
}
