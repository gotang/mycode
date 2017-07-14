
ifeq ($(NO_LIBRARY), 1)

GTEST_DIR = ../googletest
CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra -pthread

GTEST_SRCS_ = $(GTEST_SRCS)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

LOCAL_SRC:= \
	foo.cpp

LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

LOCAL_MODULE:= foo_unittest

all: $(LOCAL_MODULE)

$(LOCAL_OBJS):%.o:%.cpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

gtest-all.o:$(GTEST_SRCS)
	$(CXX) -o $@ -I$(GTEST_DIR) $(CPPFLAGS) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

$(LOCAL_MODULE):$(LOCAL_OBJS)
	$(CXX) $^ -lpthread -o $@
	
else

LOCAL_SRC:= \
	foo.cpp

LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

LOCAL_MODULE:= gtest_test

all: $(LOCAL_MODULE)

$(LOCAL_OBJS):%.o:%.cpp
	$(CXX) -o $@ -c $<

$(LOCAL_MODULE):$(LOCAL_OBJS)
	$(CXX) $^ -lgtest -lpthread -o $@
	
clean:
	rm -rf $(LOCAL_OBJS)
	rm -rf $(LOCAL_MODULE)
	rm -rf gtest-all.o
	
endif