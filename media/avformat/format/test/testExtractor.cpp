//#define LOG_NDEBUG 0
#define LOG_TAG "testExtractor"
#include <utils/Log.h>

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <utils/Utils.h>
#include <MediaInfo.h>
#include <DataSource.h>
#include <MediaSource.h>
#include <MediaExtractor.h>
#include <inttypes.h>
#include <vector>
using namespace std;
using namespace avformat;

//#define WRITE_HEADER

enum STREAM_TYPE {
	STREAM_AUDIO 	= 1 << 0,
	STREAM_VIDEO 	= 1 << 1,
	STREAM_SUBTITLE = 1 << 2,
};

enum STATUS {
	STATUS_IDLE = 0,
	STATUS_INITIALIZED,
	STATUS_STARTED,
	STATUS_STOPPED,
};

class TestExtractor {
public:
	TestExtractor();
	~TestExtractor();
	int32_t setDataSource(const char *filePath);
	int32_t init();
	int32_t start();
	int32_t read();
	int32_t stop();
	void dumpTrackInfo();
private:
	int32_t getOutputFileWithoutSuffix(char *dir);
	int32_t writeFrameLength(int32_t fd, int32_t length);
	struct Track {
		Track(MediaSource *source, int32_t streamType, int32_t trackId);
		~Track();
		int32_t mFd;
		bool mIsEof;
		int64_t mLastPts;
		MediaSource *mSource;
		int32_t mTrackId;
		int32_t mStreamType;
	private:
		Track(const Track&);
		Track &operator=(const Track&);
	};
	char *mUri;
	int32_t mStatus;
	list<Track*> mTracks;
	DataSource *mDataSource;
	MediaExtractor *mExtractor;
	TestExtractor(const TestExtractor&);
	TestExtractor &operator= (const TestExtractor&);
};

TestExtractor::Track::Track(MediaSource *source, int32_t streamType, int32_t trackId)
	:mFd(-1),
	 mIsEof(false),
	 mLastPts(0),
	 mSource(source),
	 mTrackId(trackId),
	 mStreamType(streamType) {
	ALOGV("Track, stream type %d", streamType);
}

TestExtractor::Track::~Track() {
	ALOGV("~Track()");
	if (mFd) {
		close(mFd);
	}
}

TestExtractor::TestExtractor()
	: mUri(NULL),
	  mStatus(STATUS_IDLE),
	  mDataSource(NULL),
	  mExtractor(NULL) {
	ALOGV("TestExtractor()");
}

TestExtractor::~TestExtractor(){
	ALOGV("~TestExtractor()");
	stop();
}

int32_t TestExtractor::setDataSource(const char *filePath) {
	ALOGD("setDataSource (%s)", filePath);
	mUri = strdup(filePath);
	return 0;
}

//get directory and basename.
int32_t TestExtractor::getOutputFileWithoutSuffix(char *dir) {
	ALOGV("buildOutputFiles");
	// get current working directory.
	char *cwd = getcwd(dir, 1024);
	if(cwd == NULL)
		return 0;

	// add a slash
	strncpy(dir + strlen(dir), "/data/", 6);
	if(access(dir, F_OK) != 0) {
		//directory doesn't exsist.
		ALOGI("directory doesn't exsist, create it");
		mkdir(dir, 0777);
	}

	// get file name position
	const char *file = strrchr(mUri, '/');
	if(file) { //path contains directory.
		file ++;//move to filename
	} else {
		file = mUri;
	}
	// get file basename
	const char *dot = strchr(file, '.');
	if(!dot)//There is no file extension.
		dot = file + strlen(file);

	// copy file name
	strncpy(dir + strlen(dir), file, dot - file);
	// add suffix

	return 0;
}

int32_t TestExtractor::writeFrameLength(int32_t fd, int32_t length) {
#ifdef WRITE_HEADER
	length = htonl(length);
	return write(fd, &length, 4);
#else
	return 0;
#endif
}

int32_t TestExtractor::init() {
	ALOGV("init");
	DataSource::RegisterDefaultSniffers();
	mDataSource = DataSource::CreateFromURI(mUri);
	if(mDataSource == NULL)
		return -1;

	mExtractor = MediaExtractor::Create(mDataSource);
	if(mExtractor == NULL)
		return -1;

	char suffix[12] = {0};
	char output[1024] = {0};

	MediaSource *mediaSource;
	for (size_t i = 0; i < mExtractor->countTracks(); i++) {
		TrackInfo *trackInfo = mExtractor->getTrackMetaData(i, 0);
		mediaSource = mExtractor->getTrack(i);
		int32_t streamType;
		if (!strncasecmp(trackInfo->mMIME, "video/", 6)) {
			streamType = STREAM_VIDEO;
			sprintf(suffix, "%02zd.video", i);
		} else if (!strncasecmp(trackInfo->mMIME, "audio/", 6)) {
			streamType = STREAM_AUDIO;
			sprintf(suffix, "%02zd.audio", i);
		} else {
			//discard others
			continue;
		}
		Track *track = new Track(mediaSource, streamType, i);
		mTracks.push_back(track);

		memset(output, '\0', sizeof(output));
		getOutputFileWithoutSuffix(output);
		strncpy(output + strlen(output), suffix, strlen(suffix));
		ALOGV("track[%d] output file %s", track->mTrackId, output);
		track->mFd = open(output, O_CREAT | O_TRUNC | O_RDWR, 0644);
		CHECK(track->mFd >= 0);

		if (track->mStreamType == STREAM_VIDEO) {
#ifdef WRITE_HEADER
			write(track->mFd, "h264", 4);
#endif
			if(trackInfo->mCodecSpecificData && trackInfo->mCodecSpecificData->size()) {
#ifdef WRITE_TO_FILE
				writeFrameLength(track->mFd, trackInfo->mCodecSpecificData->size());
				write(track->mFd, trackInfo->mCodecSpecificData->data(), trackInfo->mCodecSpecificData->size());;
#endif
			}
		} else if (track->mStreamType == STREAM_AUDIO) {
#ifdef WRITE_HEADER
			write(track->mFd, "aac", 3);
#endif
			if(trackInfo->mESDS && trackInfo->mESDS->size()) {
#ifdef WRITE_TO_FILE
				writeFrameLength(track->mFd, trackInfo->mESDS->size());
				write(track->mFd, trackInfo->mESDS->data(), trackInfo->mESDS->size());;
#endif
			}
		} else {
			CHECK(!"Should not be here");
		}
	}
	return 0;
}

int32_t TestExtractor::read() {
	ALOGV("start");
	AVPacket packet;
	packet.mBuffer = new Buffer(1920*1080*3/2);
	while(mStatus != STATUS_STOPPED) {
		Track *track = NULL;
		int64_t olderstPts = LLONG_MAX;
		for(list<Track *>::iterator it = mTracks.begin(); it != mTracks.end(); ++it) {
			if(((*it)->mLastPts < olderstPts)) {
				if((*it)->mIsEof) {
					continue;
				}
				track = *it;
				olderstPts = track->mLastPts;
			}
		}
		if(track == NULL) {
			//all track reach end of file.
			break;
		}
		int32_t err = track->mSource->read(&packet);
		if(err != 0) {
			ALOGE("track[%d] reaches end", track->mTrackId);
			track->mIsEof = true;
			continue;
		}
		if(track->mTrackId == 0 && (packet.mPts - track->mLastPts >45*1000ll)) {
			ALOGD("track[%d]: pts %"PRIu64", delta %"PRIu64", length %zd", track->mTrackId, packet.mPts,
					packet.mPts - track->mLastPts, packet.mBuffer->size());
		}
		writeFrameLength(track->mFd, packet.mBuffer->size());
#ifdef WRITE_TO_FILE
		ssize_t writeByes = write(track->mFd, packet.mBuffer->data(), packet.mBuffer->size()) ;
		if(writeByes != (ssize_t)packet.mBuffer->size()) {
			ALOGE("write file failed");
			track->mIsEof = true;
		}
#endif
		track->mLastPts = packet.mPts;
	}
	delete packet.mBuffer;
	return 0;
}

int32_t TestExtractor::start() {
	ALOGV("start");
	for(list<Track *>::iterator it = mTracks.begin(); it != mTracks.end(); ++it) {
		(*it)->mSource->start();
	}
	return 0;
}

int32_t TestExtractor::stop() {
	ALOGV("stop");
	while(!mTracks.empty()) {
		list<Track*>::iterator it = mTracks.begin();
		(*it)->mSource->stop();
		delete (*it);
		(*it) = NULL;
		mTracks.erase(it);
	}

	mTracks.clear();
	if(mDataSource) {
		delete mDataSource;
		mDataSource = NULL;
	}
	if(mExtractor) {
		delete mExtractor;
		mExtractor = NULL;
	}
	if(mUri) {
		free(mUri);
		mUri = NULL;
	}
	return 0;
}

void TestExtractor::dumpTrackInfo() {
	MediaInfo *mediaInfo = mExtractor->getMetaData();
	CHECK(mediaInfo != NULL);
	ALOGV("dump file information:");
	ALOGV("mime       :%s", mediaInfo->mMIME);
	ALOGV("track count:%zd", mExtractor->countTracks());

	for(size_t i = 0; i < mExtractor->countTracks(); i ++) {
		TrackInfo *trackInfo = mExtractor->getTrackMetaData(i, 0);
		ALOGV("dump track[%zd] information:", i);
		ALOGV("mime           :%s", trackInfo->mMIME);
		ALOGV("duration       :%"PRId64, trackInfo->mDuration);
		ALOGV("thumbnailTime  :%"PRId64, trackInfo->mThumbnailTime);
		ALOGV("language code  :%s", trackInfo->mLangCode);
		ALOGV("is drm         :%d", trackInfo->mIsDrm);
		ALOGV("crypto mode    :%u", trackInfo->mCryptoMode);
		ALOGV("default iv size:%u", trackInfo->mCryptoDefaultIVSize);
		if(trackInfo->mCryptoTENCKey) {
			hexdump("TENCKey", trackInfo->mCryptoTENCKey->data(), trackInfo->mCryptoTENCKey->size());
		}
		if(!strncasecmp(trackInfo->mMIME, "audio/", 6)) {
			ALOGV("channel count  :%u", trackInfo->mChannelCount);
			ALOGV("sample rate    :%u", trackInfo->mSampleRate);
			ALOGV("encoder delay  :%d", trackInfo->mEncoderDelay);
			ALOGV("encoder padding:%d", trackInfo->mEncoderPadding);
			if(trackInfo->mESDS) {
				hexdump("ESDS", trackInfo->mESDS->data(), trackInfo->mESDS->size());
			}
		} else if(!strncasecmp(trackInfo->mMIME, "video/", 6)) {
			ALOGV("width          :%u", trackInfo->mWidth);
			ALOGV("height         :%u", trackInfo->mHeight);
			ALOGV("frame rate     :%d", trackInfo->mFrameRate);
			ALOGV("display width  :%d", trackInfo->mDisplayWidth);
			ALOGV("display height :%u", trackInfo->mDisplayHeight);
			ALOGV("max input size :%zd", trackInfo->mMaxInputSize);
			if(trackInfo->mCodecSpecificData) {
				hexdump("CSD", trackInfo->mCodecSpecificData->data(), trackInfo->mCodecSpecificData->size());
			}
		} else {
			//TODO:subtitle.
		}
	}
	ALOGV("dump file information finished");
}

//////////////////////////////////////////////////////////////////////////

int32_t testDataSource(const char *filePath) {
	ALOGV("**************test Extractor start*************");
	uint32_t size = 512*1024;
	uint8_t *buffer = new uint8_t[size];
	DataSource::RegisterDefaultSniffers();

	DataSource *dataSource = DataSource::CreateFromURI(filePath);
	off64_t offset = 0;
	ssize_t readBytes = 0;
	while((readBytes = dataSource->readAt(offset, buffer, size)) > 0) {
		ALOGV("reading return %ld", readBytes);
		offset += readBytes;
	}

	delete dataSource;
	delete[] buffer;
	ALOGV("**************test Extractor finished**********");
	return 0;
}

/////////////////////////////////////////////////////////////////////////

int32_t main(int32_t argc, char *argv[]) {
	if (argc != 2) {
		ALOGE("Usage: %s path", argv[0]);
		exit(0);
	}
	TestExtractor *testExtractor = new TestExtractor();
	int32_t err = testExtractor->setDataSource(argv[1]);
	CHECK(err == 0);
	err = testExtractor->init();
	CHECK(err == 0);
	testExtractor->dumpTrackInfo();

	err = testExtractor->start();
	CHECK(err == 0);

	err = testExtractor->read();
	CHECK(err == 0);

	err = testExtractor->stop();
	CHECK(err == 0);
	delete testExtractor;
	return 0;
}
