LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libstlport

LOCAL_C_INCLUDES:= \
	bionic/libstdc++/include \
	external/stlport/stlport \
	bionic/libc/include \
	$(LOCAL_PATH)/../include
				
LOCAL_SRC_FILES:= \
	./utils/ESDS.cpp \
	./utils/Utils.cpp \
	./utils/Buffer.cpp \
	./utils/BitReader.cpp \
	./MediaDefs.cpp \
	./DataSource.cpp \
	./FileSource.cpp \
	./MediaSource.cpp \
	./MediaExtractor.cpp \
	./mpeg4/SampleTable.cpp \
	./mpeg4/SampleIterator.cpp \
	./mpeg4/MPEG4Extractor.cpp	
	
LOCAL_CFLAGS:=-D__STDC_FORMAT_MACROS
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libextractor

include $(BUILD_SHARED_LIBRARY)
