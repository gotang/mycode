LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	DisplayTest.cpp	

LOCAL_C_INCLUDES :=  \
    $(TOP)/frameworks/av/display \
	$(TOP)/frameworks/include/include_media \
	$(TOP)/frameworks/include/include_media/display \
	$(TOP)/frameworks/av/media/CedarX-Projects/LIBRARY/MEMORY/include

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libmedia \
	libMemAdapter \
	libhwdisp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := display_test
include $(BUILD_EXECUTABLE)

