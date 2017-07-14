#ifndef MEDIA_INFO_H_
#define MEDIA_INFO_H_
#include <vector>
using namespace std;
#include <MediaDefs.h>
#include <utils/Buffer.h>
namespace avformat {
enum METADATA_ITEM {
	kKeyStart = 0,
	kKeyAlbum = 0,//start from 0, we need to declare an array.
	kKeyArtist,
	kKeyAlbumArtist,
	kKeyYear,
	kKeyTitle,
	kKeyWriter,
	kKeyAlbumArt,
	kKeyGenre,
	kKeyDate,
	kKeyComplilation,
	kKeyCDTrackNumber,
	kKeyDiscNumber,
	kKeyPssh,
	kKeyTextFormatData,
	kKeyLocation,
	kKeyEnd,
};

struct TrackInfo {
public:
	TrackInfo()
	:mTrackId(-1),
	 mMIME(MEDIA_MIMETYPE_UNKOWN),
	 mDuration(0),
	 mThumbnailTime(0),
	 mChannelCount(0),
	 mSampleRate(0),
	 mESDS(NULL),
	 mEncoderDelay(0),
	 mEncoderPadding(0),
	 mObjectType(0),
	 mCryptoMode(0),
	 mCryptoDefaultIVSize(0),
	 mCryptoTENCKey(NULL),
	 mWidth(0),
	 mHeight(0),
	 mFrameRate(0),
	 mCodecSpecificData(NULL),
	 mDisplayWidth(0),
	 mDisplayHeight(0),
	 mRotation(0),
	 mMaxInputSize(0),
	 mLangCode(NULL),
	 mIsDrm(false) {

	}
	~TrackInfo() {
		if(mESDS)
			delete mESDS;
		if(mCryptoTENCKey)
			delete mCryptoTENCKey;
		if(mCodecSpecificData)
			delete mCodecSpecificData;
		if(mLangCode)
			free(mLangCode);
	}
	int32_t mTrackId;
	const char *mMIME;
	int64_t mDuration;
	int64_t mThumbnailTime;
	//audio
	uint32_t mChannelCount;
	uint32_t mSampleRate;
	Buffer *mESDS;
	int32_t mEncoderDelay;
	int32_t mEncoderPadding;
	uint32_t mObjectType;

	//Crypto information for cenc.
	uint32_t mCryptoMode;
	uint32_t mCryptoDefaultIVSize;
	//memory requires to be allocated.
	Buffer *mCryptoTENCKey;

	//video
	int32_t mWidth;
	int32_t mHeight;
	int32_t mFrameRate; //fps
	Buffer *mCodecSpecificData;
	int32_t mDisplayWidth;
	int32_t mDisplayHeight;
	int32_t mRotation;

	size_t mMaxInputSize;

	char *mLangCode;

	bool mIsDrm;
private:
	TrackInfo(const TrackInfo&);
	TrackInfo &operator=(const TrackInfo&);
};

struct MediaInfo {
	const char *mMIME;
	int64_t mDuration;
	//memory requires to be allocated.
	Buffer *mMetaData[kKeyEnd];
};

struct AVPacket {
	Buffer *mBuffer;
	int64_t mPts;
	int64_t mDuration;
	int64_t mTargetTime;
	bool mIsSyncFrame;
	//for cenc
	vector<size_t> mClearSize;
	vector<size_t> mEncryptedSize;
	//key same size with MPEG4Source::mCryptoKey
	uint8_t mCryptoIV[16];
	uint8_t mCryptoKey[16];
	int32_t mDefaultIVSize;
	int32_t mCryptoMode;
};

}//avformat
#endif
