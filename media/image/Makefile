LOCAL_PATH:=$(shell pwd)

LOCAL_CFLAGS:= \
	-DCURRENT_WORK_DIR=\"$(LOCAL_PATH)\"
	
SDL_VERSION:=x

SDL_1:=$(shell pkg-config --modversion sdl 1>&2 2>/dev/null;echo $$?)
SDL_2:=$(shell pkg-config --modversion sdl2 >/dev/null 2>&1;echo $$?)

ifeq ($(SDL_2), 0)
SDL_VERSION=2
LOCAL_CFLAGS+= -DSDL2
else
ifeq ($(SDL_1), 0)
SDL_VERSION=
LOCAL_CFLAGS+= -DSDL1
else
$(error "SDL invalid")
endif
endif

LOCAL_SRC:= \
	./watermark/waterMarkWrapper.cpp \
	./watermark/waterMark.cpp \
	./src/ColorConverter.cpp \
	./src/RawPlayerSDL$(SDL_VERSION).cpp \
	./src/Bitmap.cpp \
	./main.cpp

LOCAL_SHARED_LIBRARY:= \

LOCAL_SHARED_LIBRARY+=$(shell pkg-config --libs sdl$(SDL_VERSION))


LOCAL_CINCLUDES:= \
	-I$(LOCAL_PATH) \
	-I$(LOCAL_PATH)/watermark \
	-I$(LOCAL_PATH)/include
	
LOCAL_CINCLUDES+=$(shell pkg-config --cflags sdl$(SDL_VERSION))

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
