cmake_minimum_required(VERSION 3.7)
project(netframz)

include_directories(E:\\linux_inc\\include)

add_definitions(-DLINUX_EPOLL -W -Wall)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp src/nf_event.cpp src/nf_epoll.cpp src/nf_event_task.cpp)
add_executable(netframz ${SOURCE_FILES})

#target_link_libraries(netframz -lboostsystem)
target_link_libraries(netframz -lglog)