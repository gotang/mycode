# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)

# Project's name
project(test)

# Set the output folder where your program will be created
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_CXX_FLAGS "-std=c++11")

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(test_c ${PROJECT_SOURCE_DIR}/test.c)

add_executable(test2 ${PROJECT_SOURCE_DIR}/test2.cpp)

add_executable(analyser ${PROJECT_SOURCE_DIR}/analyser.cpp)

add_library(testShareLibrary SHARED ${PROJECT_SOURCE_DIR}/testShareLibrary.cpp)

add_executable(test_cpp ${PROJECT_SOURCE_DIR}/test.cpp)
target_link_libraries(test_cpp testShareLibrary)
