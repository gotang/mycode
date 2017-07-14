#ifndef WIFI_DEMO_H_
#define WIFI_DEMO_H_

#include <hardware_legacy/wifi_api.h>

enum WIFI_DEMO_STATUS{
	OK = 0,
	ERROR_INVALID_SSID_OR_PASSWORD,
	ERROR_REQUEST_IP_FAILED,
	ERROR_PING_FAILED,
	ERROR_ENABLE_WIFI_FAILED
};

enum WIFI_DEMO_EVENT {
	WIFI_DEMO_ERROR,
	WIFI_DEMO_SCAN_COMPLETED,
};

class WiFiDemoListener {
public:
	virtual void notify(int msg, int ext) = 0;
	virtual ~WiFiDemoListener() {}
};

class WiFiDemo {
public:
	WiFiDemo();
	~WiFiDemo();
	int scanWiFi();
	int connect(const char *ssid, const char *passwd, const char *type);
	int reset();
	int softAP();

	void getWiFiList(wifi_scanlist *wifiList);
	void onWiFiCallback(event_t event, int arg);

	void setListener(WiFiDemoListener *listener);

	int getErrorCode();
	static const char *strerror(int error);

	void dumpWiFiList();
private:
	int onWiFiScanned();
	int mWiFiID;
	wifi_scanlist mWiFiList;

	WiFiDemoListener *mListener;
	pthread_mutex_t mListenerLock;

	int32_t mErrorCode;
};
#endif
