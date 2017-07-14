LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := wifi_test
LOCAL_SRC_FILES := \
	WiFiDemo.cpp \
	WiFiTest.cpp

LOCAL_C_INCLUDES :=  \
	$(TOP)/system/core/include

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libhardware \
	libhardware_legacy \
	libnetutils

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

legacy_modules += wifi
