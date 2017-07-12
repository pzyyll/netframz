cmake_minimum_required(VERSION 3.7)
project(test)

include_directories(E:\\linux_inc\\include)
include_directories(../)


add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../nf_event.cpp
        ../nf_epoll.cpp
        ../nf_event_task.cpp
        ../nf_event_timer.cpp
        ../nf_event_timer_mng.cpp
        )

add_executable(iotask iotask.cpp ${SOURCE_FILES})
add_executable(timer timer.cpp ${SOURCE_FILES})

#target_link_libraries(netframz -lboostsystem)
target_link_libraries(iotask -lglog)
