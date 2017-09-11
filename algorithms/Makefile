LOCAL_SRC_ALGORITHMS:= \
	./main.cpp \
	./sorting/Heap.cpp \
	./sorting/Sorting.cpp \
	./sorting/SortingVerifier.cpp \
	./searching/BinarySearchTree.cpp \
	./searching/HashTables.cpp \
	./searching/SearchingVerifier.cpp \
	./searching/BalancedSearchTree.cpp \
	./string/StringVerifier.cpp \
	./string/Strings.cpp \
	./graph/Graphs.cpp \
	./graph/GraphVerifier.cpp \
	./misc/Miscellany.cpp \
	./misc/MiscellanyVerifer.cpp \
	./number_theory/NumberTheory.cpp \
	./number_theory/NumberTheoryVerifier.cpp

LOCAL_C_INCLUDES:= \
	-I. \
	-I./base
	

LOCAL_OUT:=./build/out

LOCAL_OBJS_ALGORITHMS:=$(addprefix $(LOCAL_OUT)/, $(LOCAL_SRC_ALGORITHMS:.cpp=.o))

LOCAL_MODULE_ALGORITHMS:= algorithms

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
