cmake_minimum_required(VERSION 2.8)

project(final_x)

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
set(SOURCE main.cpp )
add_executable(final ${SOURCE})
target_link_libraries(final)