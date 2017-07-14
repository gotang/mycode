
LOCAL_SRC:= \
	Trilateration.cpp \
	Mathematics.cpp \
	DeviceManager.cpp \
	TrilaterationTest.cpp \
	LinearLeastSquare.cpp \
	WoxuTest.cpp

LOCAL_OBJS:=$(addsuffix .o, $(basename $(LOCAL_SRC)))

LOCAL_MODULE:= tri_test

all: $(LOCAL_MODULE)

$(LOCAL_OBJS):%.o:%.cpp
	$(CXX) -o $@ -c $<

$(LOCAL_MODULE):$(LOCAL_OBJS)
	$(CXX) $^ -lgtest -lpthread -o $@
	
clean:
	rm -rf $(LOCAL_OBJS)
	rm -rf $(LOCAL_MODULE)
	rm -rf gtest-all.o
	
