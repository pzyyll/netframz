cmake_minimum_required(VERSION 3.7)
project(test_svr)

include_directories(E:\\linux_inc\\include)
include_directories(../)

add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

set(NFSOURCE_FILES
        ../src/nf_event.cpp
        ../src/nf_epoll.cpp
        ../src/nf_event_task.cpp
        ../src/nf_event_timer.cpp
        ../src/nf_event_timer_mng.cpp)

set(SOURCE_FILES main.cpp)

add_executable(test_svr ${SOURCE_FILES} ${NFSOURCE_FILES})

#target_link_libraries(netframz -lboostsystem)
target_link_libraries(test_svr -lglog)
