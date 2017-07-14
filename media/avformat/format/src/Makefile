LOCAL_PATH:=$(shell pwd)
include $(LOCAL_PATH)/../config.mak

TARGET_DIR:=$(LOCAL_PATH)/../out

LOCAL_SRC:= \
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

LOCAL_CFLAGS:=  \
	-Werror \
	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
	
LOCAL_CINCLUDES:= \
	-I$(LOCAL_PATH)/../include
	
LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

ifeq ($(BUILD_SHARED_LIBRARY), 1)
LOCAL_MODULE:=$(TARGET_DIR)/libextractor.so
LOCAL_CFLAGS+= \
	-fPIC \
	-shared
else
LOCAL_MODULE:=$(TARGET_DIR)/libextractor.a
endif

all:$(LOCAL_MODULE)
	rm $(LOCAL_OBJS) -rf

$(LOCAL_OBJS):%.o:%.cpp
	g++ -o $@ $(LOCAL_CFLAGS) $(LOCAL_CINCLUDES) -c $<
	
ifeq ($(BUILD_SHARED_LIBRARY), 1)
$(LOCAL_MODULE):$(LOCAL_OBJS)
	g++ -o $@ $(LOCAL_CFLAGS) $^
else
$(LOCAL_MODULE):$(LOCAL_OBJS)
	ar rcs -o $@ $^
endif
		
clean:
	rm $(LOCAL_MODULE) -rf
	rm $(LOCAL_OBJS) -rf
