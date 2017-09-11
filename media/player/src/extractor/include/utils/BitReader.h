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

#ifndef BIT_READER_H_

#define BIT_READER_H_


#include <sys/types.h>

namespace avformat {

struct BitReader {
    BitReader(const uint8_t *data, size_t size);
    ~BitReader();

    uint32_t getBits(size_t n);
    void skipBits(size_t n);

    void putBits(uint32_t x, size_t n);

    size_t numBitsLeft() const;

    const uint8_t *data() const;

private:
    const uint8_t *mData;
    size_t mSize;

    uint32_t mReservoir;  // left-aligned bits
    size_t mNumBitsLeft;

    void fillReservoir();
    BitReader(const BitReader&);
    BitReader &operator=(const BitReader&);
};

}  // namespace avformat

#endif  // BIT_READER_H_
