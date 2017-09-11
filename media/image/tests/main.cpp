#define LOG_NDEBUG 0
#define LOG_TAG "main"
#include "Log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

#include "ColorConverter.h"
#include "waterMarkInterface.h"
#include "SDL.h"
#include "RawPlayer.h"
#include "Bitmap.h"

#define DEFAULT_WIDTH 768
#define DEFAULT_HEIGHT 1024
#define DEFULT_FORMAT (COLOR_FORMAT_YVU420_SEMI_PLANNAR)
#define DEFAULT_PATH CURRENT_WORK_DIR"/data/768x1024.yuv"

/////////////////////////////////////////////////////////////////
static void printUsage(const char *app) {
	printf("Usage: %s [OPTION]\n"
			" -a     function, 0:display YUV; 1:convert; 2:bitmap rotate; 3:rotate\n"
			" -p     raw data file path\n"
			" -w     width\n"
			" -h     height\n"
			" -f     format\n"
			" -r     rotate degree\n"
			" -t     time water mark\n"
			, app);
}

static int testRotate(const char *path, int w, int h, int format, int rotate) {
	int fd = open(path, O_RDONLY);
	off_t length = lseek(fd, 0, SEEK_END);
	assert(length = w*h*3/2);
	lseek(fd, 0, SEEK_SET);
	uint8_t *mem = (uint8_t*)malloc(length);
	read(fd, mem, length);
	close(fd);
	uint8_t *rotated = (uint8_t*)malloc(length);
	ALOGV("rotate %d", rotate);
	BitmapParams source(mem, format, w, h, 0, 0, 0, 0);
	BitmapParams dest(rotated, format, w, h, 0, 0, 0, 0);
	int err = pictureRotate(&source, &dest, rotate);
	ALOGV("rotate return %d", err);
	if(!err) {
		char output[512] = {0};
		getDirectName(path, output);
		if(rotate == 90 || rotate == 270) {
			h = w - h;
			w = w - h;
			h = w + h;
		}
		sprintf(output + strlen(output), "%dx%d_rotated_%d.yuv", w, h, rotate);
		fd = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);
		ALOGV("write rotated file:%s", output);
		ssize_t bytes = write(fd, rotated, length);
		ALOGV("write return %zd", bytes);
		close(fd);
	}
	free(mem);
	free(rotated);
	return err;
}

static int testBitmapRotate(const char *path) {
	int fd = open(path, O_RDONLY);
	CHECK(fd >= 0);
	uint16_t header;
	read(fd, &header, 2);
	int width;
	int height;
	lseek(fd, 18, SEEK_SET);
	read(fd, &width, 4);
	read(fd, &height, 4);
	if(height < 0) {
		//OK, it's top to bottom order.
	}
	int w = abs(width);
	int h = abs(height);

	lseek(fd, 28, SEEK_SET);
	read(fd, &header, 2);
	uint8_t *origin = (uint8_t *)malloc(w * h * 4);
	uint8_t *dest = (uint8_t *)malloc(w * h * 4);

	lseek(fd, 54, SEEK_SET);
	read(fd, origin, w * h * 4);

	ALOGV("bit map size (%d, %d)", w, h);

	BitmapParams source(origin, COLOR_FORMAT_ARGB8888, w, h, 0, 0, 0, 0);
	BitmapParams rotated(dest, COLOR_FORMAT_ARGB8888, w, h, 0, 0, 0, 0);
	int err = pictureRotate(&source, &rotated, 180);

	char output[512];
	strcpy(output, CURRENT_WORK_DIR"/data");
	const char *filename = strrchr(path, '/');
	strcat(output, filename);
	ALOGD("file is %s", output);
	int out = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);

	lseek(fd, 0, SEEK_SET);
	read(fd, origin, 54);
	write(out, origin, 54);
	write(out, dest, w * h * 4);
	lseek(out, 22, SEEK_SET);
	height = -height;
	write(out, &height, 4);

	free(origin);
	free(dest);
	close(out);
	close(fd);
	return 0;
}

static int testGenerateBitmapARGB888(const char *path, int w, int h) {
	int opt;
	int width = 0;
	int height = 0;
	int format = 0;
	int rotate = 0;
	int app = 0;

	bool timeWaterMark = false;
	ALOGD("file %s", path);
	int fd = open(path, O_RDONLY);
	if(fd >= 0) {
		struct stat st;
		int bytesPerPixel = 4;
		int err = fstat(fd, &st);
		size_t size = w * h * bytesPerPixel;
		ALOGV("file size %zd, size %zd", st.st_size, size);
		uint8_t *buffer = (uint8_t *) malloc(size);
		read(fd, buffer, size);

		char output[512] = {0};
		getDirectName(path, output);
		getBaseName(path, output + strlen(output));
		sprintf(output + strlen(output), "_bitmap.bmp");
		ALOGV("output file %s", output);
	    getnerateBitmap(output, buffer, bytesPerPixel * 8, w, h);
		free(buffer);
		close(fd);
	}
	return 0;
}

static int testConvert(const char *path, int width, int height) {
	int fd = open(path, O_RDONLY);
	if (fd >= 0) {
		struct stat st;
		fstat(fd, &st);
		int bitsPerPixel = 32;
		uint8_t *src = new uint8_t[st.st_size];
		read(fd, src, st.st_size);
		uint8_t *dest = new uint8_t[width * height * bitsPerPixel/8];
		BitmapParams source(src, COLOR_FORMAT_YVU420_PLANNAR, width, height, 0, 0, 0, 0);
		BitmapParams converted(dest, COLOR_FORMAT_RGB888, width, height, 0, 0, 0, 0);
		colorConvert(&source, &converted);
		close(fd);

		char output[512];
		strcpy(output, CURRENT_WORK_DIR"/data");
		const char *filename = strrchr(path, '/');
		const char *suffix = strrchr(path, '.');
		if(suffix == NULL) {
			suffix = path + strlen(path);
		}
		int basenameLength = suffix - filename;
		strncat(output, filename, basenameLength);
		strcat(output, "_converted.bmp");
		ALOGV("file is %s", output);

		getnerateBitmap(output, src, bitsPerPixel, width, height);

		ALOGV("bit map generated");
		delete[] dest;
		delete[] src;
		return 0;
	}
}

static int testRawPlayer(const char *path, int width, int height, int format, int rotate, bool timeWaterMark) {
	RawPlayer *player = new RawPlayer();
	int err = player->init(path, width, height, format, rotate, timeWaterMark);
	CHECK(err == 0);
	err = player->display();
	CHECK(err == 0);
	err = player->reset();
	CHECK(err == 0);
	delete player;
}


static int others(const char *path, int width, int height) {
	int fd = open(path, O_RDONLY);
	if (fd >= 0) {
		struct stat st;
		fstat(fd, &st);
		uint8_t *src = new uint8_t[st.st_size];
		read(fd, src, st.st_size);
		char output[512] = {0};
		getDirectName(path, output);
		getBaseName(path, output + strlen(output));
		sprintf(output + strlen(output), "_bmp.bmp");
		ALOGV("bitmap:%s", output);
		getnerateBitmap(output, src, st.st_size/width/height * 8, width, height);

		delete[] src;
		close(fd);
	}
	return 0;
}

int main( int argc, char* argv[])
{
	int opt;
	int width = 0;
	int height = 0;
	int format = 0;
	int rotate = 0;
	int app = 0;

	bool timeWaterMark = false;
	const char *path = NULL;

	while ((opt = getopt(argc, argv, "a:p:w:h:f:r:t")) != -1) {
		switch (opt) {
		case 'a':
			app = atoi(optarg);
			break;
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
			height = atoi(optarg);
			break;
		case 'p':
			path = strdup(optarg);
			break;
		case 'f':
			format = atoi(optarg);
			break;
		case 'r':
			rotate = atoi(optarg);
			break;
		case 't':
			timeWaterMark = true;
			break;
		default:
			printUsage (argv[0]);
			exit(0);
		}
	}
	if(access(path, F_OK)) {
		ALOGV("access file %s failed, use default", path);
		path = DEFAULT_PATH;
	}
	if(width <= 0 || height <= 0) {
		const char *resolution = strrchr(path, '/');
		if(resolution) {
			resolution ++;
			width = atoi(resolution);
			resolution = strrchr(resolution, 'x');
			if(resolution) {
				resolution++;
				height = atoi(resolution);
			}
		}
		if(width <= 0 || height <= 0) {
			ALOGV("invalid size(%d, %d), use default.", width, height);
			width = DEFAULT_WIDTH;
			height = DEFAULT_HEIGHT;
		}
	}

	if(format <= 0) {
		ALOGV("invalid format %d, use default.", format);
		format = DEFULT_FORMAT;
	}
	ALOGV("size is (%d, %d), format %d, rotate %d", width, height, format, rotate);
	ALOGV("file is %s", path);
	int err = 0;
	if(app == 0) {
		err = testRawPlayer(path, width, height, format, rotate, timeWaterMark);
	} else if(app == 1) {
		err = testConvert(path, width, height);
	} else if(app == 2) {
		err =  bitmapRotate(path, rotate);
	} else if(app == 3) {
		err =  testRotate(path, width, height, format, rotate);
	} else if(app == 4) {
		err = testGenerateBitmapARGB888(path, width, height);
	} else {
		err = others(path, width, height);
	}

    return err;
}

