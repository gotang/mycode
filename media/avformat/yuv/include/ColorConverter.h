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

#ifndef COLOR_CONVERTER_H_

#define COLOR_CONVERTER_H_

#include <sys/types.h>

#include <stdint.h>

enum COLOR_FORMAT_TYPE {
	COLOR_FORMAT_UNKOWN = 0,

	COLOR_FORMAT_YUV_MIN,
	COLOR_FORMAT_YUV420_PLANNAR,//I420
	COLOR_FORMAT_YVU420_PLANNAR,//YV12
	COLOR_FORMAT_YUV420_SEMI_PLANNAR,//NV12
	COLOR_FORMAT_YVU420_SEMI_PLANNAR,//NV21
	COLOR_FORMAT_YUV_MAX,

	COLOR_FORMAT_RGB_MIN,
	COLOR_FORMAT_ARGB8888,//8
	COLOR_FORMAT_RGB888,
	COLOR_FORMAT_ARGB16,
	COLOR_FORMAT_ARGB8,
	COLOR_FORMAT_ARGB4,
	COLOR_FORMAT_ARGB1,
	COLOR_FORMAT_CbYCrY,
	COLOR_FORMAT_RGB_MAX
};

struct BitmapParams {
    BitmapParams(
    		uint8_t *bits, int format,
            int32_t width, int32_t height,
            size_t cropLeft, size_t cropTop,
            size_t cropRight, size_t cropBottom, bool alpha = false);

    uint8_t *mBits;
    bool mAlpha;
    int mFormat;
    int32_t mWidth, mHeight;
    size_t mCropLeft, mCropTop, mCropRight, mCropBottom;
};

int colorConvert(BitmapParams *source, BitmapParams *dest);
int pictureRotate(BitmapParams *source, BitmapParams *dest, int degree);

#endif  // COLOR_CONVERTER_H_
