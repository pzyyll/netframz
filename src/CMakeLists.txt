cmake_minimum_required(VERSION 3.7)

add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

set(LIB_SOURCE_FILES
        nf_event.cpp
        nf_epoll.cpp
        nf_event_task.cpp
        nf_event_timer_mng.cpp
        )

add_library(nf STATIC ${LIB_SOURCE_FILES})
