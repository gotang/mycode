LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libbinder \
	libmedia \
	libdisplay_client \
	libmedia_proxy \
	libcamera_client \
	libstagefright_foundation

LOCAL_C_INCLUDES += \
	$(TOP)/frameworks/av/include \
	$(TOP)/frameworks/include/include_media \
	$(TOP)/frameworks/include/include_media/media \
	$(TOP)/device/softwinner/common/hardware/include
		
LOCAL_SRC_FILES := \
	PlayerDemo.cpp \
	PlayerTest.cpp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := player_test

include $(BUILD_EXECUTABLE)
