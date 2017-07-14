/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "ColorConverter"
#include "Log.h"
#include <string.h>
#include "ColorConverter.h"

BitmapParams::BitmapParams(
		uint8_t *bits, int format,
		int32_t width, int32_t height,
        size_t cropLeft, size_t cropTop,
        size_t cropRight, size_t cropBottom, bool alpha)
    : mBits(bits),
      mAlpha(alpha),
      mFormat(format),
      mWidth(width),
      mHeight(height),
      mCropLeft(cropLeft),
      mCropTop(cropTop),
      mCropRight(cropRight),
      mCropBottom(cropBottom) {
}

template<typename T>
static void rotate0(T* dst, const T* src, size_t width, size_t height)
{
    memcpy(dst, src, width * height * sizeof(T));
}

template<typename T>
static void rotate90(T* dst, const T* src, size_t width, size_t height, bool mirror)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
        	if(mirror) {
        		dst[j * height + height - 1 - i] = src[(height -1 - i) * width + j];
        	} else {
        		dst[j * height + height - 1 - i] = src[i * width + j];
        	}
        }
    }
}

template<typename T>
static void rotate180(T* dst, const T* src, size_t width, size_t height, bool mirror)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
        	if(mirror) {
        		dst[(height - 1 - i) * width + width - 1 - j] = src[(height -1 - i) * width + j];
        	} else {
        		dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
        	}
        }
    }
}

template<typename T>
static void rotate270(T* dst, const T* src, size_t width, size_t height, bool mirror)
{
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
        	if(mirror) {
                dst[(width - 1 - j) * height + i] = src[(height -1 - i) * width + j];
        	} else {
        		dst[(width - 1 - j) * height + i] = src[i * width + j];
        	}
        }
    }
}

template<typename T>
static void rotate(T *dst, const T *src, size_t width, size_t height, int angle, bool mirror = false)
{
    switch (angle) {
        case 0:
            rotate0(dst, src, width, height);
            break;
        case 90:
            rotate90(dst, src, width, height, mirror);
            break;
        case 180:
            rotate180(dst, src, width, height, mirror);
            break;
        case 270:
            rotate270(dst, src, width, height, mirror);
            break;
    }
}

static int YV12Rotate(BitmapParams *source, BitmapParams *dest, int degree) {
	int err = 0;
	//rotate Y
	int YSize = source->mWidth * source->mHeight;
	int VSize = YSize/4;
	rotate(dest->mBits, source->mBits, source->mWidth, source->mHeight, degree);
	rotate(dest->mBits + YSize, source->mBits + YSize, source->mWidth/2, source->mHeight/2, degree);
	rotate(dest->mBits + YSize + VSize, source->mBits + YSize + VSize, source->mWidth/2, source->mHeight/2, degree);
	if(source->mAlpha) {
		rotate(dest->mBits + YSize + VSize * 2, source->mBits + YSize + VSize * 2, source->mWidth, source->mHeight, degree);
	}
	return err;
}

static int NV21Rotate(BitmapParams *source, BitmapParams *dest, int degree) {
	int err = 0;
	//rotate Y
	int YSize = source->mWidth * source->mHeight;
	int VSize = YSize/4;
	rotate(dest->mBits, source->mBits, source->mWidth, source->mHeight, degree);
	rotate((uint16_t *)(dest->mBits + YSize), (uint16_t*)(source->mBits + YSize),
			source->mWidth/2, source->mHeight/2, degree);
	if(source->mAlpha) {
		rotate(dest->mBits + YSize + VSize * 2, source->mBits + YSize + VSize * 2, source->mWidth, source->mHeight, degree);
	}

	return err;
}

static int RGBRotate(BitmapParams *source, BitmapParams *dest, int degree) {
	int err = 0;
	bool mirror = source->mHeight > 0 ? true : false;
	if(source->mFormat == COLOR_FORMAT_ARGB8888) {
		rotate((uint32_t *)dest->mBits, (uint32_t*)source->mBits, source->mWidth, abs(source->mHeight), degree, mirror);
	} else if(source->mFormat == COLOR_FORMAT_RGB888) {

	} else {
		err = -1;
	}
	return err;
}

int pictureRotate(BitmapParams *source, BitmapParams *dest, int degree) {
	int err = 0;
	ALOGV("rotate %d, format %d", degree, source->mFormat);
	if(source->mFormat == COLOR_FORMAT_YVU420_PLANNAR) {
		err = YV12Rotate(source, dest, degree);
	} else if(source->mFormat == COLOR_FORMAT_YVU420_SEMI_PLANNAR) {
		err = NV21Rotate(source, dest, degree);
	} else if(source->mFormat >COLOR_FORMAT_RGB_MIN && source->mFormat < COLOR_FORMAT_RGB_MAX) {
		err = RGBRotate(source, dest, degree);
	} else {
		err = -1;
	}
	return err;
}

//////////////////////////////////////////////////////////////////////////
static int YV12Scale(BitmapParams *source, BitmapParams *dest, int scale) {
	int err = 0;
	int YSize = source->mWidth * source->mHeight;
	int VSize = YSize/4;
	//Y
	for(int i = 0; i < dest->mHeight; i ++) {
		for(int j = 0; j < dest->mWidth; j++) {
			((uint16_t *)dest->mBits)[i * dest->mWidth + j] = ((uint16_t *)source->mBits)[2 * scale * j];
		}
	}
	//V
	for(int i = 0; i < dest->mHeight; i ++) {
		for(int j = 0; j < dest->mWidth; j++ ) {
			dest->mBits[i * dest->mWidth + j] = source->mBits[2 * j];
		}
	}
	//U
	for(int i = 0; i < dest->mHeight; i ++) {
		for(int j = 0; j < dest->mWidth; j++ ) {
			dest->mBits[i * dest->mWidth + j] = source->mBits[2 * j];
		}
	}
	return err;
}

static int NV21Scale(BitmapParams *source, BitmapParams *dest, int scale) {

	return 0;
}

static int ARGB32Scale(BitmapParams *source, BitmapParams *dest, int scale) {
	return 0;
}

int pictureScale(BitmapParams *source, BitmapParams *dest, int scale) {
	int err = 0;
	if(source->mFormat == COLOR_FORMAT_YVU420_PLANNAR) {
		err = YV12Scale(source, dest, scale);
	} else if(source->mFormat == COLOR_FORMAT_YVU420_SEMI_PLANNAR) {
		err = NV21Scale(source, dest, scale);
	} else if(source->mFormat == COLOR_FORMAT_ARGB8888) {
		err = ARGB32Scale(source, dest, scale);
	} else {
		err = -1;
	}
	return err;
}

//////////////////////////////////////////////////////////////////////////

//Y = 0.257R + 0.504G + 0.098B + 16
//U = 0.148R - 0.291G + 0.439B + 128
//V = 0.439R - 0.368G - 0.071B + 128
//B = 1.164(Y - 16) + 2.018(U - 128)
//G = 1.164(Y - 16) - 0.813(V - 128)  - 0.391(U - 128)
//R = 1.164(Y - 16) + 1.596(V - 128)
//
//
static int ARGBToNV21(BitmapParams *source, BitmapParams *dest) {
	int YSize = source->mWidth * source->mHeight;
	int USize = YSize/4;
	uint8_t *src = source->mBits;
	uint8_t *dest_y = dest->mBits;
	uint8_t *dest_alpha = dest->mBits + YSize + USize * 2;
	uint8_t *dest_c = dest->mBits + YSize/*alpha*/;

	for (size_t i = 0; i < source->mHeight; i++) {
		if ((i & 1) == 0) {
			for (int j = 0; j < source->mWidth; j++) {
				*dest_y = (299 * src[2] + 587 * src[1] + 114 * src[0]) / 1000;
				if ((j & 1) == 0) {
					*dest_c++ = 128 + (564 * (src[0] - *dest_y) / 1000);
				} else {
					*dest_c++ = 128 + (713 * (src[2] - *dest_y) / 1000);
				}

				*dest_alpha++ = src[3];
				src += 4;
				dest_y++;
			}
		} else {
			for (size_t j = 0; j < source->mWidth; j++) {
				*dest_y = (299 * src[2] + 587 * src[1] + 114 * src[0]) / 1000;
				*dest_alpha++ = src[3];
				src += 4;
				dest_y++;
			}
		}
	}
	return 0;
}

static int ARGBToYV12(BitmapParams *source, BitmapParams *dest) {
	int YSize = source->mWidth * source->mHeight;
	int USize = YSize/4;
	uint8_t *src = source->mBits;
	uint8_t *dest_y = dest->mBits;
	uint8_t *dest_alpha = dest->mBits + YSize + USize * 2;
	uint8_t *dest_v = dest->mBits + YSize/*alpha*/;
	uint8_t *dest_u = dest->mBits + YSize/*alpha*/ + USize;

	for (size_t i = 0; i < source->mHeight; i++) {
		if ((i & 1) == 0) {
			for (size_t j = 0; j < source->mWidth; j++) {
				*dest_y = (299 * src[2] + 587 * src[1] + 114 * src[0]) / 1000;
				if ((j & 1) == 0) {
					*dest_v++ = 128 + (564 * (src[0] - *dest_y) / 1000);
				} else {
					*dest_u++ = 128 + (713 * (src[2] - *dest_y) / 1000);
				}

				*dest_alpha++ = src[3];
				src += 4;
				dest_y++;
			}
		} else {
			for (size_t j = 0; j < source->mWidth; j++) {
				*dest_y = (299 * src[2] + 587 * src[1] + 114 * src[0]) / 1000;
				*dest_alpha++ = src[3];
				src += 4;
				dest_y++;
			}
		}
	}
	return 0;
}

struct RGB {
	uint32_t r;
	uint32_t g;
	uint32_t b;
};

static RGB yuvToRGB(uint8_t Y, uint8_t U, uint8_t V) {
	RGB rgb;
	rgb.r = (int) ((Y & 0xff) + 1.4075 * ((V & 0xff) - 128));
	rgb.g = (int) ((Y & 0xff) - 0.3455 * ((U & 0xff) - 128) - 0.7169 * ((V & 0xff) - 128));
	rgb.b = (int) ((Y & 0xff) + 1.779 * ((U & 0xff) - 128));
	rgb.r = (rgb.r < 0 ? 0 : rgb.r > 255 ? 255 : rgb.r);
	rgb.g = (rgb.g < 0 ? 0 : rgb.g > 255 ? 255 : rgb.g);
	rgb.b = (rgb.b < 0 ? 0 : rgb.b > 255 ? 255 : rgb.b);
	return rgb;
}

static int YV12ToRGB(BitmapParams *source, BitmapParams *dest) {
	int YSize = source->mWidth * source->mHeight;
	int VSize = YSize/4;

	uint8_t *rgb = dest->mBits;
	uint8_t *src = source->mBits;
	for (int i = 0; i < source->mHeight; i++) {
		int startY = i * source->mWidth;
		int step = (i / 2) * (source->mWidth / 2);
		int startV = YSize + step;
		int startU = YSize + VSize + step;
		for (int j = 0; j < source->mWidth; j++) {
			int Y = startY + j;
			int V = startV + j / 2;
			int U = startU + j / 2;
			int index = Y * 3;

			RGB tmp = yuvToRGB(src[Y], src[U], src[V]);
			rgb[index + 0] = tmp.r;
			rgb[index + 1] = tmp.g;
			rgb[index + 2] = tmp.b;
		}
	}
	return 0;
}

static status_t YV12ToRGB565(
        const BitmapParams *src, const BitmapParams *dst) {
	return 0;
}

static int YV12ToNV21(BitmapParams *source, BitmapParams *dest) {
	int err = 0;
	int YSize = source->mWidth * source->mHeight;
	int USize = YSize/4;
	memcpy(dest->mBits, source->mBits, YSize);

	for(int i = 0; i < USize; i ++) {
		*(dest->mBits + YSize + 2*i) = *(source->mBits + YSize + i);//V
		*(dest->mBits + YSize + 2*i + 1) = *(source->mBits + YSize + USize + i);//U
	}
	return err;
}

static int NV21ToYV12(BitmapParams *source, BitmapParams *dest) {
	int err = 0;

	int YSize = source->mWidth * source->mHeight;
	int USize = YSize/4;
	memcpy(dest->mBits, source->mBits, YSize);//Y

	for(int i = 0; i < USize; i ++) {
		*(dest->mBits + YSize + i) = *(source->mBits + YSize + 2*i);//V
		*(dest->mBits + YSize + USize + i) = *(source->mBits + YSize + 2*i + 1);//U
	}
	return err;
}

int colorConvert(BitmapParams *source, BitmapParams *dest) {

    CHECK(source->mWidth > 0);
    CHECK(source->mHeight > 0);

    CHECK(dest->mWidth > 0);
    CHECK(dest->mHeight > 0);

    CHECK((source->mWidth * source->mHeight) <= (dest->mHeight * dest->mWidth));

    ALOGV("convert from %d to %d", source->mFormat, dest->mFormat);
    int err;
    if(source->mFormat == COLOR_FORMAT_YVU420_PLANNAR &&
    		dest->mFormat == COLOR_FORMAT_YVU420_SEMI_PLANNAR) {
    	err = YV12ToNV21(source, dest);
    } else if(source->mFormat == COLOR_FORMAT_YVU420_SEMI_PLANNAR &&
    		dest->mFormat == COLOR_FORMAT_YVU420_PLANNAR) {
    	err = NV21ToYV12(source, dest);
    } else if(source->mFormat == COLOR_FORMAT_ARGB8888 &&
    		dest->mFormat == COLOR_FORMAT_YVU420_SEMI_PLANNAR) {
    	err = ARGBToNV21(source, dest);
    } else if(source->mFormat == COLOR_FORMAT_ARGB8888 &&
    		dest->mFormat == COLOR_FORMAT_YVU420_PLANNAR) {
    	err = ARGBToYV12(source, dest);
    } else if(source->mFormat == COLOR_FORMAT_YVU420_PLANNAR &&
    		dest->mFormat == COLOR_FORMAT_RGB888) {
    	err = YV12ToRGB(source, dest);
    } else if(source->mFormat == COLOR_FORMAT_YVU420_PLANNAR &&
    		dest->mFormat == COLOR_FORMAT_ARGB16) {
    	err = YV12ToRGB565(source, dest);
    }  else {
    	err = -1;
    }
    return err;
}
