LOCAL_SRC_ALGORITHMS:= \
	main.cpp \
	STL.cpp \
	CPlusPlus.cpp \
	NowCoder.cpp \
	ProjectEuler.cpp \
	NowCoderVerifier.cpp \
	LintCodeVerifier.cpp \
	LintCode.cpp \
	LeetCode/LeetCodeDataStructure.cpp \
	LeetCode/LeetCodeAVerifier.cpp \
	LeetCode/LeetCodeA.cpp \
	LeetCode/LeetCodeBVerifier.cpp \
	LeetCode/LeetCodeB.cpp

LOCAL_C_INCLUDES:= \
	-I. \

LOCAL_OUT:=./build/out

LOCAL_OBJS_ALGORITHMS:=$(addprefix $(LOCAL_OUT)/, $(LOCAL_SRC_ALGORITHMS:.cpp=.o))

LOCAL_MODULE_ALGORITHMS:= exercises

################################################################################
	
all:$(LOCAL_MODULE_ALGORITHMS) 

$(LOCAL_OUT)/%.o:%.cpp
	@mkdir -p $(@D)
	$(CXX) -o $@ -std=c++11 $(LOCAL_C_INCLUDES) -c $<

$(LOCAL_MODULE_ALGORITHMS):$(LOCAL_OBJS_ALGORITHMS)
	$(CXX) $^ -lgtest -lpthread -o $@
	
clean:
	rm -rf $(LOCAL_OBJS_ALGORITHMS)
	rm -rf $(LOCAL_MODULE_ALGORITHMS)
	rm -rf gtest-all.o
