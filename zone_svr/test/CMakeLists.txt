cmake_minimum_required(VERSION 3.7)
project(test)

include_directories(../)

add_definitions(-DLINUX_EPOLL -W -Wall)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tcp_client.cpp)

add_executable(robot_client robot_client.cpp ${SOURCE_FILES})
add_executable(send_client send_client.cpp ${SOURCE_FILES})
add_executable(send_patch_client send_patch_client.cpp ${SOURCE_FILES})
add_executable(zone_client zone_client.cpp client_api.cpp ../proto/zonesvr.pb.cc ${SOURCE_FILES})
add_executable(zone_clientv2 zone_client_v2.cpp ../proto/zonesvr.pb.cc ${SOURCE_FILES})

target_link_libraries(robot_client -lpthread)
target_link_libraries(zone_client -lpthread -lprotobuf)
target_link_libraries(zone_clientv2 -lpthread -lprotobuf)
