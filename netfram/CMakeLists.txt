cmake_minimum_required(VERSION 3.7)
project(netframz)

include_directories(E:\\linux_inc\\include)
include_directories(./utils)
include_directories(./net)

add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE debug CACHE STRING "set build type debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/)

set(UTILS_FILES
    ./utils/nf_event_iotask_impl.cpp
    ./utils/nf_event_idle_impl.cpp
    ./utils/nf_thread.cpp
    ./utils/nf_event_timer_mng.cpp
    ./utils/nf_event_idle_task.cpp
    ./utils/nf_event_timer_task.cpp
    #./utils/nf_mem_check.cpp
    ./utils/nf_mutex.cpp
    ./utils/nf_event_iotask.cpp
    ./utils/nf_condition.cpp
    ./utils/nf_event.cpp
    ./utils/nf_event_timer_impl.cpp
    ./utils/nf_epoll.cpp
    )

set(NET_FILES
    ./net/nf_socket_api.cpp
    ./net/acceptor.cpp
    ./net/endpoint.cpp
    ./net/socket.cpp
    ./net/connector.cpp
   )

#add_executable(netframz ${UTILS_FILES} ${NET_FILES})

add_library(nf STATIC  ${UTILS_FILES} ${NET_FILES})

#target_link_libraries(netframz -lboostsystem)
