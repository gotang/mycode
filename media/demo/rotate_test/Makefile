LOCAL_PATH:=$(shell pwd)

LOCAL_SRC := \
	RotateTest.cpp \
	RotateDemo.cpp

LOCAL_CINCLUDES:= \
	-I$(LOCAL_PATH)
	
LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

LOCAL_MODULE:=rotate_test
	
all:$(LOCAL_MODULE)

$(LOCAL_OBJS):%.o:%.cpp
	gcc -o $@ $(LOCAL_CFLAGS) $(LOCAL_CINCLUDES) -c $<
	
$(LOCAL_MODULE):$(LOCAL_OBJS)
	g++ -o $@ $^ $(LOCAL_SHARED_LIBRARY)
	
clean:
	rm $(LOCAL_OBJS) -rf
	rm $(LOCAL_MODULE) -rf
