LOCAL_SRC_ALGORITHMS:= \
	main.cpp \
	CPlusPlus.cpp \
	NowCoder.cpp \
	ProjectEuler.cpp \
	NowCoderVerifier.cpp \
	LintCodeVerifier.cpp \
	LintCode.cpp \
	stl/Others.cpp \
	stl/Containers.cpp \
	stl/MultiThreading.cpp \
	LeetCode/LeetCodeDataStructure.cpp \
	LeetCode/LeetCodeAVerifier.cpp \
	LeetCode/LeetCodeA.cpp \
	LeetCode/LeetCodeBVerifier.cpp \
	LeetCode/LeetCodeB.cpp \
	interviews/BAT.cpp \
	interviews/BATVerifier.cpp \
	interviews/NetEase.cpp \
	interviews/NetEaseVerifier.cpp \

LOCAL_C_INCLUDES:= \
	-I. \

LOCAL_OUT:=./build/out

LOCAL_OBJS_ALGORITHMS:=$(addprefix $(LOCAL_OUT)/, $(LOCAL_SRC_ALGORITHMS:.cpp=.o))

LOCAL_MODULE_ALGORITHMS:= exercises

LOCAL_CFLAGS:= \
	-Werror \
	-O0 \
	-g \
	-std=c++11 \

################################################################################
	
all:$(LOCAL_MODULE_ALGORITHMS) 

$(LOCAL_OUT)/%.o:%.cpp
	@mkdir -p $(@D)
	$(CXX) -o $@ $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -c $<

$(LOCAL_MODULE_ALGORITHMS):$(LOCAL_OBJS_ALGORITHMS)
	$(CXX) $^ -lgtest -lpthread -o $@
	
clean:
	rm -rf $(LOCAL_OBJS_ALGORITHMS)
	rm -rf $(LOCAL_MODULE_ALGORITHMS)
	rm -rf gtest-all.o
