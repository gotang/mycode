LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libtinyalsa

LOCAL_C_INCLUDES += \
		
LOCAL_SRC_FILES := \
	SpeakerDemo.cpp \
	SpeakerTest.cpp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := speaker_test

include $(BUILD_EXECUTABLE)
