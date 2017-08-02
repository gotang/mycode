# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)

# Project's name
project(test)

# Set the output folder where your program will be created
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(GTest REQUIRED)

# The following folder will be included
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

# Build executable program
add_executable(algorithms 
	main.cpp
	sorting/Sorting.cpp
	sorting/SortingVerifier.cpp
	searching/BinarySearchTree.cpp
	searching/HashTables.cpp
	searching/SearchingVerifier.cpp
	searching/BalancedSearchTree.cpp
	string/StringVerifier.cpp
	string/Strings.cpp
	graph/Graphs.cpp
	graph/GraphVerifier.cpp
	misc/Miscellany.cpp
	misc/MiscellanyVerifer.cpp

)
	
target_link_libraries(algorithms ${GTEST_BOTH_LIBRARIES} pthread)
