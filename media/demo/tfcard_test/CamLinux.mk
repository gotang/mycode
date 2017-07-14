LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils

LOCAL_C_INCLUDES += \
		
LOCAL_SRC_FILES := \
	TFCardDemo.cpp \
	TFCardTest.cpp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := tfcard_test

include $(BUILD_EXECUTABLE)
