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

# Build executable program
add_executable(exercises 
	main.cpp
	STL.cpp
	CPlusPlus.cpp
	LeetCode/LeetCodeDataStructure.cpp
	LeetCode/LeetCodeAVerifier.cpp
	LeetCode/LeetCodeA.cpp
	LeetCode/LeetCodeBVerifier.cpp
	LeetCode/LeetCodeB.cpp
	NowCoder.cpp
	ProjectEuler.cpp
	NowCoderVerifier.cpp
	LintCodeVerifier.cpp
	LintCode.cpp
)
	
target_link_libraries(exercises ${GTEST_BOTH_LIBRARIES} pthread)
