#define LOG_NDEBUG 0
#define LOG_TAG "BoardTestConfig"
//#include "/home/sheldon/workspace/library/libmedia_hd/avformat/include/utils/Log.h"
#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include <libexpat/expat.h>
#include "BoardTestConfig.h"
namespace android {
BoardTestConfig::BoardTestConfig() {
	parseXMLFile("/mnt/sdcard/media_codecs.xml");
}

BoardTestConfig::~BoardTestConfig() {

}

void BoardTestConfig::parseXMLFile(const char *path) {
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        ALOGW("unable to open media codecs configuration xml file: %s", path);
    	mScreenRotation = SCREEN_ROTATION;
    	mUVCFilePath = strdup(DEFAULT_UVC_FILE);
    	mCSIFilePath = strdup(DEFAULT_CSI_FILE);
    	mPlayerPath = strdup(DEFAULT_PLAYER_FILE);
    	mMICFilePath = strdup(DEFAULT_MIC_FILE);
    	mSpeakerFilePath = strdup(DEFAULT_SPEAKER_FILE);
    	mItemFlags = TEST_MAX - 1 -1 ;
        mInitCheck = -1;
        return;
    }

    XML_Parser parser = ::XML_ParserCreate(NULL);
    assert(parser != NULL);

    ::XML_SetUserData(parser, this);
    ::XML_SetElementHandler(
            parser, StartElementHandlerWrapper, EndElementHandlerWrapper);

    const int BUFF_SIZE = 512;
    while (mInitCheck == 0) {
        void *buff = ::XML_GetBuffer(parser, BUFF_SIZE);
        if (buff == NULL) {
            ALOGE("failed in call to XML_GetBuffer()");
            mInitCheck =  -1;
            break;
        }

        int bytes_read = ::fread(buff, 1, BUFF_SIZE, file);
        if (bytes_read < 0) {
            ALOGE("failed in call to read");
            mInitCheck = -1;
            break;
        }

        XML_Status status = ::XML_ParseBuffer(parser, bytes_read, bytes_read == 0);
        if (status != XML_STATUS_OK) {
            ALOGE("malformed (%s)", ::XML_ErrorString(::XML_GetErrorCode(parser)));
            mInitCheck = -1;
            break;
        }

        if (bytes_read == 0) {
            break;
        }
    }

    ::XML_ParserFree(parser);

    fclose(file);
    file = NULL;
}

// static
void BoardTestConfig::StartElementHandlerWrapper(
        void *me, const char *name, const char **attrs) {
    static_cast<BoardTestConfig *>(me)->startElementHandler(name, attrs);
}

// static
void BoardTestConfig::EndElementHandlerWrapper(void *me, const char *name) {
    static_cast<BoardTestConfig *>(me)->endElementHandler(name);
}

int dumpAttrs(const char **attrs) {
	int i = 0;
    const char *name = NULL;
    const char *type = NULL;

    while (attrs[i] != NULL) {
    	ALOGV("attrs[%d]:%s:%s", i, attrs[i], attrs[i + 1]);
        if (!strcmp(attrs[i], "name")) {
            if (attrs[i + 1] == NULL) {
                return -EINVAL;
            }
            name = attrs[i + 1];
            ++i;
        } else if (!strcmp(attrs[i], "type")) {
            if (attrs[i + 1] == NULL) {
                return -EINVAL;
            }
            type = attrs[i + 1];
            ++i;
        } else {
            return -EINVAL;
        }

        ++i;
    }
    return 0;
}

void BoardTestConfig::startElementHandler(
        const char *name, const char **attrs) {
    if (mInitCheck != 0) {
        return;
    }

    bool inType = true;
    ALOGV("name:%s", name);
    dumpAttrs(attrs);
    ALOGV("//////////////////////");
    switch (mCurrentSection) {
        case SECTION_TOPLEVEL:
        {
            if (!strcmp(name, "Decoders")) {
                mCurrentSection = SECTION_DECODERS;
            } else if (!strcmp(name, "Encoders")) {
                mCurrentSection = SECTION_ENCODERS;
            }
            break;
        }

        case SECTION_DECODERS:
        {
            if (!strcmp(name, "MediaCodec")) {
                mCurrentSection = SECTION_DECODER;
            }
            break;
        }

        case SECTION_ENCODERS:
        {
            if (!strcmp(name, "MediaCodec")) {

                mCurrentSection = SECTION_ENCODER;
            }
            break;
        }

        case SECTION_DECODER:
        case SECTION_ENCODER:
        {
        	dumpAttrs(attrs);
            if (!strcmp(name, "Quirk")) {
            } else if (!strcmp(name, "Type")) {
                mCurrentSection =
                    (mCurrentSection == SECTION_DECODER
                            ? SECTION_DECODER_TYPE : SECTION_ENCODER_TYPE);
            }
        }
        inType = false;
        // fall through

        case SECTION_DECODER_TYPE:
        case SECTION_ENCODER_TYPE:
        {
            // ignore limits and features specified outside of type
            bool outside = !inType;
            if (outside && (!strcmp(name, "Limit") || !strcmp(name, "Feature"))) {
                ALOGW("ignoring %s specified outside of a Type", name);
            } else if (!strcmp(name, "Limit")) {
            } else if (!strcmp(name, "Feature")) {
            }
            break;
        }

        default:
            break;
    }

    ++mDepth;
}

void BoardTestConfig::endElementHandler(const char *name) {
    if (mInitCheck != 0) {
        return;
    }

    switch (mCurrentSection) {
        case SECTION_DECODERS:
        {
            if (!strcmp(name, "Decoders")) {
                mCurrentSection = SECTION_TOPLEVEL;
            }
            break;
        }

        case SECTION_ENCODERS:
        {
            if (!strcmp(name, "Encoders")) {
                mCurrentSection = SECTION_TOPLEVEL;
            }
            break;
        }

        case SECTION_DECODER_TYPE:
        case SECTION_ENCODER_TYPE:
        {
            if (!strcmp(name, "Type")) {
                mCurrentSection =
                    (mCurrentSection == SECTION_DECODER_TYPE
                            ? SECTION_DECODER : SECTION_ENCODER);

            }
            break;
        }

        case SECTION_DECODER:
        {
            if (!strcmp(name, "MediaCodec")) {
                mCurrentSection = SECTION_DECODERS;
            }
            break;
        }

        case SECTION_ENCODER:
        {
            if (!strcmp(name, "MediaCodec")) {
                mCurrentSection = SECTION_ENCODERS;
            }
            break;
        }


        default:
            break;
    }

    --mDepth;
}

}
