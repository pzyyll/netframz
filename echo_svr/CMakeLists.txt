cmake_minimum_required(VERSION 3.7)
project(echo_svr)

include_directories(E:\\linux_inc\\include)
include_directories(../src/)

add_definitions(
        -W -Wall
        -DLINUX_EPOLL
        -DNF_TIMER_WHEEL
        -DMEM_CHECK
        )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug CACHE STRING "set build type debug")

set(NFSOURCE_FILES
        ../src/nf_event.cpp
        ../src/nf_epoll.cpp
        ../src/nf_mem_check.cpp
        ../src/nf_event_iotask.cpp
        ../src/nf_event_iotask_impl.cpp
        ../src/nf_event_timer_task.cpp
        ../src/nf_event_timer_impl.cpp
        ../src/nf_event_timer_mng.cpp
        ../src/nf_event_idle_impl.cpp
        )

set(SOURCE_FILES
        main.cpp
        connector.cpp
        server_config.cpp
        server.cpp
        log.cpp
        config.cpp
        )

add_executable(echo_svr ${SOURCE_FILES} ${NFSOURCE_FILES})

#target_link_libraries(netframz -lboostsystem)
#target_link_libraries(test_svr -lglog)
