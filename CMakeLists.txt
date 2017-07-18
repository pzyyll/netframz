cmake_minimum_required(VERSION 3.7)
project(netframz)

include_directories(E:\\linux_inc\\include)

add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        src/nf_event.cpp
        src/nf_epoll.cpp
        src/nf_event_timer_mng.cpp
        src/nf_event_timer_task.cpp
        src/nf_event_iotask.cpp
        src/nf_event_iotask_impl.cpp
        src/nf_event_timer_impl.cpp)
add_executable(netframz ${SOURCE_FILES})

#target_link_libraries(netframz -lboostsystem)
target_link_libraries(netframz -lglog)
