LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	LEDTest.cpp	

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libmedia \
	libMemAdapter \
	libhwdisp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := led_test
include $(BUILD_EXECUTABLE)

