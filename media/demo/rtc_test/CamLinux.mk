LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_C_INCLUDES += \
		
LOCAL_SRC_FILES := \
	RTCDemo.cpp \
	RTCTest.cpp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := rtc_test

include $(BUILD_EXECUTABLE)
