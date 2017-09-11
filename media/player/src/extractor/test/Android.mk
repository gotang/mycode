LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport \
	libextractor
    		
LOCAL_SRC_FILES:= \
	testExtractor.cpp
	
LOCAL_C_INCLUDES:= \
	external/stlport/stlport \
	bionic/libc/include \
	$(LOCAL_PATH)/../include

LOCAL_CFLAGS:=-D__STDC_FORMAT_MACROS
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := test_extractor

include $(BUILD_EXECUTABLE)


##################################################
