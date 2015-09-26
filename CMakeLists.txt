cmake_minimum_required (VERSION 2.6)
project (ydcv-cpp)
set (ydcv-cpp_VERSION_MAJOR 0)
set (ydcv-cpp_VERSION_MINOR 1)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(ydcv-cpp ydcv.cpp)
target_link_libraries(ydcv-cpp curlcpp curl)
