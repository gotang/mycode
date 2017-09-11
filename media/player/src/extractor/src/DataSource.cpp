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
#define LOG_TAG "DataSource"
#include <utils/Log.h>

#include "MPEG4Extractor.h"
#include <utils/Utils.h>
#include <DataSource.h>
#include <FileSource.h>
#include <MediaErrors.h>
#include <MediaDefs.h>

using namespace std;
namespace avformat {


bool DataSource::getUInt16(off64_t offset, uint16_t *x) {
    *x = 0;

    uint8_t byte[2];
    if (readAt(offset, byte, 2) != 2) {
        return false;
    }

    *x = (byte[0] << 8) | byte[1];

    return true;
}

bool DataSource::getUInt24(off64_t offset, uint32_t *x) {
    *x = 0;

    uint8_t byte[3];
    if (readAt(offset, byte, 3) != 3) {
        return false;
    }

    *x = (byte[0] << 16) | (byte[1] << 8) | byte[2];

    return true;
}

bool DataSource::getUInt32(off64_t offset, uint32_t *x) {
    *x = 0;

    uint32_t tmp;
    if (readAt(offset, &tmp, 4) != 4) {
        return false;
    }

    *x = ntohl(tmp);

    return true;
}

bool DataSource::getUInt64(off64_t offset, uint64_t *x) {
    *x = 0;

    uint64_t tmp;
    if (readAt(offset, &tmp, 8) != 8) {
        return false;
    }

    *x = ntoh64(tmp);

    return true;
}

status_t DataSource::getSize(off64_t *size) {
    *size = 0;

    return ERROR_UNSUPPORTED;
}

////////////////////////////////////////////////////////////////////////////////

Mutex DataSource::gSnifferMutex;
list<DataSource::SnifferFunc> DataSource::gSniffers;
bool DataSource::gSniffersRegistered = false;

bool DataSource::sniff(
        const char **mimeType, float *confidence){
    *confidence = 0.0f;
    *mimeType = NULL;
    {
        Mutex::Autolock autoLock(gSnifferMutex);
        if (!gSniffersRegistered) {
            return false;
        }
    }

    for(list<SnifferFunc>::const_iterator it = gSniffers.begin(); it != gSniffers.end(); ++it) {
    	float newConfidence;
    	const char *newMimeType = NULL;
    	if((*it)(this, &newMimeType, &newConfidence)) {
            if (newConfidence > *confidence) {
                *mimeType = newMimeType;
                *confidence = newConfidence;
            }
    	}
    }
    return *confidence > 0.0;
}

// static
void DataSource::RegisterSniffer_l(SnifferFunc func) {
    for(list<SnifferFunc>::const_iterator it = gSniffers.begin(); it != gSniffers.end(); ++it) {
    	if(func == *it)
    		return;
    }


    gSniffers.push_back(func);
}

// static
void DataSource::RegisterDefaultSniffers() {
    Mutex::Autolock autoLock(gSnifferMutex);
    if (gSniffersRegistered) {
        return;
    }

    RegisterSniffer_l(SniffMPEG4);
    gSniffersRegistered = true;
}

//static
DataSource* DataSource::CreateFromURI(const char *uri) {
	DataSource *dataSource = new FileSource(uri);
	return dataSource;

}

const char* DataSource::getMIMEType() const {
    return MEDIA_MIMETYPE_UNKOWN;
}

}  // namespace avformat
