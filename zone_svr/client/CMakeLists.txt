cmake_minimum_required(VERSION 3.7)
project(test)

include_directories(../)
include_directories(../../src/)

add_definitions(
        -W -Wall
        -DLINUX_EPOLL
        -DNF_TIMER_WHEEL
        #        -DMEM_CHECK
        )

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tcp_client.cpp)
set(CMAKE_BUILD_TYPE debug CACHE STRING "set build type debug")

set(NFSOURCE_FILES
        ../../src/nf_event.cpp
        ../../src/nf_epoll.cpp
        #../src/nf_mem_check.cpp
        ../../src/nf_event_iotask.cpp
        ../../src/nf_event_iotask_impl.cpp
        ../../src/nf_event_timer_task.cpp
        ../../src/nf_event_timer_impl.cpp
        ../../src/nf_event_timer_mng.cpp
        ../../src/nf_event_idle_impl.cpp
        )

add_executable(robot_client robot_client.cpp ${SOURCE_FILES})
add_executable(send_client send_client.cpp ${SOURCE_FILES})
add_executable(send_patch_client send_patch_client.cpp ${SOURCE_FILES})
add_executable(zone_client zone_client.cpp client_api.cpp ../proto/zonesvr.pb.cc ${SOURCE_FILES} ${NFSOURCE_FILES})
add_executable(zone_clientv2 zone_client_v2.cpp ../proto/zonesvr.pb.cc ${SOURCE_FILES})

target_link_libraries(robot_client -lpthread)
target_link_libraries(zone_client -lpthread -lprotobuf)
target_link_libraries(zone_clientv2 -lpthread -lprotobuf)
