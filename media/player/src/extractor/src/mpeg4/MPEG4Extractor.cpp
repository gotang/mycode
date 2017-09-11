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
#define LOG_TAG "MPEG4Extractor"
#include <utils/Log.h>

#include <ctype.h>
#include <string.h>

#include <utils/Utils.h>
#include <utils/Buffer.h>
#include <utils/BitReader.h>

#include <MediaDefs.h>
#include <MediaSource.h>
#include "MPEG4Extractor.h"
#include "SampleTable.h"
#include "ESDS.h"
#include <inttypes.h>
namespace avformat {

class MPEG4Source : public MediaSource {
public:
    // Caller retains ownership of both "dataSource" and "sampleTable".
    MPEG4Source(MPEG4Extractor *owner,
    			const TrackInfo *format,
    			DataSource *dataSource,
                int32_t timeScale,
                SampleTable *sampleTable,
                vector<SidxEntry> &sidx,
                const Trex *trex,
                off64_t firstMoofOffset);

	virtual status_t start();
	virtual status_t stop();

	virtual const TrackInfo *getFormat();

	virtual status_t read(AVPacket *packet, const ReadOptions *options = NULL);
    virtual status_t fragmentedRead(AVPacket *packet, const ReadOptions *options = NULL);

protected:
    virtual ~MPEG4Source();

private:
    Mutex mLock;

    // keep the MPEG4Extractor around, since we're referencing its data
    MPEG4Extractor *mOwner;
	const TrackInfo *mFormat;
	DataSource *mDataSource;
	int32_t mTimescale;
	SampleTable *mSampleTable;
	uint32_t mCurrentSampleIndex;
	uint32_t mCurrentFragmentIndex;
	vector<SidxEntry> &mSegments;
    const Trex *mTrex;
    off64_t mFirstMoofOffset;
    off64_t mCurrentMoofOffset;
    off64_t mNextMoofOffset;
    uint32_t mCurrentTime;
    int32_t mLastParsedTrackId;
    int32_t mTrackId;

    int32_t mCryptoMode;    // passed in from extractor
    int32_t mDefaultIVSize; // passed in from extractor
    uint8_t mCryptoKey[16]; // passed in from extractor
    uint32_t mCurrentAuxInfoType;
    uint32_t mCurrentAuxInfoTypeParameter;
    int32_t mCurrentDefaultSampleInfoSize;
    uint32_t mCurrentSampleInfoCount;
    uint32_t mCurrentSampleInfoAllocSize;
    uint8_t* mCurrentSampleInfoSizes;
    uint32_t mCurrentSampleInfoOffsetCount;
    uint32_t mCurrentSampleInfoOffsetsAllocSize;
    uint64_t* mCurrentSampleInfoOffsets;

    bool mIsAVC;
    bool mIsHEVC;
    size_t mNALLengthSize;

    bool mStarted;

    bool mWantsNALFragments;

    uint8_t *mSrcBuffer;

    size_t parseNALSize(const uint8_t *data) const;
    status_t parseChunk(off64_t *offset);
    status_t parseTrackFragmentHeader(off64_t offset, off64_t size);
    status_t parseTrackFragmentRun(off64_t offset, off64_t size);
    status_t parseSampleAuxiliaryInformationSizes(off64_t offset, off64_t size);
    status_t parseSampleAuxiliaryInformationOffsets(off64_t offset, off64_t size);

    struct TrackFragmentHeaderInfo {
        enum Flags {
            kBaseDataOffsetPresent         = 0x01,
            kSampleDescriptionIndexPresent = 0x02,
            kDefaultSampleDurationPresent  = 0x08,
            kDefaultSampleSizePresent      = 0x10,
            kDefaultSampleFlagsPresent     = 0x20,
            kDurationIsEmpty               = 0x10000,
        };

        uint32_t mTrackID;
        uint32_t mFlags;
        uint64_t mBaseDataOffset;
        uint32_t mSampleDescriptionIndex;
        uint32_t mDefaultSampleDuration;
        uint32_t mDefaultSampleSize;
        uint32_t mDefaultSampleFlags;

        uint64_t mDataOffset;
    };
    TrackFragmentHeaderInfo mTrackFragmentHeaderInfo;

    struct Sample {
        off64_t offset;
        size_t size;
        uint32_t duration;
        int32_t compositionOffset;
        uint8_t iv[16];
        vector<size_t> clearsizes;
        vector<size_t> encryptedsizes;
    };
    vector<Sample> mCurrentSamples;

    MPEG4Source(const MPEG4Source &);
    MPEG4Source &operator=(const MPEG4Source &);
};

////////////////////////////////////////////////////////////////////////////////

static const char *FourCC2MIME(uint32_t fourcc) {
    switch (fourcc) {
        case FOURCC('m', 'p', '4', 'a'):
            return MEDIA_MIMETYPE_AUDIO_AAC;

        case FOURCC('s', 'a', 'm', 'r'):
            return MEDIA_MIMETYPE_AUDIO_AMR_NB;

        case FOURCC('s', 'a', 'w', 'b'):
            return MEDIA_MIMETYPE_AUDIO_AMR_WB;

        case FOURCC('m', 'p', '4', 'v'):
            return MEDIA_MIMETYPE_VIDEO_MPEG4;

        case FOURCC('s', '2', '6', '3'):
        case FOURCC('h', '2', '6', '3'):
        case FOURCC('H', '2', '6', '3'):
            return MEDIA_MIMETYPE_VIDEO_H263;

        case FOURCC('a', 'v', 'c', '1'):
            return MEDIA_MIMETYPE_VIDEO_AVC;

        case FOURCC('h', 'v', 'c', '1'):
        case FOURCC('h', 'e', 'v', '1'):
            return MEDIA_MIMETYPE_VIDEO_HEVC;
        default:
            CHECK(!"should not be here.");
            return NULL;
    }
}

static bool AdjustChannelsAndRate(uint32_t fourcc, uint32_t *channels, uint32_t *rate) {
    if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AMR_NB, FourCC2MIME(fourcc))) {
        // AMR NB audio is always mono, 8kHz
        *channels = 1;
        *rate = 8000;
        return true;
    } else if (!strcasecmp(MEDIA_MIMETYPE_AUDIO_AMR_WB, FourCC2MIME(fourcc))) {
        // AMR WB audio is always mono, 16kHz
        *channels = 1;
        *rate = 16000;
        return true;
    }
    return false;
}

MPEG4Extractor::Track::Track()
	:trackInfo(NULL),
	 sampleTable(NULL) {
	ALOGI("MPEG4Extractor::Track, this %p", this);

}

MPEG4Extractor::Track::~Track() {
	ALOGI("MPEG4Extractor::~Track, this %p", this);
	if (trackInfo) {
		delete trackInfo;
		trackInfo = NULL;
	}
	if (sampleTable) {
		delete sampleTable;
		sampleTable = NULL;
	}
}

MPEG4Extractor::MPEG4Extractor(DataSource *source)
    : mMoofOffset(0),
      mDataSource(source),
      mInitCheck(NO_INIT),
      mHasVideo(false),
      mHeaderTimescale(0),
      mFirstTrack(NULL),
	 mLastTrack(NULL),
	 mFileMetaData(new MediaInfo),
      mFirstSINF(NULL),
      mIsDrm(false) {
	ALOGI("MPEG4Extractor, this %p", this);
	for (int i = 0; i < kKeyEnd; i++) {
		mFileMetaData->mMetaData[i] = NULL;
	}
}

MPEG4Extractor::~MPEG4Extractor() {
    Track *track = mFirstTrack;
    while (track) {
        Track *next = track->next;

        delete track;
        track = next;
    }
    mFirstTrack = mLastTrack = NULL;

    SINF *sinf = mFirstSINF;
    while (sinf) {
        SINF *next = sinf->next;
        delete[] sinf->IPMPData;
        delete sinf;
        sinf = next;
    }
    mFirstSINF = NULL;

    for (size_t i = 0; i < mPssh.size(); i++) {
        delete [] mPssh[i].data;
    }

	for (int i = 0; i < kKeyEnd; i++) {
		if(mFileMetaData->mMetaData[i]) {
			delete mFileMetaData->mMetaData[i];
		}
	}
	if(mLastCommentMean) {
		free(mLastCommentMean);
	}
	if(mLastCommentName) {
		free(mLastCommentName);
	}
	if(mLastCommentData) {
		free(mLastCommentData);
	}
}

uint32_t MPEG4Extractor::flags() const {
    return CAN_PAUSE |
            ((mMoofOffset == 0 || mSidxEntries.size() != 0) ?
                    (CAN_SEEK_BACKWARD | CAN_SEEK_FORWARD | CAN_SEEK) : 0);
}

MediaInfo* MPEG4Extractor::getMetaData() {
	ALOGV("getMetaData");
	status_t err;
	if ((err = readMetaData()) != OK) {
		return NULL;
	}

    return mFileMetaData;
}

size_t MPEG4Extractor::countTracks() {
	ALOGV("countTracks");
	status_t err;
    if ((err = readMetaData()) != OK) {
        ALOGV("MPEG4Extractor::countTracks: no tracks");
        return 0;
    }

    size_t n = 0;
    Track *track = mFirstTrack;
    while (track) {
        ++n;
        track = track->next;
    }

    ALOGV("MPEG4Extractor::countTracks: %zu tracks", n);
    return n;
}

TrackInfo *MPEG4Extractor::getTrackMetaData(size_t index, uint32_t flags) {
	ALOGV("getTrackMetaData");
	status_t err;
    if ((err = readMetaData()) != OK) {
        return NULL;
    }

    Track *track = mFirstTrack;
    while (index > 0) {
        if (track == NULL) {
            return NULL;
        }

        track = track->next;
        --index;
    }

    if (track == NULL) {
        return NULL;
    }

    if ((flags & kIncludeExtensiveMetaData)
            && !track->includes_expensive_metadata) {
		track->includes_expensive_metadata = true;

		const char *mime = track->trackInfo->mMIME;
		if (!strncasecmp("video/", mime, 6)) {
			if (mMoofOffset > 0) {
				int64_t duration = track->trackInfo->mDuration;
				// nothing fancy, just pick a frame near 1/4th of the duration
				track->trackInfo->mThumbnailTime = duration / 4;
			} else {
                uint32_t sampleIndex;
				uint32_t sampleTime;
				if (track->sampleTable->findThumbnailSample(&sampleIndex) == OK
						&& track->sampleTable->getMetaDataForSample(sampleIndex,
								NULL /* offset */, NULL /* size */, &sampleTime)
								== OK) {
					track->trackInfo->mThumbnailTime = ((int64_t) sampleTime
							* 1000000) / track->timescale;
                }
            }
        }
    }

	return track->trackInfo;
}

static void MakeFourCCString(uint32_t x, char *s) {
    s[0] = x >> 24;
    s[1] = (x >> 16) & 0xff;
    s[2] = (x >> 8) & 0xff;
    s[3] = x & 0xff;
    s[4] = '\0';
}

status_t MPEG4Extractor::readMetaData() {
	ALOGV("readMetaData");
	if (mInitCheck != NO_INIT) {
        return mInitCheck;
    }

    off64_t offset = 0;
    status_t err;
    while (true) {
        off64_t orig_offset = offset;
        err = parseChunk(&offset, 0);

        if (err != OK && err != (int32_t)UNKNOWN_ERROR) {
            break;
        } else if (offset <= orig_offset) {
            // only continue parsing if the offset was advanced,
            // otherwise we might end up in an infinite loop
            ALOGE("did not advance: 0x%"PRId64"0x%"PRId64, orig_offset, offset);
            err = ERROR_MALFORMED;
            break;
        } else if (err == OK) {
            continue;
        }

        uint32_t hdr[2];
        if (mDataSource->readAt(offset, hdr, 8) < 8) {
            break;
        }
        uint32_t chunk_type = ntohl(hdr[1]);
        if (chunk_type == FOURCC('m', 'o', 'o', 'f')) {
            // store the offset of the first segment
            mMoofOffset = offset;
        } else if (chunk_type != FOURCC('m', 'd', 'a', 't')) {
            // keep parsing until we get to the data
            continue;
        }
        break;
    }
	if (mInitCheck == OK) {
		if (mHasVideo) {
			mFileMetaData->mMIME = MEDIA_MIMETYPE_CONTAINER_MPEG4;
		} else {
			mFileMetaData->mMIME = MEDIA_MIMETYPE_AUDIO_MP4;
		}
    } else {
        mInitCheck = err;
    }

    CHECK_NE(err, (status_t)NO_INIT);

    // copy pssh data into file metadata
    int psshsize = 0;
    for (size_t i = 0; i < mPssh.size(); i++) {
        psshsize += 20 + mPssh[i].datalen;
    }
    if (psshsize) {
        char *buf = (char*)malloc(psshsize);
        char *ptr = buf;
        for (size_t i = 0; i < mPssh.size(); i++) {
            memcpy(ptr, mPssh[i].uuid, 20); // uuid + length
            memcpy(ptr + 20, mPssh[i].data, mPssh[i].datalen);
            ptr += (20 + mPssh[i].datalen);
        }

		mFileMetaData->mMetaData[kKeyPssh] = new Buffer(buf, psshsize);
		mFileMetaData->mMetaData[kKeyPssh]->setRange(0, psshsize);
    }
    return mInitCheck;
}

char* MPEG4Extractor::getDrmTrackInfo(size_t trackID, int *len) {
	ALOGV("getDrmTrackInfo");
	if (mFirstSINF == NULL) {
        return NULL;
    }

    SINF *sinf = mFirstSINF;
    while (sinf && (trackID != sinf->trackID)) {
        sinf = sinf->next;
    }

    if (sinf == NULL) {
        return NULL;
    }

    *len = sinf->len;
    return sinf->IPMPData;
}

// Reads an encoded integer 7 bits at a time until it encounters the high bit clear.
static int32_t readSize(off64_t offset,
		DataSource *DataSource, uint8_t *numOfBytes) {
    uint32_t size = 0;
    uint8_t data;
    bool moreData = true;
    *numOfBytes = 0;

    while (moreData) {
        if (DataSource->readAt(offset, &data, 1) < 1) {
            return -1;
        }
        offset ++;
        moreData = (data >= 128) ? true : false;
        size = (size << 7) | (data & 0x7f); // Take last 7 bits
        (*numOfBytes) ++;
    }

    return size;
}

status_t MPEG4Extractor::parseDrmSINF(
        off64_t * /* offset */, off64_t data_offset) {
	ALOGV("parseDrmSINF");
    uint8_t updateIdTag;
    if (mDataSource->readAt(data_offset, &updateIdTag, 1) < 1) {
        return ERROR_IO;
    }
    data_offset ++;

    if (0x01/*OBJECT_DESCRIPTOR_UPDATE_ID_TAG*/ != updateIdTag) {
        return ERROR_MALFORMED;
    }

    uint8_t numOfBytes;
    int32_t size = readSize(data_offset, mDataSource, &numOfBytes);
    if (size < 0) {
        return ERROR_IO;
    }
    int32_t classSize = size;
    data_offset += numOfBytes;

    while(size >= 11 ) {
        uint8_t descriptorTag;
        if (mDataSource->readAt(data_offset, &descriptorTag, 1) < 1) {
            return ERROR_IO;
        }
        data_offset ++;

        if (0x11/*OBJECT_DESCRIPTOR_ID_TAG*/ != descriptorTag) {
            return ERROR_MALFORMED;
        }

        uint8_t buffer[8];
        //ObjectDescriptorID and ObjectDescriptor url flag
        if (mDataSource->readAt(data_offset, buffer, 2) < 2) {
            return ERROR_IO;
        }
        data_offset += 2;

        if ((buffer[1] >> 5) & 0x0001) { //url flag is set
            return ERROR_MALFORMED;
        }

        if (mDataSource->readAt(data_offset, buffer, 8) < 8) {
            return ERROR_IO;
        }
        data_offset += 8;

        if ((0x0F/*ES_ID_REF_TAG*/ != buffer[1])
                || ( 0x0A/*IPMP_DESCRIPTOR_POINTER_ID_TAG*/ != buffer[5])) {
            return ERROR_MALFORMED;
        }

        SINF *sinf = new SINF;
        sinf->trackID = U16_AT(&buffer[3]);
        sinf->IPMPDescriptorID = buffer[7];
        sinf->next = mFirstSINF;
        mFirstSINF = sinf;

        size -= (8 + 2 + 1);
    }

    if (size != 0) {
        return ERROR_MALFORMED;
    }

    if (mDataSource->readAt(data_offset, &updateIdTag, 1) < 1) {
        return ERROR_IO;
    }
    data_offset ++;

    if(0x05/*IPMP_DESCRIPTOR_UPDATE_ID_TAG*/ != updateIdTag) {
        return ERROR_MALFORMED;
    }

    size = readSize(data_offset, mDataSource, &numOfBytes);
    if (size < 0) {
        return ERROR_IO;
    }
    classSize = size;
    data_offset += numOfBytes;

    while (size > 0) {
        uint8_t tag;
        int32_t dataLen;
        if (mDataSource->readAt(data_offset, &tag, 1) < 1) {
            return ERROR_IO;
        }
        data_offset ++;

        if (0x0B/*IPMP_DESCRIPTOR_ID_TAG*/ == tag) {
            uint8_t id;
            dataLen = readSize(data_offset, mDataSource, &numOfBytes);
            if (dataLen < 0) {
                return ERROR_IO;
            } else if (dataLen < 4) {
                return ERROR_MALFORMED;
            }
            data_offset += numOfBytes;

            if (mDataSource->readAt(data_offset, &id, 1) < 1) {
                return ERROR_IO;
            }
            data_offset ++;

            SINF *sinf = mFirstSINF;
            while (sinf && (sinf->IPMPDescriptorID != id)) {
                sinf = sinf->next;
            }
            if (sinf == NULL) {
                return ERROR_MALFORMED;
            }
            sinf->len = dataLen - 3;
            sinf->IPMPData = new (std::nothrow) char[sinf->len];
            if (sinf->IPMPData == NULL) {
                return ERROR_MALFORMED;
            }
            data_offset += 2;

            if (mDataSource->readAt(data_offset, sinf->IPMPData, sinf->len) < sinf->len) {
                return ERROR_IO;
            }
            data_offset += sinf->len;

            size -= (dataLen + numOfBytes + 1);
        }
    }

    if (size != 0) {
        return ERROR_MALFORMED;
    }

    return UNKNOWN_ERROR;  // Return a dummy error.
}

struct PathAdder {
	PathAdder(vector<uint32_t> *path, uint32_t chunkType) :
			mPath(path) {
		mPath->push_back(chunkType);
	}

	~PathAdder() {
		mPath->pop_back();
	}

private:
	vector<uint32_t> *mPath;

	PathAdder(const PathAdder &);
	PathAdder &operator=(const PathAdder &);
};

static bool underMetaDataPath(const vector<uint32_t> &path) {
    return path.size() >= 5
        && path[0] == FOURCC('m', 'o', 'o', 'v')
        && path[1] == FOURCC('u', 'd', 't', 'a')
        && path[2] == FOURCC('m', 'e', 't', 'a')
        && path[3] == FOURCC('i', 'l', 's', 't');
}

// Given a time in seconds since Jan 1 1904, produce a human-readable string.
static void convertTimeToDate(int64_t time_1904, char **s) {
	time_t time_1970 = time_1904 - (((66 * 365 + 17) * 24) * 3600);

	char tmp[32];
	strftime(tmp, sizeof(tmp), "%Y%m%dT%H%M%S.000Z", gmtime(&time_1970));
	*s = strdup(tmp);
}

status_t MPEG4Extractor::parseChunk(off64_t *offset, int depth) {
	ALOGV("entering parseChunk 0x%"PRIx64"/%d", *offset, depth);
    uint32_t hdr[2];
    if (mDataSource->readAt(*offset, hdr, 8) < 8) {
        return ERROR_IO;
    }
    uint64_t chunk_size = ntohl(hdr[0]);
    uint32_t chunk_type = ntohl(hdr[1]);
    off64_t data_offset = *offset + 8;

    if (chunk_size == 1) {
        if (mDataSource->readAt(*offset + 8, &chunk_size, 8) < 8) {
            return ERROR_IO;
        }
        chunk_size = ntoh64(chunk_size);
        data_offset += 8;

        if (chunk_size < 16) {
            // The smallest valid chunk is 16 bytes long in this case.
            return ERROR_MALFORMED;
        }
    } else if (chunk_size == 0) {
        if (depth == 0) {
            // atom extends to end of file
            off64_t sourceSize;
            if (mDataSource->getSize(&sourceSize) == OK) {
                chunk_size = (sourceSize - *offset);
            } else {
                // XXX could we just pick a "sufficiently large" value here?
                ALOGE("atom size is 0, and data source has no size");
                return ERROR_MALFORMED;
            }
        } else {
            // not allowed for non-toplevel atoms, skip it
            *offset += 4;
            return OK;
        }
    } else if (chunk_size < 8) {
        // The smallest valid chunk is 8 bytes long.
        ALOGE("invalid chunk size: %" PRIu64, chunk_size);
        return ERROR_MALFORMED;
    }

    char chunk[5];
	MakeFourCCString(chunk_type, chunk);
	ALOGV("chunk: %s @ 0x%"PRIx64", %d, size 0x%"PRIx64, chunk, *offset, depth, chunk_size);

#if 0
    static const char kWhitespace[] = "                                        ";
    const char *indent = &kWhitespace[sizeof(kWhitespace) - 1 - 2 * depth];
    printf("%sfound chunk '%s' of size %" PRIu64 "\n", indent, chunk, chunk_size);

    char buffer[256];
    size_t n = chunk_size;
    if (n > sizeof(buffer)) {
        n = sizeof(buffer);
    }
    if (mDataSource->readAt(*offset, buffer, n)
            < (ssize_t)n) {
        return ERROR_IO;
    }

    hexdump(buffer, n);
#endif

    PathAdder autoAdder(&mPath, chunk_type);

    off64_t chunk_data_size = *offset + chunk_size - data_offset;

    if (chunk_type != FOURCC('c', 'p', 'r', 't')
            && chunk_type != FOURCC('c', 'o', 'v', 'r')
            && mPath.size() == 5 && underMetaDataPath(mPath)) {
        off64_t stop_offset = *offset + chunk_size;
        *offset = data_offset;
        while (*offset < stop_offset) {
            status_t err = parseChunk(offset, depth + 1);
            if (err != OK) {
                return err;
            }
        }

        if (*offset != stop_offset) {
            return ERROR_MALFORMED;
        }

        return OK;
    }

    switch(chunk_type) {
        case FOURCC('m', 'o', 'o', 'v'):
        case FOURCC('t', 'r', 'a', 'k'):
        case FOURCC('m', 'd', 'i', 'a'):
        case FOURCC('m', 'i', 'n', 'f'):
        case FOURCC('d', 'i', 'n', 'f'):
        case FOURCC('s', 't', 'b', 'l'):
        case FOURCC('m', 'v', 'e', 'x'):
        case FOURCC('m', 'o', 'o', 'f'):
        case FOURCC('t', 'r', 'a', 'f'):
        case FOURCC('m', 'f', 'r', 'a'):
        case FOURCC('u', 'd', 't', 'a'):
        case FOURCC('i', 'l', 's', 't'):
        case FOURCC('s', 'i', 'n', 'f'):
        case FOURCC('s', 'c', 'h', 'i'):
        case FOURCC('e', 'd', 't', 's'):
        {
            if (chunk_type == FOURCC('s', 't', 'b', 'l')) {
                ALOGV("sampleTable chunk is %" PRIu64 " bytes long.", chunk_size);
                mLastTrack->sampleTable = new SampleTable(mDataSource);
            }

            bool isTrack = false;
            if (chunk_type == FOURCC('t', 'r', 'a', 'k')) {
                isTrack = true;

                Track *track = new Track;
                track->next = NULL;
                if (mLastTrack) {
                    mLastTrack->next = track;
                } else {
                    mFirstTrack = track;
                }
                mLastTrack = track;

    			track->trackInfo = new TrackInfo;
    			track->includes_expensive_metadata = false;
    			track->skipTrack = false;
    			track->timescale = 0;
    			track->trackInfo->mMIME = MEDIA_MIMETYPE_UNKOWN;
            }

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset;
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }

            if (isTrack) {
                if (mLastTrack->skipTrack) {
                    Track *cur = mFirstTrack;

                    if (cur == mLastTrack) {
                        delete cur;
                        mFirstTrack = mLastTrack = NULL;
                    } else {
                        while (cur && cur->next != mLastTrack) {
                            cur = cur->next;
                        }
                        cur->next = NULL;
                        delete mLastTrack;
                        mLastTrack = cur;
                    }

                    return OK;
                }

                status_t err = verifyTrack(mLastTrack);

                if (err != OK) {
                    return err;
                }
            } else if (chunk_type == FOURCC('m', 'o', 'o', 'v')) {
                mInitCheck = OK;

                if (!mIsDrm) {
                    return UNKNOWN_ERROR;  // Return a dummy error.
                } else {
                    return OK;
                }
            }
            break;
        }

        case FOURCC('e', 'l', 's', 't'):
        {
            *offset += chunk_size;

            // See 14496-12 8.6.6
            uint8_t version;
            if (mDataSource->readAt(data_offset, &version, 1) < 1) {
                return ERROR_IO;
            }

            uint32_t entry_count;
            if (!mDataSource->getUInt32(data_offset + 4, &entry_count)) {
                return ERROR_IO;
            }

            if (entry_count != 1) {
                // we only support a single entry at the moment, for gapless playback
                ALOGW("ignoring edit list with %d entries", entry_count);
            } else if (mHeaderTimescale == 0) {
                ALOGW("ignoring edit list because timescale is 0");
            } else {
                off64_t entriesoffset = data_offset + 8;
                uint64_t segment_duration;
                int64_t media_time;

                if (version == 1) {
                    if (!mDataSource->getUInt64(entriesoffset, &segment_duration) ||
                            !mDataSource->getUInt64(entriesoffset + 8, (uint64_t*)&media_time)) {
                        return ERROR_IO;
                    }
                } else if (version == 0) {
                    uint32_t sd;
                    int32_t mt;
                    if (!mDataSource->getUInt32(entriesoffset, &sd) ||
                            !mDataSource->getUInt32(entriesoffset + 4, (uint32_t*)&mt)) {
                        return ERROR_IO;
                    }
                    segment_duration = sd;
                    media_time = mt;
                } else {
                    return ERROR_IO;
                }

                uint64_t halfscale = mHeaderTimescale / 2;
                segment_duration = (segment_duration * 1000000 + halfscale)/ mHeaderTimescale;
                media_time = (media_time * 1000000 + halfscale) / mHeaderTimescale;

                int64_t duration = mLastTrack->trackInfo->mDuration;
                int32_t samplerate = mLastTrack->trackInfo->mSampleRate;
                if (duration > 0 && samplerate > 0) {

                    int64_t delay = (media_time  * samplerate + 500000) / 1000000;
                    mLastTrack->trackInfo->mEncoderDelay = delay;

                    int64_t paddingus = duration - (segment_duration + media_time);
                    if (paddingus < 0) {
                        // track duration from media header (which is what kKeyDuration is) might
                        // be slightly shorter than the segment duration, which would make the
                        // padding negative. Clamp to zero.
                        paddingus = 0;
                    }
                    int64_t paddingsamples = (paddingus * samplerate + 500000) / 1000000;
                    mLastTrack->trackInfo->mEncoderPadding = paddingsamples;
                }
            }
            break;
        }

        case FOURCC('f', 'r', 'm', 'a'):
        {
            *offset += chunk_size;

            uint32_t original_fourcc;
            if (mDataSource->readAt(data_offset, &original_fourcc, 4) < 4) {
                return ERROR_IO;
            }
            original_fourcc = ntohl(original_fourcc);
            ALOGV("read original format: %d", original_fourcc);
    		mLastTrack->trackInfo->mMIME = FourCC2MIME(original_fourcc);
            uint32_t num_channels = 0;
            uint32_t sample_rate = 0;
            if (AdjustChannelsAndRate(original_fourcc, &num_channels, &sample_rate)) {
    			mLastTrack->trackInfo->mChannelCount = num_channels;
    			mLastTrack->trackInfo->mSampleRate = sample_rate;
            }
            break;
        }

        case FOURCC('t', 'e', 'n', 'c'):
        {
            *offset += chunk_size;

            if (chunk_size < 32) {
                return ERROR_MALFORMED;
            }

            // tenc box contains 1 byte version, 3 byte flags, 3 byte default algorithm id, one byte
            // default IV size, 16 bytes default KeyID
            // (ISO 23001-7)
            char buf[4];
            memset(buf, 0, 4);
            if (mDataSource->readAt(data_offset + 4, buf + 1, 3) < 3) {
                return ERROR_IO;
            }
            uint32_t defaultAlgorithmId = ntohl(*((int32_t*)buf));
            if (defaultAlgorithmId > 1) {
                // only 0 (clear) and 1 (AES-128) are valid
                return ERROR_MALFORMED;
            }

            memset(buf, 0, 4);
            if (mDataSource->readAt(data_offset + 7, buf + 3, 1) < 1) {
                return ERROR_IO;
            }
            uint32_t defaultIVSize = ntohl(*((int32_t*)buf));

            if ((defaultAlgorithmId == 0 && defaultIVSize != 0) ||
                    (defaultAlgorithmId != 0 && defaultIVSize == 0)) {
                // only unencrypted data must have 0 IV size
                return ERROR_MALFORMED;
            } else if (defaultIVSize != 0 &&
                    defaultIVSize != 8 &&
                    defaultIVSize != 16) {
                // only supported sizes are 0, 8 and 16
                return ERROR_MALFORMED;
            }

            uint8_t defaultKeyId[16];

            if (mDataSource->readAt(data_offset + 8, &defaultKeyId, 16) < 16) {
                return ERROR_IO;
            }

    		mLastTrack->trackInfo->mCryptoMode = defaultAlgorithmId;
    		mLastTrack->trackInfo->mCryptoDefaultIVSize = defaultIVSize;
    		mLastTrack->trackInfo->mCryptoTENCKey = new Buffer(16);
    		memcpy(mLastTrack->trackInfo->mCryptoTENCKey->data(), defaultKeyId, 16);
            break;
        }

        case FOURCC('t', 'k', 'h', 'd'):
        {
            *offset += chunk_size;

            status_t err;
            if ((err = parseTrackHeader(data_offset, chunk_data_size)) != OK) {
                return err;
            }

            break;
        }

        case FOURCC('p', 's', 's', 'h'):
        {
            *offset += chunk_size;

            PsshInfo pssh;

            if (mDataSource->readAt(data_offset + 4, &pssh.uuid, 16) < 16) {
                return ERROR_IO;
            }

            uint32_t psshdatalen = 0;
            if (mDataSource->readAt(data_offset + 20, &psshdatalen, 4) < 4) {
                return ERROR_IO;
            }
            pssh.datalen = ntohl(psshdatalen);
            ALOGV("pssh data size: %d", pssh.datalen);
            if (pssh.datalen + 20 > chunk_size) {
                // pssh data length exceeds size of containing box
                return ERROR_MALFORMED;
            }

            pssh.data = new (std::nothrow) uint8_t[pssh.datalen];
            if (pssh.data == NULL) {
                return ERROR_MALFORMED;
            }
            ALOGV("allocated pssh @ %p", pssh.data);
            ssize_t requested = (ssize_t) pssh.datalen;
            if (mDataSource->readAt(data_offset + 24, pssh.data, requested) < requested) {
                return ERROR_IO;
            }
            mPssh.push_back(pssh);

            break;
        }

        case FOURCC('m', 'd', 'h', 'd'):
        {
            *offset += chunk_size;

            if (chunk_data_size < 4) {
                return ERROR_MALFORMED;
            }

            uint8_t version;
            if (mDataSource->readAt(
                        data_offset, &version, sizeof(version))
                    < (ssize_t)sizeof(version)) {
                return ERROR_IO;
            }

            off64_t timescale_offset;

            if (version == 1) {
                timescale_offset = data_offset + 4 + 16;
            } else if (version == 0) {
                timescale_offset = data_offset + 4 + 8;
            } else {
                return ERROR_IO;
            }

            uint32_t timescale;
            if (mDataSource->readAt(
                        timescale_offset, &timescale, sizeof(timescale))
                    < (ssize_t)sizeof(timescale)) {
                return ERROR_IO;
            }

            mLastTrack->timescale = ntohl(timescale);

            // 14496-12 says all ones means indeterminate, but some files seem to use
            // 0 instead. We treat both the same.
            int64_t duration = 0;
            if (version == 1) {
                if (mDataSource->readAt(
                            timescale_offset + 4, &duration, sizeof(duration))
                        < (ssize_t)sizeof(duration)) {
                    return ERROR_IO;
                }
                if (duration != -1) {
                    duration = ntoh64(duration);
                }
            } else {
                uint32_t duration32;
                if (mDataSource->readAt(
                            timescale_offset + 4, &duration32, sizeof(duration32))
                        < (ssize_t)sizeof(duration32)) {
                    return ERROR_IO;
                }
                if (duration32 != 0xffffffff) {
                    duration = ntohl(duration32);
                }
            }
            if (duration != 0) {
        		mLastTrack->trackInfo->mDuration = (duration * 1000000)
        				/ mLastTrack->timescale;
            }

            uint8_t lang[2];
            off64_t lang_offset;
            if (version == 1) {
                lang_offset = timescale_offset + 4 + 8;
            } else if (version == 0) {
                lang_offset = timescale_offset + 4 + 4;
            } else {
                return ERROR_IO;
            }

            if (mDataSource->readAt(lang_offset, &lang, sizeof(lang))
                    < (ssize_t)sizeof(lang)) {
                return ERROR_IO;
            }

            // To get the ISO-639-2/T three character language code
            // 1 bit pad followed by 3 5-bits characters. Each character
            // is packed as the difference between its ASCII value and 0x60.
            char lang_code[4];
            lang_code[0] = ((lang[0] >> 2) & 0x1f) + 0x60;
            lang_code[1] = ((lang[0] & 0x3) << 3 | (lang[1] >> 5)) + 0x60;
            lang_code[2] = (lang[1] & 0x1f) + 0x60;
            lang_code[3] = '\0';

    		mLastTrack->trackInfo->mLangCode = strdup(lang_code);

            break;
        }

        case FOURCC('s', 't', 's', 'd'):
        {
            if (chunk_data_size < 8) {
                return ERROR_MALFORMED;
            }

            uint8_t buffer[8];
            if (chunk_data_size < (off64_t)sizeof(buffer)) {
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, 8) < 8) {
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) {
                // Should be version 0, flags 0.
                return ERROR_MALFORMED;
            }

            uint32_t entry_count = U32_AT(&buffer[4]);

            if (entry_count > 1) {
                // For 3GPP timed text, there could be multiple tx3g boxes contain
                // multiple text display formats. These formats will be used to
                // display the timed text.
                // For encrypted files, there may also be more than one entry.
    			const char *mime = mLastTrack->trackInfo->mMIME;
                if (strcasecmp(mime, MEDIA_MIMETYPE_TEXT_3GPP) &&
                        strcasecmp(mime, "application/octet-stream")) {
                    // For now we only support a single type of media per track.
                    mLastTrack->skipTrack = true;
                    *offset += chunk_size;
                    break;
                }
            }
            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + 8;
            for (uint32_t i = 0; i < entry_count; ++i) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('m', 'p', '4', 'a'):
        case FOURCC('e', 'n', 'c', 'a'):
        case FOURCC('s', 'a', 'm', 'r'):
        case FOURCC('s', 'a', 'w', 'b'):
        {
            uint8_t buffer[8 + 20];
            if (chunk_data_size < (ssize_t)sizeof(buffer)) {
                // Basic AudioSampleEntry size.
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, sizeof(buffer)) < (ssize_t)sizeof(buffer)) {
                return ERROR_IO;
            }

            uint16_t data_ref_index = U16_AT(&buffer[6]);
            uint32_t num_channels = U16_AT(&buffer[16]);

            uint16_t sample_size = U16_AT(&buffer[18]);
            uint32_t sample_rate = U32_AT(&buffer[24]) >> 16;

            if (chunk_type != FOURCC('e', 'n', 'c', 'a')) {
                // if the chunk type is enca, we'll get the type from the sinf/frma box later
    			mLastTrack->trackInfo->mMIME = FourCC2MIME(chunk_type);
                AdjustChannelsAndRate(chunk_type, &num_channels, &sample_rate);
            }
            ALOGV("*** coding='%s' %d channels, size %d, rate %d\n",
                   chunk, num_channels, sample_size, sample_rate);
    		mLastTrack->trackInfo->mChannelCount = num_channels;
    		mLastTrack->trackInfo->mSampleRate = sample_rate;

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + sizeof(buffer);
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('m', 'p', '4', 'v'):
        case FOURCC('e', 'n', 'c', 'v'):
        case FOURCC('s', '2', '6', '3'):
        case FOURCC('H', '2', '6', '3'):
        case FOURCC('h', '2', '6', '3'):
        case FOURCC('a', 'v', 'c', '1'):
        case FOURCC('h', 'v', 'c', '1'):
        case FOURCC('h', 'e', 'v', '1'):
        {
            mHasVideo = true;

            uint8_t buffer[78];
            if (chunk_data_size < (ssize_t)sizeof(buffer)) {
                // Basic VideoSampleEntry size.
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, sizeof(buffer)) < (ssize_t)sizeof(buffer)) {
                return ERROR_IO;
            }

            uint16_t data_ref_index = U16_AT(&buffer[6]);
            uint16_t width = U16_AT(&buffer[6 + 18]);
            uint16_t height = U16_AT(&buffer[6 + 20]);

            // The video sample is not standard-compliant if it has invalid dimension.
            // Use some default width and height value, and
            // let the decoder figure out the actual width and height (and thus
            // be prepared for INFO_FOMRAT_CHANGED event).
            if (width == 0)  width  = 352;
            if (height == 0) height = 288;

            // printf("*** coding='%s' width=%d height=%d\n",
            //        chunk, width, height);

            if (chunk_type != FOURCC('e', 'n', 'c', 'v')) {
                // if the chunk type is encv, we'll get the type from the sinf/frma box later
    			mLastTrack->trackInfo->mMIME = FourCC2MIME(chunk_type);
    		}
    		mLastTrack->trackInfo->mWidth = width;
    		mLastTrack->trackInfo->mHeight = height;

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + sizeof(buffer);
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('s', 't', 'c', 'o'):
        case FOURCC('c', 'o', '6', '4'):
        {
            status_t err =
                mLastTrack->sampleTable->setChunkOffsetParams(
                        chunk_type, data_offset, chunk_data_size);

            *offset += chunk_size;

            if (err != OK) {
                return err;
            }

            break;
        }

        case FOURCC('s', 't', 's', 'c'):
        {
            status_t err =
                mLastTrack->sampleTable->setSampleToChunkParams(
                        data_offset, chunk_data_size);

            *offset += chunk_size;

            if (err != OK) {
                return err;
            }

            break;
        }

        case FOURCC('s', 't', 's', 'z'):
        case FOURCC('s', 't', 'z', '2'):
        {
            status_t err =
                mLastTrack->sampleTable->setSampleSizeParams(
                        chunk_type, data_offset, chunk_data_size);

            *offset += chunk_size;

            if (err != OK) {
                return err;
            }

            size_t max_size;
            err = mLastTrack->sampleTable->getMaxSampleSize(&max_size);

            if (err != OK) {
                return err;
            }

            if (max_size != 0) {
                // Assume that a given buffer only contains at most 10 chunks,
                // each chunk originally prefixed with a 2 byte length will
                // have a 4 byte header (0x00 0x00 0x00 0x01) after conversion,
                // and thus will grow by 2 bytes per chunk.
    			mLastTrack->trackInfo->mMaxInputSize = max_size + 10 * 2;
            } else {
                // No size was specified. Pick a conservatively large size.
                int32_t width, height;
    			if (mLastTrack->trackInfo->mWidth > 0
    					&& mLastTrack->trackInfo->mHeight > 0) {
    				width = mLastTrack->trackInfo->mWidth;
    				height = mLastTrack->trackInfo->mHeight;
    			} else {
                    ALOGE("No width or height, assuming worst case 1080p");
                    width = 1920;
                    height = 1080;
    			}

    			const char *mime = mLastTrack->trackInfo->mMIME;
                if (!strcmp(mime, MEDIA_MIMETYPE_VIDEO_AVC)) {
                    // AVC requires compression ratio of at least 2, and uses
                    // macroblocks
                    max_size = ((width + 15) / 16) * ((height + 15) / 16) * 192;
                } else {
                    // For all other formats there is no minimum compression
                    // ratio. Use compression ratio of 1.
                    max_size = width * height * 3 / 2;
                }
    			mLastTrack->trackInfo->mMaxInputSize = max_size + 10 * 2;
            }

            // NOTE: setting another piece of metadata invalidates any pointers (such as the
            // mimetype) previously obtained, so don't cache them.

			const char *mime = mLastTrack->trackInfo->mMIME;
            // Calculate average frame rate.
            if (!strncasecmp("video/", mime, 6)) {
                size_t nSamples = mLastTrack->sampleTable->countSamples();
    			int64_t durationUs = mLastTrack->trackInfo->mDuration;
                if (durationUs > 0) {
                	int32_t frameRate = (nSamples * 1000000LL +
                			(durationUs >> 1)) / durationUs;
                	mLastTrack->trackInfo->mFrameRate = frameRate;
                }
            }

            break;
        }

        case FOURCC('s', 't', 't', 's'):
        {
            *offset += chunk_size;

            status_t err =
                mLastTrack->sampleTable->setTimeToSampleParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            break;
        }

        case FOURCC('c', 't', 't', 's'):
        {
            *offset += chunk_size;

            status_t err =
                mLastTrack->sampleTable->setCompositionTimeToSampleParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            break;
        }

        case FOURCC('s', 't', 's', 's'):
        {
            *offset += chunk_size;

            status_t err =
                mLastTrack->sampleTable->setSyncSampleParams(
                        data_offset, chunk_data_size);

            if (err != OK) {
                return err;
            }

            break;
        }

        // @xyz
        case FOURCC('\xA9', 'x', 'y', 'z'):
        {
            *offset += chunk_size;

            // Best case the total data length inside "@xyz" box
            // would be 8, for instance "@xyz" + "\x00\x04\x15\xc7" + "0+0/",
            // where "\x00\x04" is the text string length with value = 4,
            // "\0x15\xc7" is the language code = en, and "0+0" is a
            // location (string) value with longitude = 0 and latitude = 0.
            if (chunk_data_size < 8) {
                return ERROR_MALFORMED;
            }

            // Worst case the location string length would be 18,
            // for instance +90.0000-180.0000, without the trailing "/" and
            // the string length + language code.
            char buffer[18];

            // Substracting 5 from the data size is because the text string length +
            // language code takes 4 bytes, and the trailing slash "/" takes 1 byte.
            off64_t location_length = chunk_data_size - 5;
            if (location_length >= (off64_t) sizeof(buffer)) {
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset + 4, buffer, location_length) < location_length) {
                return ERROR_IO;
            }

            buffer[location_length] = '\0';
    		mFileMetaData->mMetaData[kKeyLocation] = new Buffer(buffer, location_length + 1, true);
    		mFileMetaData->mMetaData[kKeyLocation]->setRange(0, location_length + 1);
            break;
        }

        case FOURCC('e', 's', 'd', 's'):
        {
            *offset += chunk_size;

            if (chunk_data_size < 4) {
                return ERROR_MALFORMED;
            }

            uint8_t buffer[256];
            if (chunk_data_size > (off64_t)sizeof(buffer)) {
                return ERROR_BUFFER_TOO_SMALL;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) {
                // Should be version 0, flags 0.
                return ERROR_MALFORMED;
            }

    		mLastTrack->trackInfo->mESDS = new Buffer(chunk_data_size - 4);
    		memcpy(mLastTrack->trackInfo->mESDS->data(), &buffer[4],
    				chunk_data_size - 4);
            if (mPath.size() >= 2
                    && mPath[mPath.size() - 2] == FOURCC('m', 'p', '4', 'a')) {
                // Information from the ESDS must be relied on for proper
                // setup of sample rate and channel count for MPEG4 Audio.
                // The generic header appears to only contain generic
                // information...

                status_t err = updateAudioTrackInfoFromESDS_MPEG4Audio(
                        &buffer[4], chunk_data_size - 4);

                if (err != OK) {
                    return err;
                }
            }

            break;
        }

        case FOURCC('a', 'v', 'c', 'C'):
        {
            *offset += chunk_size;
    		CHECK(mLastTrack->trackInfo->mCodecSpecificData == NULL);
    		Buffer *buffer = new Buffer(chunk_data_size);

            if (mDataSource->readAt(
                        data_offset, buffer->data(), chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }

    		mLastTrack->trackInfo->mCodecSpecificData = buffer;

            break;
        }
        case FOURCC('h', 'v', 'c', 'C'):
        {
    		CHECK(mLastTrack->trackInfo->mCodecSpecificData == NULL);
			Buffer *buffer = new Buffer(chunk_data_size);

            if (mDataSource->readAt(
                        data_offset, buffer->data(), chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }
    		mLastTrack->trackInfo->mCodecSpecificData = buffer;

            *offset += chunk_size;
            break;
        }

        case FOURCC('d', '2', '6', '3'):
        {
            *offset += chunk_size;
            /*
             * d263 contains a fixed 7 bytes part:
             *   vendor - 4 bytes
             *   version - 1 byte
             *   level - 1 byte
             *   profile - 1 byte
             * optionally, "d263" box itself may contain a 16-byte
             * bit rate box (bitr)
             *   average bit rate - 4 bytes
             *   max bit rate - 4 bytes
             */
            if (chunk_data_size != 7 &&
                chunk_data_size != 23) {
                ALOGE("Incorrect D263 box size %"PRId64, chunk_data_size);
                return ERROR_MALFORMED;
            }
    		CHECK(mLastTrack->trackInfo->mCodecSpecificData == NULL);
    		Buffer *buffer = new Buffer(chunk_data_size);
            if (mDataSource->readAt(
                    data_offset, buffer, chunk_data_size) < chunk_data_size) {
                return ERROR_IO;
            }
    		mLastTrack->trackInfo->mCodecSpecificData = buffer;
            break;
        }

        case FOURCC('m', 'e', 't', 'a'):
        {
            uint8_t buffer[4];
            if (chunk_data_size < (off64_t)sizeof(buffer)) {
                *offset += chunk_size;
                return ERROR_MALFORMED;
            }

            if (mDataSource->readAt(
                        data_offset, buffer, 4) < 4) {
                *offset += chunk_size;
                return ERROR_IO;
            }

            if (U32_AT(buffer) != 0) {
                // Should be version 0, flags 0.

                // If it's not, let's assume this is one of those
                // apparently malformed chunks that don't have flags
                // and completely different semantics than what's
                // in the MPEG4 specs and skip it.
                *offset += chunk_size;
                return OK;
            }

            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset + sizeof(buffer);
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset, depth + 1);
                if (err != OK) {
                    return err;
                }
            }

            if (*offset != stop_offset) {
                return ERROR_MALFORMED;
            }
            break;
        }

        case FOURCC('m', 'e', 'a', 'n'):
        case FOURCC('n', 'a', 'm', 'e'):
        case FOURCC('d', 'a', 't', 'a'):
        {
            *offset += chunk_size;

            if (mPath.size() == 6 && underMetaDataPath(mPath)) {
                status_t err = parseITunesMetaData(data_offset, chunk_data_size);

                if (err != OK) {
                    return err;
                }
            }

            break;
        }

        case FOURCC('m', 'v', 'h', 'd'):
        {
            *offset += chunk_size;

            if (chunk_data_size < 32) {
                return ERROR_MALFORMED;
            }

            uint8_t header[32];
            if (mDataSource->readAt(
                        data_offset, header, sizeof(header))
                    < (ssize_t)sizeof(header)) {
                return ERROR_IO;
            }

            uint64_t creationTime;
            uint64_t duration = 0;
            if (header[0] == 1) {
                creationTime = U64_AT(&header[4]);
                mHeaderTimescale = U32_AT(&header[20]);
                duration = U64_AT(&header[24]);
                if (duration == 0xffffffffffffffff) {
                    duration = 0;
                }
            } else if (header[0] != 0) {
                return ERROR_MALFORMED;
            } else {
                creationTime = U32_AT(&header[4]);
                mHeaderTimescale = U32_AT(&header[12]);
                uint32_t d32 = U32_AT(&header[16]);
                if (d32 == 0xffffffff) {
                    d32 = 0;
                }
                duration = d32;
            }
            if (duration != 0) {
            	mFileMetaData->mDuration = duration * 1000000 / mHeaderTimescale;
            }

    		char *s;
    		convertTimeToDate(creationTime, &s);
    		mFileMetaData->mMetaData[kKeyDate] = new Buffer(s, strlen(s) + 1, true);
    		mFileMetaData->mMetaData[kKeyDate]->setRange(0, strlen(s) + 1);
    		ALOGV("time %s", s);
    		free(s);

            break;
        }

        case FOURCC('m', 'e', 'h', 'd'):
        {
            *offset += chunk_size;

            if (chunk_data_size < 8) {
                return ERROR_MALFORMED;
            }

            uint8_t flags[4];
            if (mDataSource->readAt(
                        data_offset, flags, sizeof(flags))
                    < (ssize_t)sizeof(flags)) {
                return ERROR_IO;
            }

            uint64_t duration = 0;
            if (flags[0] == 1) {
                // 64 bit
                if (chunk_data_size < 12) {
                    return ERROR_MALFORMED;
                }
                mDataSource->getUInt64(data_offset + 4, &duration);
                if (duration == 0xffffffffffffffff) {
                    duration = 0;
                }
            } else if (flags[0] == 0) {
                // 32 bit
                uint32_t d32;
                mDataSource->getUInt32(data_offset + 4, &d32);
                if (d32 == 0xffffffff) {
                    d32 = 0;
                }
                duration = d32;
            } else {
                return ERROR_MALFORMED;
            }

            if (duration != 0) {

            	mFileMetaData->mDuration = duration * 1000000 / mHeaderTimescale;
            }

            break;
        }

        case FOURCC('m', 'd', 'a', 't'):
        {
            ALOGV("mdat chunk, drm: %d", mIsDrm);
            if (!mIsDrm) {
                *offset += chunk_size;
                break;
            }

            if (chunk_size < 8) {
                return ERROR_MALFORMED;
            }

            return parseDrmSINF(offset, data_offset);
        }

        case FOURCC('h', 'd', 'l', 'r'):
        {
            *offset += chunk_size;

            uint32_t buffer;
            if (mDataSource->readAt(
                        data_offset + 8, &buffer, 4) < 4) {
                return ERROR_IO;
            }

            uint32_t type = ntohl(buffer);
            // For the 3GPP file format, the handler-type within the 'hdlr' box
            // shall be 'text'. We also want to support 'sbtl' handler type
            // for a practical reason as various MPEG4 containers use it.
            if (type == FOURCC('t', 'e', 'x', 't') || type == FOURCC('s', 'b', 't', 'l')) {
            	mLastTrack->trackInfo->mMIME = MEDIA_MIMETYPE_TEXT_3GPP;
            }

            break;
        }

        case FOURCC('t', 'r', 'e', 'x'):
        {
            *offset += chunk_size;

            if (chunk_data_size < 24) {
                return ERROR_IO;
            }
            uint32_t duration;
            Trex trex;
            if (!mDataSource->getUInt32(data_offset + 4, &trex.track_ID) ||
                !mDataSource->getUInt32(data_offset + 8, &trex.default_sample_description_index) ||
                !mDataSource->getUInt32(data_offset + 12, &trex.default_sample_duration) ||
                !mDataSource->getUInt32(data_offset + 16, &trex.default_sample_size) ||
                !mDataSource->getUInt32(data_offset + 20, &trex.default_sample_flags)) {
                return ERROR_IO;
            }
            mTrex.push_back(trex);
            break;
        }

        case FOURCC('t', 'x', '3', 'g'):
        {
//            uint32_t type;
//            const void *data;
//            size_t size = 0;
//            if (!mLastTrack->meta->findData(
//                    kKeyTextFormatData, &type, &data, &size)) {
//                size = 0;
//            }
//
//            uint8_t *buffer = new (std::nothrow) uint8_t[size + chunk_size];
//            if (buffer == NULL) {
//                return ERROR_MALFORMED;
//            }
//
//            if (size > 0) {
//                memcpy(buffer, data, size);
//            }
//
//            if ((size_t)(mDataSource->readAt(*offset, buffer + size, chunk_size))
//                    < chunk_size) {
//                delete[] buffer;
//                buffer = NULL;
//
//                // advance read pointer so we don't end up reading this again
//                *offset += chunk_size;
//                return ERROR_IO;
//            }
//
//            mLastTrack->meta->setData(
//                    kKeyTextFormatData, 0, buffer, size + chunk_size);
//
//            delete[] buffer;
//
//            *offset += chunk_size;
            break;
        }

        case FOURCC('c', 'o', 'v', 'r'):
        {
            *offset += chunk_size;

            if (mFileMetaData != NULL) {
            	ALOGV("chunk_data_size = %"PRId64" and data_offset = %"PRId64, chunk_data_size, data_offset);
                Buffer *buffer = new Buffer(chunk_data_size + 1);
                if (mDataSource->readAt(
                    data_offset, buffer->data(), chunk_data_size) != (ssize_t)chunk_data_size) {
                    return ERROR_IO;
                }
                const int kSkipBytesOfDataBox = 16;
    			mFileMetaData->mMetaData[kKeyAlbumArt] = buffer;
    			mFileMetaData->mMetaData[kKeyAlbumArt]->setRange(kSkipBytesOfDataBox, chunk_data_size - kSkipBytesOfDataBox);
    		}

            break;
        }

        case FOURCC('t', 'i', 't', 'l'):
        case FOURCC('p', 'e', 'r', 'f'):
        case FOURCC('a', 'u', 't', 'h'):
        case FOURCC('g', 'n', 'r', 'e'):
        case FOURCC('a', 'l', 'b', 'm'):
        case FOURCC('y', 'r', 'r', 'c'):
        {
            *offset += chunk_size;

            status_t err = parse3GPPMetaData(data_offset, chunk_data_size, depth);

            if (err != OK) {
                return err;
            }

            break;
        }

        case FOURCC('I', 'D', '3', '2'):
        {
            *offset += chunk_size;

            if (chunk_data_size < 6) {
                return ERROR_MALFORMED;
            }

            parseID3v2MetaData(data_offset + 6);

            break;
        }

        case FOURCC('-', '-', '-', '-'):
        {
    		mLastCommentMean = NULL;
    		mLastCommentName = NULL;
    		mLastCommentData = NULL;
            *offset += chunk_size;
            break;
        }

        case FOURCC('s', 'i', 'd', 'x'):
        {
            parseSegmentIndex(data_offset, chunk_data_size);
            *offset += chunk_size;
            return UNKNOWN_ERROR; // stop parsing after sidx
        }

        default:
        {
            *offset += chunk_size;
            break;
        }
    }

    return OK;
}

status_t MPEG4Extractor::parseSegmentIndex(off64_t offset, size_t size) {
  ALOGV("MPEG4Extractor::parseSegmentIndex");

    if (size < 12) {
      return -EINVAL;
    }

    uint32_t flags;
    if (!mDataSource->getUInt32(offset, &flags)) {
        return ERROR_MALFORMED;
    }

    uint32_t version = flags >> 24;
    flags &= 0xffffff;

    ALOGV("sidx version %d", version);

    uint32_t referenceId;
    if (!mDataSource->getUInt32(offset + 4, &referenceId)) {
        return ERROR_MALFORMED;
    }

    uint32_t timeScale;
    if (!mDataSource->getUInt32(offset + 8, &timeScale)) {
        return ERROR_MALFORMED;
    }
    ALOGV("sidx refid/timescale: %d/%d", referenceId, timeScale);

    uint64_t earliestPresentationTime;
    uint64_t firstOffset;

    offset += 12;
    size -= 12;

    if (version == 0) {
        if (size < 8) {
            return -EINVAL;
        }
        uint32_t tmp;
        if (!mDataSource->getUInt32(offset, &tmp)) {
            return ERROR_MALFORMED;
        }
        earliestPresentationTime = tmp;
        if (!mDataSource->getUInt32(offset + 4, &tmp)) {
            return ERROR_MALFORMED;
        }
        firstOffset = tmp;
        offset += 8;
        size -= 8;
    } else {
        if (size < 16) {
            return -EINVAL;
        }
        if (!mDataSource->getUInt64(offset, &earliestPresentationTime)) {
            return ERROR_MALFORMED;
        }
        if (!mDataSource->getUInt64(offset + 8, &firstOffset)) {
            return ERROR_MALFORMED;
        }
        offset += 16;
        size -= 16;
    }
    ALOGV("sidx pres/off: %" PRIu64 "/%" PRIu64, earliestPresentationTime, firstOffset);

    if (size < 4) {
        return -EINVAL;
    }

    uint16_t referenceCount;
    if (!mDataSource->getUInt16(offset + 2, &referenceCount)) {
        return ERROR_MALFORMED;
    }
    offset += 4;
    size -= 4;
    ALOGV("refcount: %d", referenceCount);

    if (size < referenceCount * 12) {
        return -EINVAL;
    }

    uint64_t total_duration = 0;
    for (unsigned int i = 0; i < referenceCount; i++) {
        uint32_t d1, d2, d3;

        if (!mDataSource->getUInt32(offset, &d1) ||     // size
            !mDataSource->getUInt32(offset + 4, &d2) || // duration
            !mDataSource->getUInt32(offset + 8, &d3)) { // flags
            return ERROR_MALFORMED;
        }

        if (d1 & 0x80000000) {
            ALOGW("sub-sidx boxes not supported yet");
        }
        bool sap = d3 & 0x80000000;
        uint32_t saptype = (d3 >> 28) & 7;
        if (!sap || (saptype != 1 && saptype != 2)) {
            // type 1 and 2 are sync samples
            ALOGW("not a stream access point, or unsupported type: %08x", d3);
        }
        total_duration += d2;
        offset += 12;
        ALOGV(" item %d, %08x %08x %08x", i, d1, d2, d3);
        SidxEntry se;
        se.mSize = d1 & 0x7fffffff;
		se.mDurationUs = 1000000LL * d2 / timeScale;
		mSidxEntries.push_back(se);
    }

    uint64_t sidxDuration = total_duration * 1000000 / timeScale;
	if (!mLastTrack->trackInfo->mDuration == 0) {
		mLastTrack->trackInfo->mDuration = sidxDuration;
	}
    return OK;
}

status_t MPEG4Extractor::parseTrackHeader(
        off64_t data_offset, off64_t data_size) {
    if (data_size < 4) {
        return ERROR_MALFORMED;
    }

    uint8_t version;
    if (mDataSource->readAt(data_offset, &version, 1) < 1) {
        return ERROR_IO;
    }

    size_t dynSize = (version == 1) ? 36 : 24;

    uint8_t buffer[36 + 60];

    if (data_size != (off64_t)dynSize + 60) {
        return ERROR_MALFORMED;
    }

    if (mDataSource->readAt(
                data_offset, buffer, data_size) < (ssize_t)data_size) {
        return ERROR_IO;
    }

    uint64_t ctime, mtime, duration;
    int32_t id;

    if (version == 1) {
        ctime = U64_AT(&buffer[4]);
        mtime = U64_AT(&buffer[12]);
        id = U32_AT(&buffer[20]);
        duration = U64_AT(&buffer[28]);
    } else if (version == 0) {
        ctime = U32_AT(&buffer[4]);
        mtime = U32_AT(&buffer[8]);
        id = U32_AT(&buffer[12]);
        duration = U32_AT(&buffer[20]);
    } else {
        return ERROR_UNSUPPORTED;
    }

	mLastTrack->trackInfo->mTrackId = id;

    size_t matrixOffset = dynSize + 16;
    int32_t a00 = U32_AT(&buffer[matrixOffset]);
    int32_t a01 = U32_AT(&buffer[matrixOffset + 4]);
    int32_t dx = U32_AT(&buffer[matrixOffset + 8]);
    int32_t a10 = U32_AT(&buffer[matrixOffset + 12]);
    int32_t a11 = U32_AT(&buffer[matrixOffset + 16]);
    int32_t dy = U32_AT(&buffer[matrixOffset + 20]);

#if 0
    ALOGI("x' = %.2f * x + %.2f * y + %.2f",
         a00 / 65536.0f, a01 / 65536.0f, dx / 65536.0f);
    ALOGI("y' = %.2f * x + %.2f * y + %.2f",
         a10 / 65536.0f, a11 / 65536.0f, dy / 65536.0f);
#endif

    uint32_t rotationDegrees;

    static const int32_t kFixedOne = 0x10000;
    if (a00 == kFixedOne && a01 == 0 && a10 == 0 && a11 == kFixedOne) {
        // Identity, no rotation
        rotationDegrees = 0;
    } else if (a00 == 0 && a01 == kFixedOne && a10 == -kFixedOne && a11 == 0) {
        rotationDegrees = 90;
    } else if (a00 == 0 && a01 == -kFixedOne && a10 == kFixedOne && a11 == 0) {
        rotationDegrees = 270;
    } else if (a00 == -kFixedOne && a01 == 0 && a10 == 0 && a11 == -kFixedOne) {
        rotationDegrees = 180;
    } else {
        ALOGW("We only support 0,90,180,270 degree rotation matrices");
        rotationDegrees = 0;
    }

	if (rotationDegrees != 0) {
		mLastTrack->trackInfo->mRotation = rotationDegrees;
    }

    // Handle presentation display size, which could be different
    // from the image size indicated by kKeyWidth and kKeyHeight.
    uint32_t width = U32_AT(&buffer[dynSize + 52]);
	uint32_t height = U32_AT(&buffer[dynSize + 56]);
	mLastTrack->trackInfo->mDisplayWidth = width >> 16;
	mLastTrack->trackInfo->mDisplayHeight = height >> 16;

    return OK;
}

status_t MPEG4Extractor::parseITunesMetaData(off64_t offset, size_t size) {
    if (size < 4) {
        return ERROR_MALFORMED;
    }

    uint8_t *buffer = new (std::nothrow) uint8_t[size + 1];
    if (buffer == NULL) {
        return ERROR_MALFORMED;
    }
    if (mDataSource->readAt(
                offset, buffer, size) != (ssize_t)size) {
        delete[] buffer;
        buffer = NULL;

        return ERROR_IO;
    }

    uint32_t flags = U32_AT(buffer);

    uint32_t metadataKey = 0;
    char chunk[5];
	MakeFourCCString(mPath[4], chunk);
	ALOGV("meta: %s @ %"PRId64, chunk, offset);
    switch (mPath[4]) {
        case FOURCC(0xa9, 'a', 'l', 'b'):
        {
            metadataKey = kKeyAlbum;
            break;
        }
        case FOURCC(0xa9, 'A', 'R', 'T'):
        {
            metadataKey = kKeyArtist;
            break;
        }
        case FOURCC('a', 'A', 'R', 'T'):
        {
            metadataKey = kKeyAlbumArtist;
            break;
        }
        case FOURCC(0xa9, 'd', 'a', 'y'):
        {
            metadataKey = kKeyYear;
            break;
        }
        case FOURCC(0xa9, 'n', 'a', 'm'):
        {
            metadataKey = kKeyTitle;
            break;
        }
        case FOURCC(0xa9, 'w', 'r', 't'):
        {
            metadataKey = kKeyWriter;
            break;
        }
        case FOURCC('c', 'o', 'v', 'r'):
        {
            metadataKey = kKeyAlbumArt;
            break;
        }
        case FOURCC('g', 'n', 'r', 'e'):
        {
            metadataKey = kKeyGenre;
            break;
        }
        case FOURCC(0xa9, 'g', 'e', 'n'):
        {
            metadataKey = kKeyGenre;
            break;
        }
        case FOURCC('c', 'p', 'i', 'l'):
        {
            if (size == 9 && flags == 21) {
                char tmp[16];
                sprintf(tmp, "%d",
                        (int)buffer[size - 1]);
    			mFileMetaData->mMetaData[kKeyComplilation] = new Buffer(tmp, strlen(tmp) + 1, true);
    			mFileMetaData->mMetaData[kKeyComplilation]->setRange(0, strlen(tmp) + 1);
            }
            break;
        }
        case FOURCC('t', 'r', 'k', 'n'):
        {
            if (size == 16 && flags == 0) {
                char tmp[16];
                uint16_t* pTrack = (uint16_t*)&buffer[10];
                uint16_t* pTotalTracks = (uint16_t*)&buffer[12];
                sprintf(tmp, "%d/%d", ntohs(*pTrack), ntohs(*pTotalTracks));

    			mFileMetaData->mMetaData[kKeyCDTrackNumber] = new Buffer(tmp, strlen(tmp) + 1, true);
    			mFileMetaData->mMetaData[kKeyCDTrackNumber]->setRange(0, strlen(tmp) + 1);
            }
            break;
        }
        case FOURCC('d', 'i', 's', 'k'):
        {
            if ((size == 14 || size == 16) && flags == 0) {
                char tmp[16];
                uint16_t* pDisc = (uint16_t*)&buffer[10];
                uint16_t* pTotalDiscs = (uint16_t*)&buffer[12];
                sprintf(tmp, "%d/%d", ntohs(*pDisc), ntohs(*pTotalDiscs));

    			mFileMetaData->mMetaData[kKeyDiscNumber] = new Buffer(tmp, strlen(tmp), true);
    			mFileMetaData->mMetaData[kKeyDiscNumber]->setRange(0, strlen(tmp) + 1);
            }
            break;
        }
        case FOURCC('-', '-', '-', '-'):
        {
            buffer[size] = '\0';
            switch (mPath[5]) {
                case FOURCC('m', 'e', 'a', 'n'):
					mLastCommentMean = strdup((const char *) buffer + 4);
					break;
				case FOURCC('n', 'a', 'm', 'e'):
					mLastCommentName = strdup((const char *) buffer + 4);
					break;
				case FOURCC('d', 'a', 't', 'a'):
					mLastCommentData = strdup((const char *) buffer + 8);
				break;
            }

            // Once we have a set of mean/name/data info, go ahead and process
            // it to see if its something we are interested in.  Whether or not
            // were are interested in the specific tag, make sure to clear out
            // the set so we can be ready to process another tuple should one
            // show up later in the file.
    		if ((strlen(mLastCommentMean) != 0) && (strlen(mLastCommentName) != 0)
    				&& (strlen(mLastCommentData) != 0)) {

    			if (strcmp(mLastCommentMean, "com.apple.iTunes")
    					&& strcmp(mLastCommentName, "iTunSMPB")) {
    				int32_t delay, padding;
    				if (sscanf(mLastCommentData, " %*x %x %x %*x", &delay, &padding)
    						== 2) {
    					mLastTrack->trackInfo->mEncoderDelay = delay;
    					mLastTrack->trackInfo->mEncoderPadding = padding;
    				}
    			}
    			free(mLastCommentMean);
    			free(mLastCommentName);
    			free(mLastCommentData);
    			mLastCommentMean = NULL;
    			mLastCommentName = NULL;
    			mLastCommentData = NULL;
    		}
            break;
        }

        default:
            break;
    }

    if (size >= 8 && metadataKey && !mFileMetaData->mMetaData[metadataKey]) {
        if (metadataKey == kKeyAlbumArt) {
			CHECK_EQ(mFileMetaData->mMetaData[kKeyAlbumArt], NULL);
			mFileMetaData->mMetaData[kKeyAlbumArt] = new Buffer(buffer + 8, size - 8, true);
			mFileMetaData->mMetaData[kKeyAlbumArt]->setRange(0, size - 8);
        } else if (metadataKey == kKeyGenre) {
            if (flags == 0) {
                // uint8_t genre code, iTunes genre codes are
                // the standard id3 codes, except they start
                // at 1 instead of 0 (e.g. Pop is 14, not 13)
                // We use standard id3 numbering, so subtract 1.
                int genrecode = (int)buffer[size - 1];
                genrecode--;
                if (genrecode < 0) {
                    genrecode = 255; // reserved for 'unknown genre'
                }
                char genre[10];
                sprintf(genre, "%d", genrecode);
				CHECK_EQ(mFileMetaData->mMetaData[kKeyAlbumArt], NULL);
				mFileMetaData->mMetaData[kKeyAlbumArt] = new Buffer(genre, strlen(genre) + 1, true);
				mFileMetaData->mMetaData[kKeyAlbumArt]->setRange(0, strlen(genre) + 1);
            } else if (flags == 1) {
                // custom genre string
                buffer[size] = '\0';

				CHECK_EQ(mFileMetaData->mMetaData[kKeyAlbumArt], NULL);
				mFileMetaData->mMetaData[kKeyAlbumArt] = new Buffer(buffer + 8, size - 8 + 1, true);
				mFileMetaData->mMetaData[kKeyAlbumArt]->setRange(0, size - 8 + 1);
            }
        } else {
            buffer[size] = '\0';
			CHECK_EQ(mFileMetaData->mMetaData[metadataKey], NULL);
			mFileMetaData->mMetaData[kKeyAlbumArt] = new Buffer((char *)buffer + 8, size - 8 + 1, true);
			mFileMetaData->mMetaData[kKeyAlbumArt]->setRange(0, size - 8 + 1);
        }
    }

    delete[] buffer;
    buffer = NULL;

    return OK;
}

status_t MPEG4Extractor::parse3GPPMetaData(off64_t offset, size_t size, int depth) {
	return OK;
}

void MPEG4Extractor::parseID3v2MetaData(off64_t offset) {

}

MediaSource* MPEG4Extractor::getTrack(size_t index) {
	ALOGV("getTrack");
	status_t err;
	if ((err = readMetaData()) != OK) {
		return NULL;
	}

    Track *track = mFirstTrack;
    while (index > 0) {
        if (track == NULL) {
            return NULL;
        }

        track = track->next;
        --index;
    }

    if (track == NULL) {
        return NULL;
    }


    Trex *trex = NULL;
    int32_t trackId = 0;
    if (track->trackInfo->mTrackId >= 0) {
        for (size_t i = 0; i < mTrex.size(); i++) {
            Trex *t = &mTrex[index];
            if (t->track_ID == (uint32_t) trackId) {
                trex = t;
                break;
            }
        }
    }

    ALOGV("getTrack called, pssh: %zu", mPssh.size());

    return new MPEG4Source(this,
    		track->trackInfo, mDataSource, track->timescale, track->sampleTable,
            mSidxEntries, trex, mMoofOffset);
}

// static
status_t MPEG4Extractor::verifyTrack(Track *track) {
	const char *mime = track->trackInfo->mMIME;
    uint32_t type;
    const void *data;
    size_t size;
    if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC)) {
        if (!track->trackInfo->mCodecSpecificData) {
            return ERROR_MALFORMED;
        }
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_HEVC)) {
        if (!track->trackInfo->mCodecSpecificData) {
            return ERROR_MALFORMED;
        }
    } else if (!strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_MPEG4)
            || !strcasecmp(mime, MEDIA_MIMETYPE_AUDIO_AAC)) {
        if (!track->trackInfo->mESDS) {
            return ERROR_MALFORMED;
        }
    }

    if (track->sampleTable == NULL || !track->sampleTable->isValid()) {
        // Make sure we have all the metadata we need.
        ALOGE("stbl atom missing/invalid.");
        return ERROR_MALFORMED;
    }

    return OK;
}

typedef enum {
    //AOT_NONE             = -1,
    //AOT_NULL_OBJECT      = 0,
    //AOT_AAC_MAIN         = 1, /**< Main profile                              */
    AOT_AAC_LC           = 2,   /**< Low Complexity object                     */
    //AOT_AAC_SSR          = 3,
    //AOT_AAC_LTP          = 4,
    AOT_SBR              = 5,
    //AOT_AAC_SCAL         = 6,
    //AOT_TWIN_VQ          = 7,
    //AOT_CELP             = 8,
    //AOT_HVXC             = 9,
    //AOT_RSVD_10          = 10, /**< (reserved)                                */
    //AOT_RSVD_11          = 11, /**< (reserved)                                */
    //AOT_TTSI             = 12, /**< TTSI Object                               */
    //AOT_MAIN_SYNTH       = 13, /**< Main Synthetic object                     */
    //AOT_WAV_TAB_SYNTH    = 14, /**< Wavetable Synthesis object                */
    //AOT_GEN_MIDI         = 15, /**< General MIDI object                       */
    //AOT_ALG_SYNTH_AUD_FX = 16, /**< Algorithmic Synthesis and Audio FX object */
    AOT_ER_AAC_LC        = 17,   /**< Error Resilient(ER) AAC Low Complexity    */
    //AOT_RSVD_18          = 18, /**< (reserved)                                */
    //AOT_ER_AAC_LTP       = 19, /**< Error Resilient(ER) AAC LTP object        */
    AOT_ER_AAC_SCAL      = 20,   /**< Error Resilient(ER) AAC Scalable object   */
    //AOT_ER_TWIN_VQ       = 21, /**< Error Resilient(ER) TwinVQ object         */
    AOT_ER_BSAC          = 22,   /**< Error Resilient(ER) BSAC object           */
    AOT_ER_AAC_LD        = 23,   /**< Error Resilient(ER) AAC LowDelay object   */
    //AOT_ER_CELP          = 24, /**< Error Resilient(ER) CELP object           */
    //AOT_ER_HVXC          = 25, /**< Error Resilient(ER) HVXC object           */
    //AOT_ER_HILN          = 26, /**< Error Resilient(ER) HILN object           */
    //AOT_ER_PARA          = 27, /**< Error Resilient(ER) Parametric object     */
    //AOT_RSVD_28          = 28, /**< might become SSC                          */
    AOT_PS               = 29,   /**< PS, Parametric Stereo (includes SBR)      */
    //AOT_MPEGS            = 30, /**< MPEG Surround                             */

    AOT_ESCAPE           = 31,   /**< Signal AOT uses more than 5 bits          */

    //AOT_MP3ONMP4_L1      = 32, /**< MPEG-Layer1 in mp4                        */
    //AOT_MP3ONMP4_L2      = 33, /**< MPEG-Layer2 in mp4                        */
    //AOT_MP3ONMP4_L3      = 34, /**< MPEG-Layer3 in mp4                        */
    //AOT_RSVD_35          = 35, /**< might become DST                          */
    //AOT_RSVD_36          = 36, /**< might become ALS                          */
    //AOT_AAC_SLS          = 37, /**< AAC + SLS                                 */
    //AOT_SLS              = 38, /**< SLS                                       */
    //AOT_ER_AAC_ELD       = 39, /**< AAC Enhanced Low Delay                    */

    //AOT_USAC             = 42, /**< USAC                                      */
    //AOT_SAOC             = 43, /**< SAOC                                      */
    //AOT_LD_MPEGS         = 44, /**< Low Delay MPEG Surround                   */

    //AOT_RSVD50           = 50,  /**< Interim AOT for Rsvd50                   */
} AUDIO_OBJECT_TYPE;

status_t MPEG4Extractor::updateAudioTrackInfoFromESDS_MPEG4Audio(
        const void *esds_data, size_t esds_size) {
    ESDS esds(esds_data, esds_size);

    uint8_t objectTypeIndication;
    if (esds.getObjectTypeIndication(&objectTypeIndication) != OK) {
        return ERROR_MALFORMED;
    }

	if (objectTypeIndication == 0xe1) {
		// This isn't MPEG4 audio at all, it's QCELP 14k...
		mLastTrack->trackInfo->mMIME = MEDIA_MIMETYPE_AUDIO_QCELP;
        return OK;
    }

    if (objectTypeIndication  == 0x6b) {
        // The media subtype is MP3 audio
        // Our software MP3 audio decoder may not be able to handle
        // packetized MP3 audio; for now, lets just return ERROR_UNSUPPORTED
        ALOGE("MP3 track in MP4/3GPP file is not supported");
        return ERROR_UNSUPPORTED;
    }

    const uint8_t *csd;
    size_t csd_size;
    if (esds.getCodecSpecificInfo(
                (const void **)&csd, &csd_size) != OK) {
        return ERROR_MALFORMED;
    }

#if 0
    printf("ESD of size %d\n", csd_size);
    hexdump(csd, csd_size);
#endif

    if (csd_size == 0) {
        // There's no further information, i.e. no codec specific data
        // Let's assume that the information provided in the mpeg4 headers
        // is accurate and hope for the best.

        return OK;
    }

    if (csd_size < 2) {
        return ERROR_MALFORMED;
    }

    static uint32_t kSamplingRate[] = {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000, 7350
    };

	BitReader br(csd, csd_size);
	uint32_t objectType = br.getBits(5);

    if (objectType == 31) {  // AAC-ELD => additional 6 bits
        objectType = 32 + br.getBits(6);
    }

    //keep AOT type
    mLastTrack->trackInfo->mObjectType = objectType;

    uint32_t freqIndex = br.getBits(4);

    int32_t sampleRate = 0;
    int32_t numChannels = 0;
    if (freqIndex == 15) {
        if (csd_size < 5) {
            return ERROR_MALFORMED;
        }
        sampleRate = br.getBits(24);
        numChannels = br.getBits(4);
    } else {
        numChannels = br.getBits(4);

        if (freqIndex == 13 || freqIndex == 14) {
            return ERROR_MALFORMED;
        }

        sampleRate = kSamplingRate[freqIndex];
    }

    if (objectType == AOT_SBR || objectType == AOT_PS) {//SBR specific config per 14496-3 table 1.13
        uint32_t extFreqIndex = br.getBits(4);
        int32_t extSampleRate;
        if (extFreqIndex == 15) {
            if (csd_size < 8) {
                return ERROR_MALFORMED;
            }
            extSampleRate = br.getBits(24);
        } else {
            if (extFreqIndex == 13 || extFreqIndex == 14) {
                return ERROR_MALFORMED;
            }
            extSampleRate = kSamplingRate[extFreqIndex];
        }
        //TODO: save the extension sampling rate value in meta data =>
        //      mLastTrack->meta->setInt32(kKeyExtSampleRate, extSampleRate);
    }

    switch (numChannels) {
        // values defined in 14496-3_2009 amendment-4 Table 1.19 - Channel Configuration
        case 0:
        case 1:// FC
        case 2:// FL FR
        case 3:// FC, FL FR
        case 4:// FC, FL FR, RC
        case 5:// FC, FL FR, SL SR
        case 6:// FC, FL FR, SL SR, LFE
            //numChannels already contains the right value
            break;
        case 11:// FC, FL FR, SL SR, RC, LFE
            numChannels = 7;
            break;
        case 7: // FC, FCL FCR, FL FR, SL SR, LFE
        case 12:// FC, FL  FR,  SL SR, RL RR, LFE
        case 14:// FC, FL  FR,  SL SR, LFE, FHL FHR
            numChannels = 8;
            break;
        default:
            return ERROR_UNSUPPORTED;
    }

    {
        if (objectType == AOT_SBR || objectType == AOT_PS) {
            objectType = br.getBits(5);

            if (objectType == AOT_ESCAPE) {
                objectType = 32 + br.getBits(6);
            }
        }
        if (objectType == AOT_AAC_LC || objectType == AOT_ER_AAC_LC ||
                objectType == AOT_ER_AAC_LD || objectType == AOT_ER_AAC_SCAL ||
                objectType == AOT_ER_BSAC) {
            const int32_t frameLengthFlag = br.getBits(1);

            const int32_t dependsOnCoreCoder = br.getBits(1);

            if (dependsOnCoreCoder ) {
                const int32_t coreCoderDelay = br.getBits(14);
            }

            int32_t extensionFlag = -1;
            if (br.numBitsLeft() > 0) {
                extensionFlag = br.getBits(1);
            } else {
                switch (objectType) {
                // 14496-3 4.5.1.1 extensionFlag
                case AOT_AAC_LC:
                    extensionFlag = 0;
                    break;
                case AOT_ER_AAC_LC:
                case AOT_ER_AAC_SCAL:
                case AOT_ER_BSAC:
                case AOT_ER_AAC_LD:
                    extensionFlag = 1;
                    break;
                default:
                    TRESPASS();
                    break;
                }
                ALOGW("csd missing extension flag; assuming %d for object type %u.",
                        extensionFlag, objectType);
            }

            if (numChannels == 0) {
                int32_t channelsEffectiveNum = 0;
                int32_t channelsNum = 0;
                const int32_t ElementInstanceTag = br.getBits(4);
                const int32_t Profile = br.getBits(2);
                const int32_t SamplingFrequencyIndex = br.getBits(4);
                const int32_t NumFrontChannelElements = br.getBits(4);
                const int32_t NumSideChannelElements = br.getBits(4);
                const int32_t NumBackChannelElements = br.getBits(4);
                const int32_t NumLfeChannelElements = br.getBits(2);
                const int32_t NumAssocDataElements = br.getBits(3);
                const int32_t NumValidCcElements = br.getBits(4);

                const int32_t MonoMixdownPresent = br.getBits(1);
                if (MonoMixdownPresent != 0) {
                    const int32_t MonoMixdownElementNumber = br.getBits(4);
                }

                const int32_t StereoMixdownPresent = br.getBits(1);
                if (StereoMixdownPresent != 0) {
                    const int32_t StereoMixdownElementNumber = br.getBits(4);
                }

                const int32_t MatrixMixdownIndexPresent = br.getBits(1);
                if (MatrixMixdownIndexPresent != 0) {
                    const int32_t MatrixMixdownIndex = br.getBits(2);
                    const int32_t PseudoSurroundEnable = br.getBits(1);
                }

                int i;
                for (i=0; i < NumFrontChannelElements; i++) {
                    const int32_t FrontElementIsCpe = br.getBits(1);
                    const int32_t FrontElementTagSelect = br.getBits(4);
                    channelsNum += FrontElementIsCpe ? 2 : 1;
                }

                for (i=0; i < NumSideChannelElements; i++) {
                    const int32_t SideElementIsCpe = br.getBits(1);
                    const int32_t SideElementTagSelect = br.getBits(4);
                    channelsNum += SideElementIsCpe ? 2 : 1;
                }

                for (i=0; i < NumBackChannelElements; i++) {
                    const int32_t BackElementIsCpe = br.getBits(1);
                    const int32_t BackElementTagSelect = br.getBits(4);
                    channelsNum += BackElementIsCpe ? 2 : 1;
                }
                channelsEffectiveNum = channelsNum;

                for (i=0; i < NumLfeChannelElements; i++) {
                    const int32_t LfeElementTagSelect = br.getBits(4);
                    channelsNum += 1;
                }
                ALOGV("mpeg4 audio channelsNum = %d", channelsNum);
                ALOGV("mpeg4 audio channelsEffectiveNum = %d", channelsEffectiveNum);
                numChannels = channelsNum;
            }
        }
    }

    if (numChannels == 0) {
        return ERROR_UNSUPPORTED;
    }

	int32_t prevSampleRate = mLastTrack->trackInfo->mSampleRate;
	if (prevSampleRate != sampleRate) {
        ALOGV("mpeg4 audio sample rate different from previous setting. "
             "was: %d, now: %d", prevSampleRate, sampleRate);
	}
	mLastTrack->trackInfo->mSampleRate = sampleRate;

	int32_t prevChannelCount = mLastTrack->trackInfo->mChannelCount;

    if (prevChannelCount != numChannels) {
        ALOGV("mpeg4 audio channel count different from previous setting. "
             "was: %d, now: %d", prevChannelCount, numChannels);
	}
	mLastTrack->trackInfo->mChannelCount = numChannels;

    return OK;
}

////////////////////////////////////////////////////////////////////////////////

MPEG4Source::MPEG4Source(
		MPEG4Extractor *owner,
		const TrackInfo *format,
		DataSource *dataSource,
        int32_t timeScale,
        SampleTable *sampleTable,
        vector<SidxEntry> &sidx,
        const Trex *trex,
        off64_t firstMoofOffset)
    : mOwner(owner),
      mFormat(format),
      mDataSource(dataSource),
      mTimescale(timeScale),
      mSampleTable(sampleTable),
      mCurrentSampleIndex(0),
      mCurrentFragmentIndex(0),
      mSegments(sidx),
      mTrex(trex),
      mFirstMoofOffset(firstMoofOffset),
      mCurrentMoofOffset(firstMoofOffset),
      mCurrentTime(0),
      mCurrentSampleInfoAllocSize(0),
      mCurrentSampleInfoSizes(NULL),
      mCurrentSampleInfoOffsetsAllocSize(0),
      mCurrentSampleInfoOffsets(NULL),
      mIsAVC(false),
      mIsHEVC(false),
      mNALLengthSize(0),
	 mStarted(false),
	 mWantsNALFragments(false),
	 mSrcBuffer(NULL) {
	ALOGI("MPEG4Source, this %p", this);
    memset(&mTrackFragmentHeaderInfo, 0, sizeof(mTrackFragmentHeaderInfo));

	mCryptoMode = mFormat->mCryptoMode;
	mDefaultIVSize = mFormat->mCryptoDefaultIVSize;
	if(mFormat->mCryptoTENCKey && mFormat->mCryptoTENCKey->size()) {
		memcpy(mCryptoKey, mFormat->mCryptoTENCKey->data(), mFormat->mCryptoTENCKey->size());
	}

	const char *mime = mFormat->mMIME;

    mIsAVC = !strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_AVC);
    mIsHEVC = !strcasecmp(mime, MEDIA_MIMETYPE_VIDEO_HEVC);

    if (mIsAVC) {
		const uint8_t *ptr = mFormat->mCodecSpecificData->data();
		size_t size = mFormat->mCodecSpecificData->size();

        CHECK(size >= 7);
        CHECK_EQ((unsigned)ptr[0], 1u);  // configurationVersion == 1

        // The number of bytes used to encode the length of a NAL unit.
        mNALLengthSize = 1 + (ptr[4] & 3);
    } else if (mIsHEVC) {
		const uint8_t *ptr = mFormat->mCodecSpecificData->data();
		size_t size = mFormat->mCodecSpecificData->size();

        CHECK(size >= 7);
        CHECK_EQ((unsigned)ptr[0], 1u);  // configurationVersion == 1

        mNALLengthSize = 1 + (ptr[14 + 7] & 3);
    }

	mTrackId = format->mTrackId;

    if (mFirstMoofOffset != 0) {
        off64_t offset = mFirstMoofOffset;
        parseChunk(&offset);
    }
}

MPEG4Source::~MPEG4Source() {
	ALOGV("~MPEG4Source, this %p", this);
	if (mStarted) {
		stop();
	}
	free(mCurrentSampleInfoSizes);
	free(mCurrentSampleInfoOffsets);
}

status_t MPEG4Source::start() {
	ALOGV("start");
	Mutex::Autolock autoLock(mLock);
	CHECK(!mStarted);

	mWantsNALFragments = false;

 	int32_t max_size = mFormat->mMaxInputSize;

    mSrcBuffer = new (std::nothrow) uint8_t[max_size];
    if (mSrcBuffer == NULL) {
        // file probably specified a bad max size
        return ERROR_MALFORMED;
    }

    mStarted = true;

    return OK;
}

status_t MPEG4Source::stop() {
	ALOGV("stop");
	Mutex::Autolock autoLock(mLock);

	CHECK(mStarted);

	delete[] mSrcBuffer;
	mSrcBuffer = NULL;

	mStarted = false;
	mCurrentSampleIndex = 0;

	return OK;
}

status_t MPEG4Source::parseChunk(off64_t *offset) {
    uint32_t hdr[2];
    if (mDataSource->readAt(*offset, hdr, 8) < 8) {
        return ERROR_IO;
    }
    uint64_t chunk_size = ntohl(hdr[0]);
    uint32_t chunk_type = ntohl(hdr[1]);
    off64_t data_offset = *offset + 8;

    if (chunk_size == 1) {
        if (mDataSource->readAt(*offset + 8, &chunk_size, 8) < 8) {
            return ERROR_IO;
        }
        chunk_size = ntoh64(chunk_size);
        data_offset += 8;

        if (chunk_size < 16) {
            // The smallest valid chunk is 16 bytes long in this case.
            return ERROR_MALFORMED;
        }
    } else if (chunk_size < 8) {
        // The smallest valid chunk is 8 bytes long.
        return ERROR_MALFORMED;
    }

	char chunk[5];
	MakeFourCCString(chunk_type, chunk);
	ALOGV("MPEG4Source chunk %s @ %"PRIx64, chunk, *offset);

    off64_t chunk_data_size = *offset + chunk_size - data_offset;

    switch(chunk_type) {

        case FOURCC('t', 'r', 'a', 'f'):
        case FOURCC('m', 'o', 'o', 'f'): {
            off64_t stop_offset = *offset + chunk_size;
            *offset = data_offset;
            while (*offset < stop_offset) {
                status_t err = parseChunk(offset);
                if (err != OK) {
                    return err;
                }
            }
            if (chunk_type == FOURCC('m', 'o', 'o', 'f')) {
                // *offset points to the box following this moof. Find the next moof from there.

                while (true) {
                    if (mDataSource->readAt(*offset, hdr, 8) < 8) {
                        return ERROR_END_OF_STREAM;
                    }
                    chunk_size = ntohl(hdr[0]);
                    chunk_type = ntohl(hdr[1]);
                    if (chunk_type == FOURCC('m', 'o', 'o', 'f')) {
                        mNextMoofOffset = *offset;
                        break;
                    }
                    *offset += chunk_size;
                }
            }
            break;
        }

        case FOURCC('t', 'f', 'h', 'd'): {
                status_t err;
                if ((err = parseTrackFragmentHeader(data_offset, chunk_data_size)) != OK) {
                    return err;
                }
                *offset += chunk_size;
                break;
        }

        case FOURCC('t', 'r', 'u', 'n'): {
                status_t err;
                if (mLastParsedTrackId == mTrackId) {
                    if ((err = parseTrackFragmentRun(data_offset, chunk_data_size)) != OK) {
                        return err;
                    }
                }

                *offset += chunk_size;
                break;
        }

        case FOURCC('s', 'a', 'i', 'z'): {
            status_t err;
            if ((err = parseSampleAuxiliaryInformationSizes(data_offset, chunk_data_size)) != OK) {
                return err;
            }
            *offset += chunk_size;
            break;
        }
        case FOURCC('s', 'a', 'i', 'o'): {
            status_t err;
            if ((err = parseSampleAuxiliaryInformationOffsets(data_offset, chunk_data_size)) != OK) {
                return err;
            }
            *offset += chunk_size;
            break;
        }

        case FOURCC('m', 'd', 'a', 't'): {
            // parse DRM info if present
            ALOGV("MPEG4Source::parseChunk mdat");
            // if saiz/saoi was previously observed, do something with the sampleinfos
            *offset += chunk_size;
            break;
        }

        default: {
            *offset += chunk_size;
            break;
        }
    }
    return OK;
}

status_t MPEG4Source::parseSampleAuxiliaryInformationSizes(
        off64_t offset, off64_t /* size */) {
    ALOGV("parseSampleAuxiliaryInformationSizes");
    // 14496-12 8.7.12
    uint8_t version;
    if (mDataSource->readAt(
            offset, &version, sizeof(version))
            < (ssize_t)sizeof(version)) {
        return ERROR_IO;
    }

    if (version != 0) {
        return ERROR_UNSUPPORTED;
    }
    offset++;

    uint32_t flags;
    if (!mDataSource->getUInt24(offset, &flags)) {
        return ERROR_IO;
    }
    offset += 3;

    if (flags & 1) {
        uint32_t tmp;
        if (!mDataSource->getUInt32(offset, &tmp)) {
            return ERROR_MALFORMED;
        }
        mCurrentAuxInfoType = tmp;
        offset += 4;
        if (!mDataSource->getUInt32(offset, &tmp)) {
            return ERROR_MALFORMED;
        }
        mCurrentAuxInfoTypeParameter = tmp;
        offset += 4;
    }

    uint8_t defsize;
    if (mDataSource->readAt(offset, &defsize, 1) != 1) {
        return ERROR_MALFORMED;
    }
    mCurrentDefaultSampleInfoSize = defsize;
    offset++;

    uint32_t smplcnt;
    if (!mDataSource->getUInt32(offset, &smplcnt)) {
        return ERROR_MALFORMED;
    }
    mCurrentSampleInfoCount = smplcnt;
    offset += 4;

    if (mCurrentDefaultSampleInfoSize != 0) {
        ALOGV("@@@@ using default sample info size of %d", mCurrentDefaultSampleInfoSize);
        return OK;
    }
    if (smplcnt > mCurrentSampleInfoAllocSize) {
        mCurrentSampleInfoSizes = (uint8_t*) realloc(mCurrentSampleInfoSizes, smplcnt);
        mCurrentSampleInfoAllocSize = smplcnt;
    }

    mDataSource->readAt(offset, mCurrentSampleInfoSizes, smplcnt);
    return OK;
}

status_t MPEG4Source::parseSampleAuxiliaryInformationOffsets(
        off64_t offset, off64_t /* size */) {
    ALOGV("parseSampleAuxiliaryInformationOffsets");
    // 14496-12 8.7.13
    uint8_t version;
    if (mDataSource->readAt(offset, &version, sizeof(version)) != 1) {
        return ERROR_IO;
    }
    offset++;

    uint32_t flags;
    if (!mDataSource->getUInt24(offset, &flags)) {
        return ERROR_IO;
    }
    offset += 3;

    uint32_t entrycount;
    if (!mDataSource->getUInt32(offset, &entrycount)) {
        return ERROR_IO;
    }
    offset += 4;

    if (entrycount > mCurrentSampleInfoOffsetsAllocSize) {
        mCurrentSampleInfoOffsets = (uint64_t*) realloc(mCurrentSampleInfoOffsets, entrycount * 8);
        mCurrentSampleInfoOffsetsAllocSize = entrycount;
    }
    mCurrentSampleInfoOffsetCount = entrycount;

    for (size_t i = 0; i < entrycount; i++) {
        if (version == 0) {
            uint32_t tmp;
            if (!mDataSource->getUInt32(offset, &tmp)) {
                return ERROR_IO;
            }
            mCurrentSampleInfoOffsets[i] = tmp;
            offset += 4;
        } else {
            uint64_t tmp;
            if (!mDataSource->getUInt64(offset, &tmp)) {
                return ERROR_IO;
            }
            mCurrentSampleInfoOffsets[i] = tmp;
            offset += 8;
        }
    }

    // parse clear/encrypted data

    off64_t drmoffset = mCurrentSampleInfoOffsets[0]; // from moof

	drmoffset += mCurrentMoofOffset;
	int ivlength = mFormat->mCryptoDefaultIVSize;

    // read CencSampleAuxiliaryDataFormats
	for (size_t i = 0; i < mCurrentSampleInfoCount; i++) {
		Sample *smpl = &mCurrentSamples[i];

        memset(smpl->iv, 0, 16);
        if (mDataSource->readAt(drmoffset, smpl->iv, ivlength) != ivlength) {
            return ERROR_IO;
        }

        drmoffset += ivlength;

        int32_t smplinfosize = mCurrentDefaultSampleInfoSize;
        if (smplinfosize == 0) {
            smplinfosize = mCurrentSampleInfoSizes[i];
        }
        if (smplinfosize > ivlength) {
            uint16_t numsubsamples;
            if (!mDataSource->getUInt16(drmoffset, &numsubsamples)) {
                return ERROR_IO;
            }
            drmoffset += 2;
            for (size_t j = 0; j < numsubsamples; j++) {
                uint16_t numclear;
                uint32_t numencrypted;
                if (!mDataSource->getUInt16(drmoffset, &numclear)) {
                    return ERROR_IO;
                }
                drmoffset += 2;
                if (!mDataSource->getUInt32(drmoffset, &numencrypted)) {
                    return ERROR_IO;
                }
                drmoffset += 4;
				smpl->clearsizes.push_back(numclear);
				smpl->encryptedsizes.push_back(numencrypted);
			}
		} else {
			smpl->clearsizes.push_back(0);
			smpl->encryptedsizes.push_back(smpl->size);
		}
    }


    return OK;
}

status_t MPEG4Source::parseTrackFragmentHeader(off64_t offset, off64_t size) {
	ALOGV("parseTrackFragmentHeader");

    if (size < 8) {
        return -EINVAL;
    }

    uint32_t flags;
    if (!mDataSource->getUInt32(offset, &flags)) { // actually version + flags
        return ERROR_MALFORMED;
    }

    if (flags & 0xff000000) {
        return -EINVAL;
    }

    if (!mDataSource->getUInt32(offset + 4, (uint32_t*)&mLastParsedTrackId)) {
        return ERROR_MALFORMED;
    }

    if (mLastParsedTrackId != mTrackId) {
        // this is not the right track, skip it
        return OK;
    }

    mTrackFragmentHeaderInfo.mFlags = flags;
    mTrackFragmentHeaderInfo.mTrackID = mLastParsedTrackId;
    offset += 8;
    size -= 8;

    ALOGV("fragment header: %08x %08x", flags, mTrackFragmentHeaderInfo.mTrackID);

    if (flags & TrackFragmentHeaderInfo::kBaseDataOffsetPresent) {
        if (size < 8) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt64(offset, &mTrackFragmentHeaderInfo.mBaseDataOffset)) {
            return ERROR_MALFORMED;
        }
        offset += 8;
        size -= 8;
    }

    if (flags & TrackFragmentHeaderInfo::kSampleDescriptionIndexPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mSampleDescriptionIndex)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (flags & TrackFragmentHeaderInfo::kDefaultSampleDurationPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mDefaultSampleDuration)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (flags & TrackFragmentHeaderInfo::kDefaultSampleSizePresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mDefaultSampleSize)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (flags & TrackFragmentHeaderInfo::kDefaultSampleFlagsPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &mTrackFragmentHeaderInfo.mDefaultSampleFlags)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    if (!(flags & TrackFragmentHeaderInfo::kBaseDataOffsetPresent)) {
        mTrackFragmentHeaderInfo.mBaseDataOffset = mCurrentMoofOffset;
    }

    mTrackFragmentHeaderInfo.mDataOffset = 0;
    return OK;
}

status_t MPEG4Source::parseTrackFragmentRun(off64_t offset, off64_t size) {

    ALOGV("MPEG4Extractor::parseTrackFragmentRun");
    if (size < 8) {
        return -EINVAL;
    }

    enum {
        kDataOffsetPresent                  = 0x01,
        kFirstSampleFlagsPresent            = 0x04,
        kSampleDurationPresent              = 0x100,
        kSampleSizePresent                  = 0x200,
        kSampleFlagsPresent                 = 0x400,
        kSampleCompositionTimeOffsetPresent = 0x800,
    };

    uint32_t flags;
    if (!mDataSource->getUInt32(offset, &flags)) {
        return ERROR_MALFORMED;
    }
    ALOGV("fragment run flags: %08x", flags);

    if (flags & 0xff000000) {
        return -EINVAL;
    }

    if ((flags & kFirstSampleFlagsPresent) && (flags & kSampleFlagsPresent)) {
        // These two shall not be used together.
        return -EINVAL;
    }

    uint32_t sampleCount;
    if (!mDataSource->getUInt32(offset + 4, &sampleCount)) {
        return ERROR_MALFORMED;
    }
    offset += 8;
    size -= 8;

    uint64_t dataOffset = mTrackFragmentHeaderInfo.mDataOffset;

    uint32_t firstSampleFlags = 0;

    if (flags & kDataOffsetPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        int32_t dataOffsetDelta;
        if (!mDataSource->getUInt32(offset, (uint32_t*)&dataOffsetDelta)) {
            return ERROR_MALFORMED;
        }

        dataOffset = mTrackFragmentHeaderInfo.mBaseDataOffset + dataOffsetDelta;

        offset += 4;
        size -= 4;
    }

    if (flags & kFirstSampleFlagsPresent) {
        if (size < 4) {
            return -EINVAL;
        }

        if (!mDataSource->getUInt32(offset, &firstSampleFlags)) {
            return ERROR_MALFORMED;
        }
        offset += 4;
        size -= 4;
    }

    uint32_t sampleDuration = 0, sampleSize = 0, sampleFlags = 0,
             sampleCtsOffset = 0;

    size_t bytesPerSample = 0;
    if (flags & kSampleDurationPresent) {
        bytesPerSample += 4;
    } else if (mTrackFragmentHeaderInfo.mFlags
            & TrackFragmentHeaderInfo::kDefaultSampleDurationPresent) {
        sampleDuration = mTrackFragmentHeaderInfo.mDefaultSampleDuration;
    } else if (mTrex) {
        sampleDuration = mTrex->default_sample_duration;
    }

    if (flags & kSampleSizePresent) {
        bytesPerSample += 4;
    } else if (mTrackFragmentHeaderInfo.mFlags
            & TrackFragmentHeaderInfo::kDefaultSampleSizePresent) {
        sampleSize = mTrackFragmentHeaderInfo.mDefaultSampleSize;
    } else {
        sampleSize = mTrackFragmentHeaderInfo.mDefaultSampleSize;
    }

    if (flags & kSampleFlagsPresent) {
        bytesPerSample += 4;
    } else if (mTrackFragmentHeaderInfo.mFlags
            & TrackFragmentHeaderInfo::kDefaultSampleFlagsPresent) {
        sampleFlags = mTrackFragmentHeaderInfo.mDefaultSampleFlags;
    } else {
        sampleFlags = mTrackFragmentHeaderInfo.mDefaultSampleFlags;
    }

    if (flags & kSampleCompositionTimeOffsetPresent) {
        bytesPerSample += 4;
    } else {
        sampleCtsOffset = 0;
    }

    if (size < (off64_t)sampleCount * bytesPerSample) {
        return -EINVAL;
    }

    Sample tmp;
    for (uint32_t i = 0; i < sampleCount; ++i) {
        if (flags & kSampleDurationPresent) {
            if (!mDataSource->getUInt32(offset, &sampleDuration)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        if (flags & kSampleSizePresent) {
            if (!mDataSource->getUInt32(offset, &sampleSize)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        if (flags & kSampleFlagsPresent) {
            if (!mDataSource->getUInt32(offset, &sampleFlags)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        if (flags & kSampleCompositionTimeOffsetPresent) {
            if (!mDataSource->getUInt32(offset, &sampleCtsOffset)) {
                return ERROR_MALFORMED;
            }
            offset += 4;
        }

        ALOGV("adding sample %d at offset 0x%08" PRIx64 ", size %u, duration %u, "
              " flags 0x%08x", i + 1,
                dataOffset, sampleSize, sampleDuration,
                (flags & kFirstSampleFlagsPresent) && i == 0
                    ? firstSampleFlags : sampleFlags);
        tmp.offset = dataOffset;
        tmp.size = sampleSize;
        tmp.duration = sampleDuration;
        tmp.compositionOffset = sampleCtsOffset;
		mCurrentSamples.push_back(tmp);

        dataOffset += sampleSize;
    }

    mTrackFragmentHeaderInfo.mDataOffset = dataOffset;

    return OK;
}

const TrackInfo *MPEG4Source::getFormat() {
	Mutex::Autolock autoLock(mLock);

    return mFormat;
}

size_t MPEG4Source::parseNALSize(const uint8_t *data) const {
    switch (mNALLengthSize) {
        case 1:
            return *data;
        case 2:
            return U16_AT(data);
        case 3:
            return ((size_t)data[0] << 16) | U16_AT(&data[1]);
        case 4:
            return U32_AT(data);
    }

    // This cannot happen, mNALLengthSize springs to life by adding 1 to
    // a 2-bit integer.
    CHECK(!"Should not be here.");

    return 0;
}

status_t MPEG4Source::read(
		AVPacket *packet, const ReadOptions *options) {
    Mutex::Autolock autoLock(mLock);

    CHECK(mStarted);

	if (mFirstMoofOffset > 0) {
		return fragmentedRead(packet, options);
	}

    int64_t targetSampleTimeUs = -1;

    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;
    if (options && options->getSeekTo(&seekTimeUs, &mode)) {
        uint32_t findFlags = 0;
        switch (mode) {
            case ReadOptions::SEEK_PREVIOUS_SYNC:
                findFlags = SampleTable::kFlagBefore;
                break;
            case ReadOptions::SEEK_NEXT_SYNC:
                findFlags = SampleTable::kFlagAfter;
                break;
            case ReadOptions::SEEK_CLOSEST_SYNC:
            case ReadOptions::SEEK_CLOSEST:
                findFlags = SampleTable::kFlagClosest;
                break;
            default:
                CHECK(!"Should not be here.");
                break;
        }

        uint32_t sampleIndex;
        status_t err = mSampleTable->findSampleAtTime(
                seekTimeUs, 1000000, mTimescale,
                &sampleIndex, findFlags);

        if (mode == ReadOptions::SEEK_CLOSEST) {
            // We found the closest sample already, now we want the sync
            // sample preceding it (or the sample itself of course), even
            // if the subsequent sync sample is closer.
            findFlags = SampleTable::kFlagBefore;
        }

        uint32_t syncSampleIndex;
        if (err == OK) {
            err = mSampleTable->findSyncSampleNear(
                    sampleIndex, &syncSampleIndex, findFlags);
        }

        uint32_t sampleTime;
        if (err == OK) {
            err = mSampleTable->getMetaDataForSample(
                    sampleIndex, NULL, NULL, &sampleTime);
        }

        if (err != OK) {
            if (err == ERROR_OUT_OF_RANGE) {
                // An attempt to seek past the end of the stream would
                // normally cause this ERROR_OUT_OF_RANGE error. Propagating
                // this all the way to the MediaPlayer would cause abnormal
                // termination. Legacy behaviour appears to be to behave as if
                // we had seeked to the end of stream, ending normally.
                err = ERROR_END_OF_STREAM;
            }
            ALOGV("end of stream");
            return err;
        }

        if (mode == ReadOptions::SEEK_CLOSEST) {
            targetSampleTimeUs = (sampleTime * 1000000ll) / mTimescale;
        }

#if 0
        uint32_t syncSampleTime;
        CHECK_EQ(OK, mSampleTable->getMetaDataForSample(
                    syncSampleIndex, NULL, NULL, &syncSampleTime));

		ALOGI("seek to time %"PRId64" us => sample at time %"PRId64" us, "
				"sync sample at time %"PRId64" us",
             seekTimeUs,
             sampleTime * 1000000ll / mTimescale,
             syncSampleTime * 1000000ll / mTimescale);
#endif

        mCurrentSampleIndex = syncSampleIndex;
        // fall through
    }

    off64_t offset;
    size_t size;
    uint32_t cts, stts;
    bool isSyncSample;
    status_t err = mSampleTable->getMetaDataForSample(
                  mCurrentSampleIndex, &offset, &size, &cts, &isSyncSample, &stts);
	if (err != OK) {
		return err;
	}

    if ((!mIsAVC && !mIsHEVC) || mWantsNALFragments) {
		CHECK(packet->mBuffer->capacity() >= size);
		ssize_t num_bytes_read = mDataSource->readAt(offset,
				(uint8_t *) packet->mBuffer->data(), size);

		if (num_bytes_read < (ssize_t) size) {
			packet->mBuffer->setRange(0, 0);
			return ERROR_IO;
		}

		packet->mBuffer->setRange(0, size);
		packet->mPts = ((int64_t) cts * 1000000) / mTimescale;
		packet->mDuration = ((int64_t)stts * 1000000) / mTimescale;
		if (targetSampleTimeUs >= 0) {
			packet->mTargetTime = targetSampleTimeUs;
		}

		if (isSyncSample) {
			packet->mIsSyncFrame = 1;
		} else {
			packet->mIsSyncFrame = 0;
		}

		++mCurrentSampleIndex;

        if (!mIsAVC && !mIsHEVC) {
            return OK;
		}
		TRESPASS();
        return OK;
    } else {
        // Whole NAL units are returned but each fragment is prefixed by
		// the start code (0x00 00 00 01).
		ssize_t num_bytes_read = 0;
		bool usesDRM = mFormat->mIsDrm;
        if (usesDRM) {
			num_bytes_read = mDataSource->readAt(offset,
					(uint8_t*) packet->mBuffer->data(), size);
        } else {
            num_bytes_read = mDataSource->readAt(offset, mSrcBuffer, size);
        }

		if (num_bytes_read < (ssize_t) size) {
			packet->mBuffer->setRange(0, 0);
			return ERROR_IO;
        }

		if (usesDRM) {
			packet->mBuffer->setRange(0, size);
		} else {
			uint8_t *dstData = (uint8_t *) packet->mBuffer->data();
            size_t srcOffset = 0;
            size_t dstOffset = 0;

            while (srcOffset < size) {
                bool isMalFormed = (srcOffset + mNALLengthSize > size);
                size_t nalLength = 0;
                if (!isMalFormed) {
                    nalLength = parseNALSize(&mSrcBuffer[srcOffset]);
                    srcOffset += mNALLengthSize;
                    isMalFormed = srcOffset + nalLength > size;
                }

				if (isMalFormed) {
					ALOGE("Video is malformed");
					packet->mBuffer->setRange(0, 0);
					return ERROR_MALFORMED;
				}

                if (nalLength == 0) {
					continue;
				}

				CHECK(dstOffset + 4 <= packet->mBuffer->capacity());

                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 1;
                memcpy(&dstData[dstOffset], &mSrcBuffer[srcOffset], nalLength);
                srcOffset += nalLength;
                dstOffset += nalLength;
			}
			CHECK_EQ(srcOffset, size);
			packet->mBuffer->setRange(0, dstOffset);
		}
		packet->mPts = ((int64_t) cts * 1000000) / mTimescale;
		packet->mDuration = ((int64_t)stts * 1000000) / mTimescale;
		if (targetSampleTimeUs >= 0) {
			packet->mTargetTime = targetSampleTimeUs;
		}

		if (isSyncSample) {
			packet->mIsSyncFrame = 1;
		} else {
			packet->mIsSyncFrame = 0;
		}

		++mCurrentSampleIndex;

		return OK;
    }
}

status_t MPEG4Source::fragmentedRead(AVPacket *packet,
		const ReadOptions *options) {
	ALOGV("MPEG4Source::fragmentedRead");

	CHECK(mStarted);

	int64_t targetSampleTimeUs = -1;

    int64_t seekTimeUs;
    ReadOptions::SeekMode mode;
    if (options && options->getSeekTo(&seekTimeUs, &mode)) {

        int numSidxEntries = mSegments.size();
        if (numSidxEntries != 0) {
            int64_t totalTime = 0;
            off64_t totalOffset = mFirstMoofOffset;
            for (int i = 0; i < numSidxEntries; i++) {
                const SidxEntry *se = &mSegments[i];
                if (totalTime + se->mDurationUs > seekTimeUs) {
                    // The requested time is somewhere in this segment
                    if ((mode == ReadOptions::SEEK_NEXT_SYNC && seekTimeUs > totalTime) ||
                        (mode == ReadOptions::SEEK_CLOSEST_SYNC &&
                        (seekTimeUs - totalTime) > (totalTime + se->mDurationUs - seekTimeUs))) {
                        // requested next sync, or closest sync and it was closer to the end of
                        // this segment
                        totalTime += se->mDurationUs;
                        totalOffset += se->mSize;
                    }
                    break;
                }
                totalTime += se->mDurationUs;
                totalOffset += se->mSize;
            }
            mCurrentMoofOffset = totalOffset;
            mCurrentSamples.clear();
            mCurrentSampleIndex = 0;
            parseChunk(&totalOffset);
            mCurrentTime = totalTime * mTimescale / 1000000ll;
        } else {
            // without sidx boxes, we can only seek to 0
            mCurrentMoofOffset = mFirstMoofOffset;
            mCurrentSamples.clear();
            mCurrentSampleIndex = 0;
            off64_t tmp = mCurrentMoofOffset;
            parseChunk(&tmp);
            mCurrentTime = 0;
        }
        // fall through
    }

    off64_t offset = 0;
    size_t size = 0;
    uint32_t cts = 0;
    bool isSyncSample = false;

	if (mCurrentSampleIndex >= mCurrentSamples.size()) {
		// move to next fragment if there is one
		if (mNextMoofOffset <= mCurrentMoofOffset) {
			return ERROR_END_OF_STREAM;
		}
		off64_t nextMoof = mNextMoofOffset;
		mCurrentMoofOffset = nextMoof;
		mCurrentSamples.clear();
		mCurrentSampleIndex = 0;
		parseChunk(&nextMoof);
		if (mCurrentSampleIndex >= mCurrentSamples.size()) {
			return ERROR_END_OF_STREAM;
		}
	}

	const Sample *smpl = &mCurrentSamples[mCurrentSampleIndex];
	offset = smpl->offset;
	size = smpl->size;
	cts = mCurrentTime + smpl->compositionOffset;
	mCurrentTime += smpl->duration;
	isSyncSample = (mCurrentSampleIndex == 0); // XXX

    if (smpl->encryptedsizes.size()) {
        // store clear/encrypted lengths in metadata
        // store clear/encrypted lengths in metadata
        packet->mClearSize = smpl->clearsizes;
        packet->mEncryptedSize = smpl->encryptedsizes;
        packet->mDefaultIVSize = mDefaultIVSize;
        packet->mCryptoMode = mCryptoMode;
        memcpy(packet->mCryptoIV, smpl->iv, 16);
        memcpy(packet->mCryptoKey, mCryptoKey, 16);
    }

	if ((!mIsAVC && !mIsHEVC) || mWantsNALFragments) {
		ssize_t num_bytes_read = mDataSource->readAt(offset,
				(uint8_t *)packet->mBuffer->data(), size);

		if (num_bytes_read < (ssize_t) size) {
			ALOGV("i/o error");
			return ERROR_IO;
		}

		packet->mBuffer->setRange(0, size);

		packet->mPts = ((int64_t) cts * 1000000) / mTimescale;
		packet->mDuration = ((int64_t)smpl->duration * 1000000) / mTimescale;
		if (targetSampleTimeUs >= 0) {
			packet->mTargetTime = targetSampleTimeUs;
		}

		if (isSyncSample) {
			packet->mIsSyncFrame = 1;
		} else {
			packet->mIsSyncFrame = 0;
		}

		++mCurrentSampleIndex;

		if (!mIsAVC && !mIsHEVC) {
			return OK;
		}

		TRESPASS();
		return OK;
	} else {
        ALOGV("whole NAL");
        // Whole NAL units are returned but each fragment is prefixed by
        // the start code (0x00 00 00 01).
		ssize_t num_bytes_read = 0;
		bool usesDRM = mFormat->mIsDrm;
		if (usesDRM) {
			num_bytes_read = mDataSource->readAt(offset,
					(uint8_t*) packet->mBuffer->data(), size);
        } else {
            num_bytes_read = mDataSource->readAt(offset, mSrcBuffer, size);
        }

		if (num_bytes_read < (ssize_t) size) {
			packet->mBuffer->setRange(0, 0);

			ALOGV("i/o error");
			return ERROR_IO;
		}

		if (usesDRM) {
			packet->mBuffer->setRange(0, size);
		} else {
			uint8_t *dstData = (uint8_t *) packet->mBuffer->data();
            size_t srcOffset = 0;
            size_t dstOffset = 0;

            while (srcOffset < size) {
                bool isMalFormed = (srcOffset + mNALLengthSize > size);
                size_t nalLength = 0;
                if (!isMalFormed) {
                    nalLength = parseNALSize(&mSrcBuffer[srcOffset]);
                    srcOffset += mNALLengthSize;
                    isMalFormed = srcOffset + nalLength > size;
                }

				if (isMalFormed) {
					ALOGE("Video is malformed");
					packet->mBuffer->setRange(0, 0);
					return ERROR_MALFORMED;
				}

                if (nalLength == 0) {
                    continue;
				}

				CHECK(dstOffset + 4 <= packet->mBuffer->capacity());

                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 0;
                dstData[dstOffset++] = 1;
                memcpy(&dstData[dstOffset], &mSrcBuffer[srcOffset], nalLength);
                srcOffset += nalLength;
                dstOffset += nalLength;
            }
			CHECK_EQ(srcOffset, size);
			packet->mBuffer->setRange(0, dstOffset);
		}

		packet->mPts = ((int64_t) cts * 1000000) / mTimescale;
		packet->mDuration = ((int64_t)smpl->duration * 1000000) / mTimescale;
		if (targetSampleTimeUs >= 0) {
			packet->mTargetTime = targetSampleTimeUs;
		}

		if (isSyncSample) {
			packet->mIsSyncFrame = 1;
		} else {
			packet->mIsSyncFrame = 0;
		}

        ++mCurrentSampleIndex;
        return OK;
    }
}

MPEG4Extractor::Track *MPEG4Extractor::findTrackByMimePrefix(
        const char *mimePrefix) {
	for (Track *track = mFirstTrack; track != NULL; track = track->next) {
		const char *mime = track->trackInfo->mMIME;
		if (track->trackInfo != NULL && !strncasecmp(mime, mimePrefix, strlen(mimePrefix))) {
			return track;
		}
	}

    return NULL;
}

static bool LegacySniffMPEG4(DataSource *source, const char **mimeType,
		float *confidence) {
	uint8_t header[8];

    ssize_t n = source->readAt(4, header, sizeof(header));
    if (n < (ssize_t)sizeof(header)) {
        return false;
    }

    if (!memcmp(header, "ftyp3gp", 7) || !memcmp(header, "ftypmp42", 8)
        || !memcmp(header, "ftyp3gr6", 8) || !memcmp(header, "ftyp3gs6", 8)
        || !memcmp(header, "ftyp3ge6", 8) || !memcmp(header, "ftyp3gg6", 8)
        || !memcmp(header, "ftypisom", 8) || !memcmp(header, "ftypM4V ", 8)
        || !memcmp(header, "ftypM4A ", 8) || !memcmp(header, "ftypf4v ", 8)
        || !memcmp(header, "ftypkddi", 8) || !memcmp(header, "ftypM4VP", 8)) {
        *mimeType = MEDIA_MIMETYPE_CONTAINER_MPEG4;
        *confidence = 0.4;

        return true;
    }

    return false;
}

static bool isCompatibleBrand(uint32_t fourcc) {
    static const uint32_t kCompatibleBrands[] = {
        FOURCC('i', 's', 'o', 'm'),
        FOURCC('i', 's', 'o', '2'),
        FOURCC('a', 'v', 'c', '1'),
        FOURCC('h', 'v', 'c', '1'),
        FOURCC('h', 'e', 'v', '1'),
        FOURCC('3', 'g', 'p', '4'),
        FOURCC('m', 'p', '4', '1'),
        FOURCC('m', 'p', '4', '2'),

        // Won't promise that the following file types can be played.
        // Just give these file types a chance.
        FOURCC('q', 't', ' ', ' '),  // Apple's QuickTime
        FOURCC('M', 'S', 'N', 'V'),  // Sony's PSP

        FOURCC('3', 'g', '2', 'a'),  // 3GPP2
        FOURCC('3', 'g', '2', 'b'),
    };

    for (size_t i = 0;
         i < sizeof(kCompatibleBrands) / sizeof(kCompatibleBrands[0]);
         ++i) {
        if (kCompatibleBrands[i] == fourcc) {
            return true;
        }
    }

    return false;
}

// Attempt to actually parse the 'ftyp' atom and determine if a suitable
// compatible brand is present.
// Also try to identify where this file's metadata ends
// (end of the 'moov' atom) and report it to the caller as part of
// the metadata.
static bool BetterSniffMPEG4(DataSource *source, const char **mimeType,
		float *confidence) {
	// We scan up to 128 bytes to identify this file as an MP4.
    static const off64_t kMaxScanOffset = 128ll;

    off64_t offset = 0ll;
    bool foundGoodFileType = false;
    off64_t moovAtomEndOffset = -1ll;
    bool done = false;

    while (!done && offset < kMaxScanOffset) {
        uint32_t hdr[2];
        if (source->readAt(offset, hdr, 8) < 8) {
            return false;
        }

        uint64_t chunkSize = ntohl(hdr[0]);
        uint32_t chunkType = ntohl(hdr[1]);
        off64_t chunkDataOffset = offset + 8;

        if (chunkSize == 1) {
            if (source->readAt(offset + 8, &chunkSize, 8) < 8) {
                return false;
            }

            chunkSize = ntoh64(chunkSize);
            chunkDataOffset += 8;

            if (chunkSize < 16) {
                // The smallest valid chunk is 16 bytes long in this case.
                return false;
            }
        } else if (chunkSize < 8) {
            // The smallest valid chunk is 8 bytes long.
            return false;
        }

        off64_t chunkDataSize = offset + chunkSize - chunkDataOffset;

        char chunkstring[5];
		MakeFourCCString(chunkType, chunkstring);
		ALOGV("saw chunk type %s, size %"PRId64" @ %"PRId64, chunkstring, chunkSize, offset);
        switch (chunkType) {
            case FOURCC('f', 't', 'y', 'p'):
            {
                if (chunkDataSize < 8) {
                    return false;
                }

                uint32_t numCompatibleBrands = (chunkDataSize - 8) / 4;
                for (size_t i = 0; i < numCompatibleBrands + 2; ++i) {
                    if (i == 1) {
                        // Skip this index, it refers to the minorVersion,
                        // not a brand.
                        continue;
                    }

                    uint32_t brand;
                    if (source->readAt(
                                chunkDataOffset + 4 * i, &brand, 4) < 4) {
                        return false;
                    }

                    brand = ntohl(brand);

                    if (isCompatibleBrand(brand)) {
                        foundGoodFileType = true;
                        break;
                    }
                }

                if (!foundGoodFileType) {
                    return false;
                }

                break;
            }

            case FOURCC('m', 'o', 'o', 'v'):
            {
                moovAtomEndOffset = offset + chunkSize;

                done = true;
                break;
            }

            default:
                break;
        }

        offset += chunkSize;
    }

    if (!foundGoodFileType) {
        return false;
    }

    *mimeType = MEDIA_MIMETYPE_CONTAINER_MPEG4;
    *confidence = 0.4f;

	return true;
}

bool SniffMPEG4(DataSource *source, const char **mimeType, float *confidence) {
	if (BetterSniffMPEG4(source, mimeType, confidence)) {
		return true;
	}

    if (LegacySniffMPEG4(source, mimeType, confidence)) {
        ALOGW("Identified supported mpeg4 through LegacySniffMPEG4.");
        return true;
    }

	return false;
}

} // namespace avformat
