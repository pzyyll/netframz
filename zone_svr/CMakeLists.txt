cmake_minimum_required(VERSION 3.7)
project(zonesvr)

include_directories(E:\\linux_inc\\include)
include_directories(../src/)

option(MEM_CHECK "Enable memory check." OFF)

add_definitions(
        -W -Wall
        -DLINUX_EPOLL
        -DNF_TIMER_WHEEL
        )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug CACHE STRING "set build type debug")

set(NFSOURCE_FILES
        ../src/nf_event.cpp
        ../src/nf_epoll.cpp
        ../src/nf_event_iotask.cpp
        ../src/nf_event_iotask_impl.cpp
        ../src/nf_event_timer_task.cpp
        ../src/nf_event_timer_impl.cpp
        ../src/nf_event_timer_mng.cpp
        ../src/nf_event_idle_impl.cpp
        ../src/nf_condition.cpp
        ../src/nf_thread.cpp
        ../src/nf_mutex.cpp
        ../src/nf_socket_api.cpp
        )

set(SOURCE_FILES
        main.cpp
        connector.cpp
        server_config.cpp
        server.cpp
        log.cpp
        config.cpp
        zonesvr.cpp
        player.cpp
        player_manage.cpp
        aoi_manage.cpp
        )

set(PROTO_FILES proto/zonesvr.pb.cc)

if (MEM_CHECK)
    add_definitions(-DMEM_CHECK)
    list(APPEND NFSOURCE_FILES ../src/nf_mem_check.cpp)
endif()

add_executable(zonesvr ${SOURCE_FILES} ${NFSOURCE_FILES} ${PROTO_FILES})

#target_link_libraries(netframz -lboostsystem)
#target_link_libraries(test_svr -lglog)
target_link_libraries(zonesvr -lprotobuf)
