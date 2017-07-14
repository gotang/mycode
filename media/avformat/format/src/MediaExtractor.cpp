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
#define LOG_TAG "MediaExtractor"
#include <utils/Log.h>
#include <string.h>

#include "MPEG4Extractor.h"
#include <MediaInfo.h>
#include <DataSource.h>
#include <MediaDefs.h>
#include <MediaExtractor.h>

namespace avformat {

MediaInfo *MediaExtractor::getMetaData() {
    return NULL;
}

uint32_t MediaExtractor::flags() const {
    return CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_PAUSE | CAN_SEEK;
}

// static
MediaExtractor *MediaExtractor::Create(
        DataSource *source, const char *mime) {
    if (mime == NULL) {
        float confidence;
        if(!source->sniff(&mime, &confidence)) {
            ALOGV("FAILED to autodetect media content.");
            return NULL;
        }
        ALOGV("Autodetected media content as '%s' with confidence %.2f",
             mime, confidence);
    }

    MediaExtractor *ret = NULL;
    if (!strcasecmp(mime, MEDIA_MIMETYPE_CONTAINER_MPEG4)
            || !strcasecmp(mime, "audio/mp4")) {
        ret = new MPEG4Extractor(source);
    }
    return ret;
}
}  // namespace avformat
