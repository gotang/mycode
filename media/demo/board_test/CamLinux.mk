LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES :=  \
	BoardTestConfig.cpp \
	BoardTest.cpp

LOCAL_C_INCLUDES := \
	$(TOP)/frameworks/include/include_gui \
	$(TOP)/external/expat
	
LOCAL_SHARED_LIBRARIES := \
	libmedia_proxy \
	libcutils \
	libutils \
	libbinder \
	libexpat
	
LOCAL_LDFLAGS := \
	-L$(TOP)/frameworks/prebuilts \
	-lminigui_ths

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := board_test

include $(BUILD_EXECUTABLE)
