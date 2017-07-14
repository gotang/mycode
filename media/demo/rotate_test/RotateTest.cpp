#define LOG_NDEBUG 0
#define LOG_TAG "RotateTest"
#include "Log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "RotateDemo.h"
using namespace android;
//#define BASE_NAME "/home/sheldon/workspace/server/workspace/library/media/demo/RotateTest/"
#define BASE_NAME "/mnt/extsd/photo/"
int main(int argc, char *argv[]) {
	int w = 640;
	int h = 368;
	const char *path;
	if (argc != 2 || argv[1] == NULL) {
		path = BASE_NAME"640x368.yuv";
	} else {
		path = argv[1];
	}
	if(access(path, F_OK)) {
		ALOGV("file %s is invalid", path);
		return -1;
	}
	int fd = open(path, O_RDONLY);
	off_t length = lseek(fd, 0, SEEK_END);
	assert(length = w*h*3/2);
	lseek(fd, 0, SEEK_SET);
	uint8_t *mem = (uint8_t*)malloc(length);
	ssize_t bytes = read(fd, mem, length);
	if(bytes != length) {
		ALOGV("read return %zd", bytes);
		return -1;
	}
	close(fd);
	uint8_t *rotated = (uint8_t*)malloc(length);

	int YSize = w*h;
	ALOGD("rotate with new one");
	int degree = 270;
	char output[512] = {0};
	for(int i = 0; i < 900; i ++) {
		rotateFast(rotated, mem, w, h, degree);
		rotateFast_u16((uint16_t*)(rotated + YSize), (uint16_t*)(mem + YSize), w/2, h/2, degree);
		usleep(30 * 1000);
	}
	if(degree == 90 || degree == 270) {
		sprintf(output, BASE_NAME"%dx%d_rotated_%d_0.yuv", h, w, degree);
	} else {
		sprintf(output, BASE_NAME"%dx%d_rotated_%d_0.yuv", w, h, degree);
	}
//		fd = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);
//		ALOGV("write rotated file:%s", output);
//		bytes = write(fd, rotated, length);
//		ALOGV("write return %zd", bytes);
//		close(fd);

	ALOGD("rotate with old one");
	printf("rotate with old one");
	for(int i = 0; i < 900; i ++) {
		rotate(rotated, mem, w, h, degree);
		rotate_u16((uint16_t*)(rotated + YSize), (uint16_t*)(mem + YSize), w/2, h/2, degree);
		usleep(30 * 1000);
	}
	//	sprintf(output, BASE_NAME"%dx%d_rotated_%d_1.yuv", h, w, 270);
	//	fd = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);
	//	ALOGV("write rotated file:%s", output);
	//	bytes = write(fd, rotated, length);
	//	ALOGV("write return %zd", bytes);
	//	close(fd);

	ALOGD("rotate finished");
	free(mem);
	free(rotated);

	return 0;
}
