
LOCAL_SRC:= \
	 MockComplement.cc

LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

LOCAL_MODULE:= gmock_test

all: $(LOCAL_MODULE)

$(LOCAL_OBJS):%.o:%.cc
	$(CXX) -o $@ -c $<

$(LOCAL_MODULE):$(LOCAL_OBJS)
	$(CXX) $^ -lgmock -lpthread -o $@
	
clean:
	rm -rf $(LOCAL_OBJS)
	rm -rf $(LOCAL_MODULE)
	
