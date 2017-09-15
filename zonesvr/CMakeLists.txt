cmake_minimum_required(VERSION 3.7)
project(zonesvr)

include_directories(E:\\linux_inc\\include)
include_directories(../netfram/net/)
include_directories(../netfram/utils/)

option(MEM_CHECK "Enable memory check." OFF)

add_definitions(
        -W -Wall
        -DLINUX_EPOLL
        -DNF_TIMER_WHEEL
        )

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE debug CACHE STRING "set build type debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
 
find_library(NFLIB libnf.a ../netfram/lib/)

set(SOURCE_FILES
        main.cpp
        server_config.cpp
        server.cpp
        log.cpp
        config.cpp
        zonesvr.cpp
        player.cpp
        player_manage.cpp
        aoi_manage.cpp
        xml/tinyxml2.cpp
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
target_link_libraries(zonesvr ${NFLIB})
