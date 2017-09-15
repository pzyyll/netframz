cmake_minimum_required(VERSION 3.7)

add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

set(LIB_SOURCE_FILES
    nf_condition.cpp
    nf_epoll.cpp
    nf_event.cpp
    nf_event_idle_impl.cpp
    nf_event_idle_task.cpp
    nf_event_iotask.cpp
    nf_event_iotask_impl.cpp
    nf_event_timer_impl.cpp
    nf_event_timer_mng.cpp
    nf_event_timer_task.cpp
    #nf_mem_check.cpp
    nf_mutex.cpp
    nf_thread.cpp
        )

add_library(nfutils STATIC ${LIB_SOURCE_FILES})
