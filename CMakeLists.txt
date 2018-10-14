cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -g")

add_library(pressCall rate_limiter.cpp)

add_executable(demo demo.cpp)
target_link_libraries(demo pressCall pthread)
