	
LOCAL_C_INCLUDES:= \
	-I. \
	
LOCAL_OUT:=./build/out

LOCAL_CFLAGS:= \
	-Werror \
	-O0 \
	-g \
	-std=c++11 \


LOCAL_SRC_TESTC:= \
	test.c
LOCAL_OBJS_TESTC:=$(addprefix $(LOCAL_OUT)/, $(LOCAL_SRC_TESTC:.c=.c.o))
LOCAL_MODULE_TESTC:= test_c

LOCAL_SRC_TESTCPP:= \
	test.cpp
LOCAL_OBJS_TESTCPP:=$(addprefix $(LOCAL_OUT)/, $(LOCAL_SRC_TESTCPP:.cpp=.cpp.o))
LOCAL_MODULE_TESTCPP:= test_cpp

LOCAL_SRC_TEST2:= \
	test2.cpp
LOCAL_OBJS_TEST2:=$(addprefix $(LOCAL_OUT)/, $(LOCAL_SRC_TEST2:.cpp=.cpp.o))
LOCAL_MODULE_TEST2:= test2

LOCAL_TARGETS:= $(LOCAL_MODULE_TESTC) $(LOCAL_MODULE_TESTCPP) $(LOCAL_MODULE_TEST2)

################################################################################
	
all:$(LOCAL_TARGETS) sdl2

$(LOCAL_OUT)/%.c.o:%.c
	@mkdir -p $(@D)
	$(CXX) -o $@ $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -c $<

$(LOCAL_OUT)/%.cpp.o:%.cpp
	@mkdir -p $(@D)
	$(CXX) -o $@ $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -c $<

$(LOCAL_MODULE_TESTC):$(LOCAL_OBJS_TESTC)
	$(CXX) $^ -lgtest -lpthread -o $@
################################################################################	

$(LOCAL_MODULE_TESTCPP):$(LOCAL_OBJS_TESTCPP)
	$(CXX) $^ -lgtest -lpthread -o $@
	
################################################################################

$(LOCAL_MODULE_TEST2):$(LOCAL_OBJS_TEST2)
	$(CXX) $^ -lgtest -lpthread -o $@
	
	
sdl2:sdl2.cpp
	$(CXX) $^ -I/usr/include/SDL2 -lSDL2 -o $@
################################################################################
clean:
	rm -rf $(LOCAL_TARGETS)
	rm -rf $(LOCAL_MODULE_TESTC)
	rm -rf $(LOCAL_OBJS_TESTCPP)
	rm -rf $(LOCAL_OBJS_TEST2)
	rm -rf gtest-all.o
