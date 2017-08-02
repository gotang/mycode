# Specify the minimum version for CMake
cmake_minimum_required(VERSION 2.8)

# Project's name
project(OpenCV)

# Set the output folder where your program will be created
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

LINK_DIRECTORIES(${EXECUTABLE_OUTPUT_PATH})
# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(DisplayImage ${PROJECT_SOURCE_DIR}/DisplayImage.cpp)
target_link_libraries(DisplayImage ${OpenCV_LIBS})