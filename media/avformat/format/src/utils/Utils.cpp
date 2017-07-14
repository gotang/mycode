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
#define LOG_TAG "Utils"
#include <utils/Log.h>

#include <arpa/inet.h>
#include <utils/Utils.h>

namespace avformat {

uint16_t U16_AT(const uint8_t *ptr) {
    return ptr[0] << 8 | ptr[1];
}

uint32_t U32_AT(const uint8_t *ptr) {
    return ptr[0] << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3];
}

uint64_t U64_AT(const uint8_t *ptr) {
    return ((uint64_t)U32_AT(ptr)) << 32 | U32_AT(ptr + 4);
}

uint16_t U16LE_AT(const uint8_t *ptr) {
    return ptr[0] | (ptr[1] << 8);
}

uint32_t U32LE_AT(const uint8_t *ptr) {
    return ptr[3] << 24 | ptr[2] << 16 | ptr[1] << 8 | ptr[0];
}

uint64_t U64LE_AT(const uint8_t *ptr) {
    return ((uint64_t)U32LE_AT(ptr + 4)) << 32 | U32LE_AT(ptr);
}

// XXX warning: these won't work on big-endian host.
uint64_t ntoh64(uint64_t x) {
    return ((uint64_t)ntohl(x & 0xffffffff) << 32) | ntohl(x >> 32);
}

uint64_t hton64(uint64_t x) {
    return ((uint64_t)htonl(x & 0xffffffff) << 32) | htonl(x >> 32);
}

void hexdump(const char *title, void * buf, uint32_t  size) {
	uint32_t i = 0;
	int32_t line ;

	int8_t tmp[32 + 16 + 11 + 1];
	uint8_t *data = (uint8_t *)buf;
	uint32_t  offset = 0;
	ALOGD("%s:", title);
	for(line = 0; offset < size; line ++ ) {
		sprintf((char*)&tmp[0], "0x%08x:", line*16);
		if(size - offset >= 16u) {
			for(i = 0; i < 16; i++) {
				sprintf((char*)&tmp[(i + 1) * 3 + 8], "%02x ", data[offset + i]);
			}
			offset += 16;
		} else {
			for(i = 0; i < size - offset; i++) {
				sprintf((char*)&tmp[(i + 1) * 3 + 8], "%02x ", data[offset + i]);
			}
			offset = size;
		}
		tmp[32 + 16 + 11] = '\0';
		ALOGD("%s", tmp);
	}
}

}  // namespace avformat

