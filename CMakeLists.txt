cmake_minimum_required(VERSION 3.7)
project(netframz)

include_directories(E:\\linux_inc\\include)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/nf_event.cpp src/nf_epoll.cpp src/nf_epoll.h)
add_executable(netframz ${SOURCE_FILES})