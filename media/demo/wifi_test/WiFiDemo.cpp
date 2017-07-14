#define LOG_NDEBUG 0
#define LOG_TAG "WiFiDemo"
#include <utils/Log.h>

#include <netutils/dhcp.h>
#include <hardware_legacy/wifi.h>
#include <hardware_legacy/softap.h>

#include "WiFiDemo.h"

WiFiDemo::WiFiDemo()
	:mWiFiID(-1),
	 mListener(NULL),
	 mErrorCode(0) {
	pthread_mutex_init(&mListenerLock, NULL);
}

WiFiDemo::~WiFiDemo() {
	pthread_mutex_destroy(&mListenerLock);
}

void WiFiDemo::dumpWiFiList() {
	for (int i = 0; i < mWiFiList.list_count; i++) {
		if (i == 0) {
			char header[32 + 32 + 6 + 6 + 63 + 2] = { 0 };
			snprintf(header, 32 + 32 + 6 + 6 + 63 + 2,
					"id|%-32s|%-32s|%-6s|%-6s|%-s",
					"bssid", "ssid", "freq", "level", "flags");
			ALOGV("%s", header);
			printf("%s\n", header);
		}
		ALOGV("%02d|%-32s|%-32s|%-6s|%-6s|%-s", i,
				mWiFiList.list[i].bssid, mWiFiList.list[i].ssid,
				mWiFiList.list[i].freq, mWiFiList.list[i].level,
				mWiFiList.list[i].flags);
		printf("%02d|%-32s|%-32s|%-6s|%-6s|%-s\n", i,
				mWiFiList.list[i].bssid, mWiFiList.list[i].ssid,
				mWiFiList.list[i].freq, mWiFiList.list[i].level,
				mWiFiList.list[i].flags);
	}
}

int WiFiDemo::onWiFiScanned() {
	ALOGV("onWiFiScanned");

	ALOGV("getWifiApList");
	int err = getWifiApList(&mWiFiList);
	ALOGV("getWifiApList return %d", err);

	dumpWiFiList();
	pthread_mutex_lock(&mListenerLock);
	if(mListener) {
		mListener->notify(WIFI_DEMO_SCAN_COMPLETED, 0);
	}
	pthread_mutex_unlock(&mListenerLock);
	return err;
}

static void wifi_callback(event_t event, int arg, void *cookie) {
	ALOGV("wifi_callback, event %d, arg %d", event, arg);

	static_cast<WiFiDemo*>(cookie)->onWiFiCallback(event, arg);
}

void WiFiDemo::onWiFiCallback(event_t event, int arg) {
	ALOGV("onWiFiCallback, event %d, arg %d", event, arg);
	switch(event) {
	case EVENT_SCAN_RESULT:
		onWiFiScanned();
		break;
	case EVENT_CONNECTED:
		break;
	case EVENT_DISCONNECTED:
		break;
	case EVENT_WRONG_PSK:
		break;
	case EVENT_DISCONNECTED_BY_USER:
		break;
	default:
		break;
	}
}

int WiFiDemo::scanWiFi() {
	ALOGV("scanWiFi");
	int err = android_wifi_enable();

	android_wifi_scan();

	wifi_setCallBack(wifi_callback, this);
	ALOGV("setConnectWifiEnable return %d", err);
	if(err != 1) {
		mErrorCode = ERROR_ENABLE_WIFI_FAILED;
		err = -1;
	} else {
		err = 0;
	}
	return err;
}


void WiFiDemo::getWiFiList(wifi_scanlist *wifiList) {
	memcpy(wifiList, &mWiFiList, sizeof(wifi_scanlist));
}

int WiFiDemo::connect(const char *ssid, const char *passwd, const char *type) {
	ALOGV("connect");

	if(strlen(ssid) <= 0 || strlen(passwd) < 8) {
		mErrorCode = ERROR_INVALID_SSID_OR_PASSWORD;
		return -1;
	}
	ALOGV("ssid is [%s], password is [%s], type is [%s]", ssid, passwd, type);
	mWiFiID = connectWifiAp(ssid, passwd, type);
	ALOGV("connectWifiAp, wifiID = %d", mWiFiID);

	int err = 0;
	if (mWiFiID != -1) {
		char ipstr[32];
		char gateway[32];
		uint32_t prefixLength;
		char dns1[32];
		char dns2[32];
		char server[32];
		uint32_t lease;
		char vendorInfo[32];
		err = dhcp_do_request("wlan0", ipstr, gateway, &prefixLength, dns1,
				dns2, server, &lease, vendorInfo);

		if (err < 0) {
			ALOGV("dhcp error result is %d, errmsg: %s", err, dhcp_get_errmsg());
			mErrorCode = ERROR_REQUEST_IP_FAILED;
		} else if (err == 0) {
			ALOGV("id:%s, gateway %s, dns1:%s, dns2:%s, server:%s, vendorInfo:%s",
					ipstr, gateway, dns1, dns2, server, vendorInfo);
			err = android_wifi_ping();
			if(err) {
				mErrorCode = ERROR_PING_FAILED;
			}
			ALOGV("android_wifi_ping return %d", err);
		}
	}
	return err;
}

int WiFiDemo::reset() {
	ALOGV("reset");

	disconnectAp(mWiFiID);
	android_wifi_disconnect();
	dhcp_release_lease("wlan0");
	android_wifi_disable();

	wifi_setCallBack(NULL, NULL);
	return 0;
}

int WiFiDemo::softAP() {
	int err = enableSoftAp();
	if(err != 0) {
		ALOGE("enableSoftAp failed");
	}
	ALOGV("enableSoftAp success");

	disableSoftAp();
	ALOGV("disableSoftAp finished");
	return err;
}

void WiFiDemo::setListener(WiFiDemoListener *listener) {
	pthread_mutex_lock(&mListenerLock);
	mListener = listener;
	pthread_mutex_unlock(&mListenerLock);
}

int WiFiDemo::getErrorCode() {
	return mErrorCode;
}

//static
const char *WiFiDemo::strerror(int error) {
	return NULL;
}
