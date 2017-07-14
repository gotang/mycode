LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
	$(TOP)/external/stlport/stlport \
	$(TOP)/external/jpeg \
	$(TOP)/frameworks/include/binder \
	$(TOP)/frameworks/include/include_gui \
	$(TOP)/frameworks/include/include_media/media \
	$(TOP)/frameworks/av/display \
	$(TOP)/frameworks/include \
	$(TOP)/frameworks/include/include_media/display \
	$(TOP)/frameworks/include/include_database \
	$(TOP)/frameworks/include/include_battery \
	$(TOP)/frameworks/include/include_storage \
	$(TOP)/frameworks/include/include_rtsp \
	$(TOP)/frameworks/include/include_http \
	$(TOP)/frameworks/include/standby \
	$(TOP)/frameworks/include/include_adas \
	$(TOP)/device/softwinner/common/hardware/camera/tvin \
	$(TOP)/device/softwinner/common/hardware/include

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils
	
LOCAL_LDFLAGS +=  \
	-L$(TOP)/frameworks/prebuilts \
	-lminigui_ths
	   
LOCAL_SRC_FILES :=  \
	ScreenTest.cpp \
	SubWindow.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := screen_test

include $(BUILD_EXECUTABLE)