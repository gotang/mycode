LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libhardware

LOCAL_C_INCLUDES += \
		
LOCAL_SRC_FILES := \
	SensorDemo.cpp \
	SensorTest.cpp
	
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := sensor_test

include $(BUILD_EXECUTABLE)
