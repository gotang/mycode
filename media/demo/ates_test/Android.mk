LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	AtesTest.cpp
	
LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport \
	libCedarX \
	libcamrecorder \
	libdatabase
	
LOCAL_C_INCLUDES := \
	$(TOP)/bionic \
	$(TOP)/bionic/libstdc++/include \
	$(TOP)/external/stlport/stlport \
	$(TOP)/frameworks/include/include_database \
    $(TOP)/frameworks/av/media/CedarX-Projects \
    $(TOP)/frameworks/av/media/CedarX-Projects/CedarXAndroid/IceCreamSandwich \
    $(TOP)/frameworks/av/media/CedarX-Projects/CedarX/include/include_camera \
	$(TOP)/frameworks/av/media/CedarX-Projects/CedarX/include \
	$(TOP)/frameworks/av/media/CedarX-Projects/LIBRARY/CODEC/VIDEO/DECODER/include \
	$(TOP)/frameworks/av/media/CedarX-Projects/LIBRARY/CODEC/VIDEO/ENCODER/include \
	$(TOP)/frameworks/av/media/CedarX-Projects/LIBRARY/CODEC/AUDIO/DECODER/include \
	$(TOP)/frameworks/av/media/libcamrecorder/include
	
LOCAL_CFLAGS +=-DCEDARX_ANDROID_VERSION=9

ifeq ($(CEDARX_HW_DISPLAY),true)
  LOCAL_CFLAGS += -DCDXCFG_HW_DISPLAY=1 
else
  LOCAL_CFLAGS += -DCDXCFG_HW_DISPLAY=0 
endif

ifeq ($(USE_SDK_LIB), true)	
LOCAL_STATIC_LIBRARIES += \
	libol
else
LOCAL_LDFLAGS += \
	$(LOCAL_PATH)/libol.a
endif
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := ates_test

include $(BUILD_EXECUTABLE)

############################################
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	AtestbCheckMock.c

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libol
#include $(BUILD_STATIC_LIBRARY)
