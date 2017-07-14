#ifndef BIT_MAP_H_
#define BIT_MAP_H_

#pragma pack(push)
#pragma pack(2)
struct BitMapFileHeader {
	uint16_t mType;
	uint32_t mSize;
	uint16_t mReserved1;
	uint16_t mReserved2;
	uint32_t mOffBits;
};

struct BitMapInfoHeader {
	uint32_t mSize;
	uint32_t mWidth;
	uint32_t mHeight;
	uint16_t mPlanes;
	uint16_t mBitCount;
	uint32_t mCompression;
	uint32_t mSizeImage;
	uint32_t mXPelsPerMeter;
	uint32_t mYPelsPerMeter;
	uint32_t mClrUsed;
	uint32_t mClrImportant;
};
#pragma pack(pop)

void getDirectName(const char *name, char *directname);
void getBaseName(const char *name, char *basename);
void getnerateBitmap(const char *path, const uint8_t *rgb, int bitsPerPixel, int width, int height);
int bitmapRotate(const char *bitmap, int degree);
#endif //BIT_MAP_H_
