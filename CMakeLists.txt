cmake_minimum_required (VERSION 2.6)
project (ydcv-cpp)
set (ydcv-cpp_VERSION_MAJOR 0)
set (ydcv-cpp_VERSION_MINOR 1)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(opt)

add_executable(ydcv-cpp ydcv.cpp)
target_link_libraries(ydcv-cpp curl boost_program_options readline ${CPR_LIBRARIES})
include_directories(${CPR_INCLUDE_DIRS})
