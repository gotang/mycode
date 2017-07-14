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

#ifndef BUFFER_H_

#define BUFFER_H_

#include <sys/types.h>

namespace avformat {
struct Buffer{
	Buffer(size_t capacity);
	Buffer(void *data, size_t capacity, bool create = false);
    uint8_t *base();
    uint8_t *data();
    size_t capacity() const;
    size_t size() const;
    size_t offset() const;

    void setRange(size_t offset, size_t size);
    ~Buffer();
private:
    void *mData;
    size_t mCapacity;
    size_t mRangeOffset;
    size_t mRangeLength;
    bool mOwnsData;
    Buffer(const Buffer&);
    Buffer &operator=(const Buffer&);
};

}  // namespace avformat

#endif  // BUFFER_H_
