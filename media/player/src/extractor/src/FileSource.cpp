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
#define LOG_TAG "FileSource"
#include <utils/Log.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <linux/limits.h>
#include <FileSource.h>
#include <inttypes.h>
namespace avformat {

FileSource::FileSource(const char *filename)
    : mFd(-1),
      mOffset(0),
      mLength(-1) {

    mFd = open(filename, O_LARGEFILE | O_RDONLY);

    if (mFd >= 0) {
        mLength = lseek64(mFd, 0, SEEK_END);
    } else {
        ALOGE("Failed to open file '%s'. (%s)", filename, strerror(errno));
    }
}

FileSource::FileSource(int fd, int64_t offset, int64_t length)
    : mFd(fd),
      mOffset(offset),
      mLength(length) {
    CHECK(offset >= 0);
    CHECK(length >= 0);
}

FileSource::~FileSource() {
    if (mFd >= 0) {
        close(mFd);
        mFd = -1;
    }
}

status_t FileSource::initCheck() const {
    return mFd >= 0 ? OK : NO_INIT;
}

ssize_t FileSource::readAt(off64_t offset, void *data, size_t size) {
    if (mFd < 0) {
        return NO_INIT;
    }

    Mutex::Autolock autoLock(mLock);

    if (mLength >= 0) {
        if (offset >= mLength) {
            return 0;  // read beyond EOF.
        }
        int64_t numAvailable = mLength - offset;
        if ((int64_t)size > numAvailable) {
            size = numAvailable;
        }
    }

	off64_t result = lseek64(mFd, offset + mOffset, SEEK_SET);
	if (result == -1) {
		ALOGE("seek to %"PRId64" failed", offset + mOffset);
		return UNKNOWN_ERROR;
	}

	return ::read(mFd, data, size);
}

status_t FileSource::getSize(off64_t *size) {
    Mutex::Autolock autoLock(mLock);

    if (mFd < 0) {
        return NO_INIT;
    }

    *size = mLength;

    return OK;
}

}  // namespace avformat
