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

#ifndef MPEG2_TS_EXTRACTOR_H_
#define MPEG2_TS_EXTRACTOR_H_
#include <vector>
using namespace std;

#include <MediaExtractor.h>

namespace avformat {

struct AnotherPacketSource;
struct ATSParser;
struct DataSource;
struct MPEG2TSSource;

struct MPEG2TSExtractor : public MediaExtractor {
    MPEG2TSExtractor(DataSource *source);
    ~MPEG2TSExtractor();

    virtual size_t countTracks();
    virtual MediaSource* getTrack(size_t index);
    virtual TrackInfo *getTrackMetaData(size_t index, uint32_t flags);

    virtual MediaInfo *getMetaData();
    virtual uint32_t flags() const;

private:
    friend struct MPEG2TSSource;

    mutable Mutex mLock;

    DataSource *mDataSource;
    ATSParser *mParser;
    vector<AnotherPacketSource *> mSourceImpls;
    MediaInfo *mFileMetaData;
    off64_t mOffset;

    void init();
    status_t feedMore();
    MPEG2TSExtractor(const MPEG2TSExtractor&);
    MPEG2TSExtractor &operator=(const MPEG2TSExtractor&);
};

bool SniffMPEG2TS(DataSource *source, const char **mimeType, float *confidence);

}  // namespace avformat

#endif  // MPEG2_TS_EXTRACTOR_H_
