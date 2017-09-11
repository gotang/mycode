LOCAL_PATH:=$(shell pwd)
include $(LOCAL_PATH)/../config.mak

LOCAL_OUT:=$(LOCAL_PATH)/../out
LOCAL_SRC:= \
	testExtractor.cpp

LOCAL_CFLAGS:=  \
	-Werror \
	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
	
LOCAL_CINCLUDES:= \
	-I$(LOCAL_PATH)/../include
	
LOCAL_OBJS:=$(addprefix $(LOCAL_OUT)/, $(addsuffix .o, $(basename $(LOCAL_SRC))))

LOCAL_MODULE:=$(LOCAL_OUT)/test_extractor

ifeq ($(BUILD_SHARED_LIBRARY), 1)
LOCAL_LIBRARIES:= \
	-lextractor -L$(LOCAL_OUT)
else	
LOCAL_LIBRARIES:= \
	$(LOCAL_OUT)/libextractor.a
endif

all:$(LOCAL_MODULE)
	rm $(LOCAL_OBJS) -rf

$(LOCAL_OUT)/%.o:%.cpp
	@mkdir -p $(@D)
	g++ -o $@ $(LOCAL_CFLAGS) $(LOCAL_CINCLUDES) -c $<

$(LOCAL_MODULE):$(LOCAL_OBJS)
	g++ -o $@ $^ $(LOCAL_LIBRARIES)
	
clean:
	rm $(LOCAL_MODULE) -rf
	rm $(LOCAL_OBJS) -rf
