/*
 * Copyright (C) 2010 The Android Open Source Project
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
#define LOG_TAG "Buffer"
#include <utils/Log.h>

#include <string.h>
#include "utils/Buffer.h"

namespace avformat {

Buffer::Buffer(size_t capacity)
    : mData(malloc(capacity)),
      mCapacity(capacity),
      mRangeOffset(0),
      mRangeLength(capacity),
      mOwnsData(true) {
	ALOGI("Buffer, this %p", this);
}

Buffer::Buffer(void *data, size_t capacity, bool create)
    : mData(data),
      mCapacity(capacity),
      mRangeOffset(0),
      mRangeLength(capacity),
      mOwnsData(false) {
	ALOGI("Buffer, this %p", this);
	if(create) {
		mData = malloc(capacity);
		memcpy(mData, data, capacity);
		mOwnsData = true;
	}
}

Buffer::~Buffer() {
	ALOGI("~Buffer, this %p", this);
    if (mOwnsData) {
        if (mData != NULL) {
            free(mData);
            mData = NULL;
        }
    }

}

void Buffer::setRange(size_t offset, size_t size) {
    CHECK_LE(offset, mCapacity);
    CHECK_LE(offset + size, mCapacity);

    mRangeOffset = offset;
    mRangeLength = size;
}

uint8_t *Buffer::base() {
	return (uint8_t *) mData;
}

uint8_t *Buffer::data() {
	return (uint8_t *) mData + mRangeOffset;
}

size_t Buffer::capacity() const {
	return mCapacity;
}

size_t Buffer::size() const {
	return mRangeLength;
}

size_t Buffer::offset() const {
	return mRangeOffset;
}

}  // namespace avformat

