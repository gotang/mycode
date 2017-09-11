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

#ifndef MPEG4_EXTRACTOR_H_

#define MPEG4_EXTRACTOR_H_

#include <vector>
using namespace std;
#include <arpa/inet.h>
#include <DataSource.h>
#include <MediaExtractor.h>

namespace avformat {

class DataSource;
class SampleTable;

struct SidxEntry {
    size_t mSize;
    uint32_t mDurationUs;
};

struct Trex {
    uint32_t track_ID;
    uint32_t default_sample_description_index;
    uint32_t default_sample_duration;
    uint32_t default_sample_size;
    uint32_t default_sample_flags;
};

class MPEG4Extractor : public MediaExtractor {
public:
    // Extractor assumes ownership of "source".
    MPEG4Extractor(DataSource *source);

    virtual size_t countTracks();
    virtual MediaSource* getTrack(size_t index);
    virtual TrackInfo *getTrackMetaData(size_t index, uint32_t flags);

    virtual MediaInfo *getMetaData();
    virtual uint32_t flags() const;

    // for DRM
    virtual char* getDrmTrackInfo(size_t trackID, int *len);
    virtual ~MPEG4Extractor();

private:

    struct PsshInfo {
        uint8_t uuid[16];
        uint32_t datalen;
        uint8_t *data;
    };
    struct Track {
    	Track();
    	~Track();
        Track *next;
        TrackInfo *trackInfo;
        uint32_t timescale;
        SampleTable *sampleTable;
        bool includes_expensive_metadata;
        bool skipTrack;
    private:
        Track(const Track&);
        Track &operator=(const Track&);
    };

    vector<SidxEntry> mSidxEntries;
    off64_t mMoofOffset;

    vector<PsshInfo> mPssh;

    vector<Trex> mTrex;

    DataSource *mDataSource;
    status_t mInitCheck;
    bool mHasVideo;
    uint32_t mHeaderTimescale;

    Track *mFirstTrack, *mLastTrack;

    MediaInfo *mFileMetaData;

    vector<uint32_t> mPath;
    char *mLastCommentMean;
    char *mLastCommentName;
    char *mLastCommentData;

    status_t readMetaData();
    status_t parseChunk(off64_t *offset, int depth);
    status_t parseITunesMetaData(off64_t offset, size_t size);
    status_t parse3GPPMetaData(off64_t offset, size_t size, int depth);
    void parseID3v2MetaData(off64_t offset);

    status_t updateAudioTrackInfoFromESDS_MPEG4Audio(
            const void *esds_data, size_t esds_size);

    static status_t verifyTrack(Track *track);

    struct SINF {
        SINF *next;
        uint16_t trackID;
        uint8_t IPMPDescriptorID;
        ssize_t len;
        char *IPMPData;
    };

    SINF *mFirstSINF;

    bool mIsDrm;
    status_t parseDrmSINF(off64_t *offset, off64_t data_offset);

    status_t parseTrackHeader(off64_t data_offset, off64_t data_size);

    status_t parseSegmentIndex(off64_t data_offset, size_t data_size);

    Track *findTrackByMimePrefix(const char *mimePrefix);

    MPEG4Extractor(const MPEG4Extractor &);
    MPEG4Extractor &operator=(const MPEG4Extractor &);
};
bool SniffMPEG4(DataSource *source, const char **mimeType, float *confidence);
}  // namespace avformat

#endif  // MPEG4_EXTRACTOR_H_
