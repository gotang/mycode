#define LOG_NDEBUG 0
#define LOG_TAG "WiFiTest"
#include <utils/Log.h>

#include <stdlib.h>
#include "WiFiDemo.h"

class WiFiTest : public WiFiDemoListener {
public:
	WiFiTest();
	virtual ~WiFiTest();

	virtual void notify(int msg, int ext);

	int loop(bool wifi);
private:

	bool mStop;
	WiFiDemo *mWiFi;
	pthread_mutex_t mLock;
	pthread_cond_t mCond;
	bool mWiFiScanned;
};

WiFiTest::WiFiTest()
	:mWiFi(NULL),
	 mWiFiScanned(false) {
}

WiFiTest::~WiFiTest() {

}

void WiFiTest::notify(int msg, int ext) {
	ALOGV("notify msg %d", msg);
	if(msg == WIFI_DEMO_SCAN_COMPLETED) {
		pthread_mutex_lock(&mLock);
		mWiFiScanned = true;
		pthread_cond_signal(&mCond);
		pthread_mutex_unlock(&mLock);
	}
}

int WiFiTest::loop(bool wifi) {
	ALOGV("loop");
	mWiFi = new WiFiDemo();

	if(!wifi) {
		//Soft AP
		mWiFi->softAP();
	} else {
		mWiFi->setListener(this);
		int err = mWiFi->scanWiFi();
		if(err) {
			ALOGE("enable wifi failed");
			return -1;
		}
		ALOGV("waiting for scanning result...");
		pthread_mutex_lock(&mLock);
		while(!mWiFiScanned) {
			pthread_cond_wait(&mCond, &mLock);
		}
		pthread_mutex_unlock(&mLock);

		ALOGV("scanning finished");
		int id = -1;
		char input[3];
		wifi_scanlist wifiList;
		mWiFi->getWiFiList(&wifiList);
		while(id < 0 || id >= wifiList.list_count) {
			mWiFi->dumpWiFiList();
			printf("input wifi id:");
			fgets(input, 3, stdin);
			id = atoi(input);
		}
		char password[64] = {0};
		while(strlen(password) < 8) {
			printf("input password of wifi \"%s\":", wifiList.list[id].ssid);
			fgets(password, 64, stdin);
			password[strlen(password) - 1] = '\0';
		}
		const char *type = "WPA/WPA2";
		const char *ssid = wifiList.list[id].ssid;
		mWiFi->connect(ssid, password, type);

		mWiFi->reset();
	}

	delete mWiFi;
	return 0;
}

static void printUsage(const char *app) {
	printf("Usage: %s <-w | -a>\n", app);
}

int main(int argc, char** argv)
{
    int opt;
	if(argc < 2) {
        printUsage(argv[0]);
		return 0;
	}
	int type = 0;
    while ((opt = getopt(argc, argv, "wa")) != -1) {
        switch (opt) {
        case 'a':
        	type = 1;
            break;
        case 'w':
            break;
        default:
        	printUsage(argv[0]);
        	exit(0);
        }
    }
    WiFiTest *wifi = new WiFiTest();
	wifi->loop(!type);
    delete wifi;
	return 0;
}
