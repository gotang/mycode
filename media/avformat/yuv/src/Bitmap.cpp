#define LOG_NDEBUG 0
#define LOG_TAG "Bitmap"
#include "Log.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ColorConverter.h"
#include "Bitmap.h"

void dumpBitmapHeader(BitMapFileHeader *fileHeader, BitMapInfoHeader *infoHeader) {
	ALOGV("type:0x%x, file size 0x%x, data offset 0x%x", fileHeader->mType, fileHeader->mSize, fileHeader->mOffBits);
	ALOGV("info size 0x%x, size(%d, %d), bit count %d, data size 0x%x", infoHeader->mSize,
			infoHeader->mWidth, infoHeader->mHeight, infoHeader->mBitCount, infoHeader->mSizeImage);
}

void getDirectName(const char *name, char *directname) {
	strcpy(directname, CURRENT_WORK_DIR"/data/");
}

void getBaseName(const char *name, char *basename) {
	const char *filename = strrchr(name, '/');
	filename ++;
	const char *suffix = strrchr(name, '.');
	if(suffix == NULL) {
		suffix = name + strlen(name);
	}
	int basenameLength = suffix - filename;
	strncpy(basename, filename, basenameLength);
}

void getnerateBitmap(const char *path, const uint8_t *rgb, int bitsPerPixel, int width, int height) {
	int fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644);
	if (fd >= 0) {
		BitMapFileHeader fileHeader;
		BitMapInfoHeader infoHeader;
		fileHeader.mType = 0x4D42;
		int dataSize = bitsPerPixel/8 * width * abs(height);
		fileHeader.mSize = dataSize + sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader);
		fileHeader.mReserved1 = 0;
		fileHeader.mReserved2 = 0;
		fileHeader.mOffBits = sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader);

		infoHeader.mSize = sizeof(BitMapInfoHeader);
		infoHeader.mWidth = width;
		infoHeader.mHeight = -height;
		infoHeader.mPlanes = 1;
		infoHeader.mBitCount = bitsPerPixel;
		infoHeader.mCompression = 0;
		infoHeader.mSizeImage = dataSize;
		infoHeader.mXPelsPerMeter = 0;
		infoHeader.mYPelsPerMeter = 0;
		infoHeader.mClrUsed = 0;
		infoHeader.mClrImportant = 0;
		write(fd, &fileHeader, sizeof(fileHeader));
		write(fd, &infoHeader, sizeof(infoHeader));
		write(fd, rgb, dataSize);
		close(fd);
	}
}

int bitmapRotate(const char *bitmap, int degree) {
	ALOGV("bitmapRotate");
	int fd = open(bitmap, O_RDONLY);
	CHECK(fd >= 0);

	BitMapFileHeader fileHeader;
	BitMapInfoHeader infoHeader;

	uint16_t header;
	ssize_t bytes = read(fd, &fileHeader, sizeof(fileHeader));
	CHECK(bytes == sizeof(fileHeader));
	bytes = read(fd, &infoHeader, sizeof(infoHeader));
	CHECK(bytes == sizeof(infoHeader));

	dumpBitmapHeader(&fileHeader, &infoHeader);
	if(infoHeader.mHeight < 0) {
		//OK, it's top to bottom order.
	}

	uint8_t *origin = (uint8_t *)malloc(infoHeader.mSizeImage);
	uint8_t *dest = (uint8_t *)malloc(infoHeader.mSizeImage);

	bytes = read(fd, origin, infoHeader.mSizeImage);
	CHECK(bytes == infoHeader.mSizeImage);
	close(fd);

	int format;
	if(infoHeader.mBitCount == 32) {
		format = COLOR_FORMAT_ARGB8888;
	} else if(infoHeader.mBitCount == 24) {
		format = COLOR_FORMAT_RGB888;
	} else {
		free(origin);
		free(dest);
		return -1;
	}
	int w = abs(infoHeader.mWidth);
	int h = abs(infoHeader.mHeight);
	BitmapParams source(origin, format, w, infoHeader.mHeight, 0, 0, 0, 0);
	BitmapParams rotated(dest, format, w, infoHeader.mHeight, 0, 0, 0, 0);
	int err = pictureRotate(&source, &rotated, degree);
	if(degree == 90 || degree == 270) {
		//after rotate, all bitmap is top to bottom order
		infoHeader.mWidth = h;
		infoHeader.mHeight = -w;
	} else if(degree == 180) {
		infoHeader.mWidth = w;
		infoHeader.mHeight = -abs(h);
	}
	char output[512] = {0};
	getDirectName(bitmap, output);
	getBaseName(bitmap, output + strlen(output));
	sprintf(output + strlen(output), "_rotated_%03d.bmp", degree);

	//write bitmap data, generate file.
	fd = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);
	CHECK(fd >= 0);
	ALOGV("file is %s", output);
	write(fd, &fileHeader, sizeof(fileHeader));
	write(fd, &infoHeader, sizeof(infoHeader));
	write(fd, dest, infoHeader.mSizeImage);
	close(fd);

	free(origin);
	free(dest);
	return 0;
}

int convertFromRGBAToARGB(const char *bitmap) {

	ALOGV("bitmapRotate");
	int intput = open(bitmap, O_RDONLY);
	CHECK(intput >= 0);

	BitMapFileHeader fileHeader;
	BitMapInfoHeader infoHeader;

	uint16_t header;
	ssize_t bytes = read(intput, &fileHeader, sizeof(fileHeader));
	CHECK(bytes == sizeof(fileHeader));
	bytes = read(intput, &infoHeader, sizeof(infoHeader));
	CHECK(bytes == sizeof(infoHeader));

	dumpBitmapHeader(&fileHeader, &infoHeader);
	if(infoHeader.mHeight < 0) {
		//OK, it's top to bottom order.
		close(intput);
		return 0;
	}

	int format;
	if(infoHeader.mBitCount == 32) {
		format = COLOR_FORMAT_ARGB8888;
	} else {
		return -1;
	}
	int w = abs(infoHeader.mWidth);
	int h = abs(infoHeader.mHeight);
	infoHeader.mWidth = h;
	infoHeader.mHeight = -w;


	char output[512] = {0};
	getDirectName(bitmap, output);
	getBaseName(bitmap, output + strlen(output));
	sprintf(output + strlen(output), "_mirror.bmp");
	//write bitmap data, generate file.
	int outputfd = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);
	CHECK(outputfd >= 0);
	ALOGV("file is %s", output);
	write(outputfd, &fileHeader, sizeof(fileHeader));
	write(outputfd, &infoHeader, sizeof(infoHeader));

	uint32_t pixel;
	for(size_t size = 0; size < infoHeader.mSizeImage; size += 4) {
		read(intput, &pixel, sizeof(pixel));
		write(outputfd, &pixel, sizeof(pixel));
	}
	close(outputfd);

	close(intput);

	return 0;
}
