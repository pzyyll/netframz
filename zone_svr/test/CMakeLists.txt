cmake_minimum_required(VERSION 3.7)
project(test)

include_directories(../)

add_definitions(-DLINUX_EPOLL -W -Wall)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES tcp_client.cpp)

add_executable(robot_client robot_client.cpp ${SOURCE_FILES})
add_executable(send_client send_client.cpp ${SOURCE_FILES})
add_executable(send_patch_client send_patch_client.cpp ${SOURCE_FILES})

target_link_libraries(robot_client -lpthread)
