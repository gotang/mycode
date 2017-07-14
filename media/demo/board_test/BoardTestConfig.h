#ifndef BOARD_TEST_CONFIG_H_
#define BOARD_TEST_CONFIG_H_

#define SCREEN_ROTATION 270

#define BOARD_TEST_CONFIG_FILE "/mnt/sdcard/board_test_config.xml"

#define DEFAULT_UVC_FILE ("/mnt/extsd/camera_uvc.mp4")
#define DEFAULT_CSI_FILE ("/mnt/extsd/camera_csi.mp4")
#define DEFAULT_PLAYER_FILE ("/mnt/extsd/player_test.mp4")
#define DEFAULT_MIC_FILE ("/mnt/extsd/mic_test.wav")
#define DEFAULT_SPEAKER_FILE ("/system/res/others/startup.wav")

enum TEST_ITEM {
	TEST_TF_CARD = 1<<0,
	TEST_CAMERA = 1<<1,
	TEST_PLAYER = 1<<2,
	TEST_MIC = 1<<3,
	TEST_SPEAKER = 1<<4,
	TEST_WIFI = 1<<5,
	TEST_SENSOR = 1<<6,
	TEST_RTC = 1<<7,
	TEST_MAX,
};
namespace android {
class BoardTestConfig {
public:
	BoardTestConfig();
	~BoardTestConfig();
private:
    enum Section {
        SECTION_TOPLEVEL,
        SECTION_DECODERS,
        SECTION_DECODER,
        SECTION_DECODER_TYPE,
        SECTION_ENCODERS,
        SECTION_ENCODER,
        SECTION_ENCODER_TYPE,
        SECTION_INCLUDE,
    };

    void parseXMLFile(const char *path);
    void parseTopLevelXMLFile(const char *path);

    static void StartElementHandlerWrapper(
            void *me, const char *name, const char **attrs);

    static void EndElementHandlerWrapper(void *me, const char *name);

    void startElementHandler(const char *name, const char **attrs);
    void endElementHandler(const char *name);
    Section mCurrentSection;
    int32_t mDepth;
    int32_t mInitCheck;
	int mScreenRotation;
	//for camera
	char *mUVCFilePath;
	char *mCSIFilePath;

	//player data source
	char *mPlayerPath;

	//MIC output file
	char *mMICFilePath;

	//Speaker data source
	char *mSpeakerFilePath;

	//WiFI
	char *mSSID;
	char *mPassword;

	//items that are supposed to be tested.
	int mItemFlags;
};
}
#endif //BOARD_TEST_CONFIG_H_
