LOCAL_PATH:=$(shell pwd)

LOCAL_SRC:= \
	./watermark/waterMarkWrapper.cpp \
	./watermark/waterMark.cpp \
	./src/ColorConverter.cpp \
	./src/RawPlayer.cpp \
	./src/Bitmap.cpp \
	./main.cpp

LOCAL_SHARED_LIBRARY:= \

LOCAL_SHARED_LIBRARY+=$(shell pkg-config --libs sdl)

LOCAL_CINCLUDES:= \
	-I$(LOCAL_PATH) \
	-I$(LOCAL_PATH)/watermark \
	-I$(LOCAL_PATH)/include
	
LOCAL_CFLAGS:= \
	-DCURRENT_WORK_DIR=\"$(LOCAL_PATH)\"
	
LOCAL_CINCLUDES+=$(shell pkg-config --cflags sdl)

LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

LOCAL_MODULE:=raw_player
	
all:$(LOCAL_MODULE)

$(LOCAL_OBJS):%.o:%.cpp
	g++ -o $@ $(LOCAL_CFLAGS) $(LOCAL_CINCLUDES) -c $<
	
$(LOCAL_MODULE):$(LOCAL_OBJS)
	g++ -o $@ $^ $(LOCAL_SHARED_LIBRARY)
	
clean:
	rm $(LOCAL_OBJS) -rf
	rm $(LOCAL_MODULE) -rf
