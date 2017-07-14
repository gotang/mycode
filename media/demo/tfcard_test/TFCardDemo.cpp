#define LOG_NDEBUG 0
#define LOG_TAG "TFCardDemo"
#include <utils/Log.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "TFCardDemo.h"

TFCardDemo::TFCardDemo()
	:mErrorCode(0),
	 mMountPoint(NULL) {
	ALOGV("TFCardDemo");
}

TFCardDemo::~TFCardDemo() {
	ALOGV("~TFCardDemo");
	reset();
}

int TFCardDemo::init(const char *point) {
	mMountPoint = strdup(point);
	return 0;
}

int TFCardDemo::run() {
	int fd = open("/dev/block/mmcblk0", O_RDONLY);
	if(fd >= 0) {
		close(fd);
	} else {
		mErrorCode = ERROR_TFCARD_NOT_INSERTED;
		return -1;
	}
	if(!isMounted()) {
		mErrorCode = ERROR_NOT_MOUNTED;
		return -1;
	}

	int size = 1<<20;//1M
	uint8_t *source = (uint8_t *)malloc(size);
	uint8_t *dest = (uint8_t *)malloc(size);

	fd = open(mMountPoint, O_CREAT | O_TRUNC | O_RDWR, 0644);
	if(fd >= 0) {
		ssize_t bytes = ::write(fd, source, size);
		if(bytes != size) {
			mErrorCode = ERROR_WRITE_FAILED;
		}
		bytes = ::read(fd, dest, size);
		if(bytes != size) {
			mErrorCode = ERROR_READ_FAILED;
		}
		if(memcmp(source, dest, size)) {
			mErrorCode = ERROR_WRITE_FAILED;
		}
		close(fd);
	}
	free(dest);
	free(source);

	return 0;
}

int TFCardDemo::reset() {
	if(mMountPoint) {
		free(mMountPoint);
		mMountPoint = NULL;
	}
	return 0;
}

bool TFCardDemo::isMounted() {

	const char *path = "/proc/mounts";

	char line[255] = {0};
	const char *delim = " \t";

	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		ALOGE(" isMount fopen fail");
		return false;
	}

	while (fgets(line, sizeof(line), fp)) {
		char *save_ptr = NULL;
		char *p = NULL;
		if (line[0] != '/' || strncmp("/dev/block/vold", line, strlen("/dev/block/vold")) != 0) {
			memset(line, '\0', sizeof(line));
			continue;
		}
		if ((p = strtok_r(line, delim, &save_ptr)) != NULL) {
			if ((p = strtok_r(NULL, delim, &save_ptr)) != NULL) {
				if (!strncmp(mMountPoint, p, strlen(mMountPoint))) {
					fclose(fp);
					ALOGV(" isMount return 1\n");
					return true;
				}
			}
		}
	}
	if (fp) {
		fclose(fp);
	}
	return 0;
}

int64_t getNowUs() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000ll + tv.tv_usec;
}

typedef struct PACKED       //定义一个cpu occupy的结构体
{
	char name[20];      //定义一个char类型的数组名name有20个元素
	unsigned int user;  //定义一个无符号的int类型的user
	unsigned int nice;  //定义一个无符号的int类型的nice
	unsigned int system;//定义一个无符号的int类型的system
	unsigned int idle;  //定义一个无符号的int类型的idle
	unsigned int iowait;
	unsigned int irq;
	unsigned int softirq;
}CPU_OCCUPY;

void cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n, int *cpuUsage, int *iowait)
{
	unsigned int od, nd, total;
	unsigned int idle, iowt;
	int cpu_use = 0, iowt_use = 0;
	ALOGD("o:%d, %d, %d, %d, %d, %d, %d", o->user,o->nice,o->system,o->idle,o->iowait,o->irq,o->softirq);
	ALOGD("n:%d, %d, %d, %d, %d, %d, %d", n->user,n->nice,n->system,n->idle,n->iowait,n->irq,n->softirq);
	od = (unsigned int) (o->user + o->nice + o->system + o->idle + o->iowait + o->irq + o->softirq); //第一次(用户+优先级+系统+空闲)的时间再赋给od
	nd = (unsigned int) (n->user + n->nice + n->system + n->idle + n->iowait + n->irq + n->softirq); //第二次(用户+优先级+系统+空闲)的时间再赋给od

	idle = (unsigned int) (n->idle - o->idle); //用户第一次和第二次的时间之差再赋给id
	iowt = (unsigned int) (n->iowait - o->iowait); //系统第一次和第二次的时间之差再赋给sd
	idle += iowt;
	total = nd - od;//43
	ALOGD("total %d, idle %d, iowt %d", total, idle, iowt);
	if (total != 0) {
		cpu_use = (int) (idle * 100) / total;
		cpu_use = 100 - cpu_use;
		iowt_use = (int) (iowt * 100) / total;
	} else {
		cpu_use = 0;
		iowt_use = 0;
	}
	*cpuUsage = cpu_use;
	*iowait = iowt_use;
}

void get_cpuoccupy (CPU_OCCUPY *cpust) {
	FILE *fd;
	int n;
	char buff[256];
	CPU_OCCUPY *cpu_occupy;
	cpu_occupy = cpust;

	fd = fopen("/proc/stat", "r");
	fgets(buff, sizeof(buff), fd);

	sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system, &cpu_occupy->idle, &cpu_occupy->iowait, &cpu_occupy->irq, &cpu_occupy->softirq);

	fclose(fd);
}

static void writeFile(int id) {
	//5M/s, and write 30 times per sec.
	int size = 1048576;
	uint8_t *buffer = (uint8_t *)malloc(size);
	ALOGD("buffer %p", buffer);
	int err = posix_memalign((void **)&buffer, 4096, size);
	ALOGD("err %d, buffer %p", err, buffer);
	char path[128];
	snprintf(path, 128, "/mnt/extsd/file_%d.bin", id);
	int fd = ::open(path, O_CREAT | O_TRUNC | O_RDWR | O_DIRECT, 0644);
	assert(fd >= 0);
	int64_t start_time;
	int64_t diff_time = 0;
	int count = 1;
	CPU_OCCUPY cpu_stat1;
	CPU_OCCUPY cpu_stat2;
	int cpu_usage;
	int io_wait;
	while(1) {
		start_time = getNowUs();
		ALOGD("start write file %d", id);
		get_cpuoccupy((CPU_OCCUPY *) &cpu_stat1);
		::write(fd, buffer, size);
		get_cpuoccupy((CPU_OCCUPY *) &cpu_stat2);
		diff_time = getNowUs() - start_time;
		int64_t sleep_time = abs((1000*1000ll - diff_time)/(count%2 + 1));
		cal_cpuoccupy(&cpu_stat1, &cpu_stat2, &cpu_usage, &io_wait);
		ALOGD("write file %d, time %lldms, sleep time %lld, cpu %d%%, io %d%%", id, diff_time/1000,
				sleep_time, cpu_usage, io_wait);
		if(diff_time >1000*1000) {
			ALOGE("write too long");
		}
		usleep(sleep_time);
		count ++;
		if(count%30 == 0) {
			ALOGD("sleep 1s");
			sleep(1);
		}
	};
	close(fd);
	free(buffer);
}
static void *threadWrapper(void *arg) {
	int id = (int)arg;
	writeFile(id);
	pthread_exit(NULL);
	return NULL;
}

void TFCardDemo::write() {
	pthread_t thread;
	pthread_create(&thread, NULL, threadWrapper, (void *)0);
	writeFile(1);
}

//static
const char *TFCardDemo::strerror(int error) {
	return NULL;
}

