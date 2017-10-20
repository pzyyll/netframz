cmake_minimum_required(VERSION 3.7)
project(test)

include_directories(E:\\linux_inc\\include)
include_directories(../)
include_directories(../../net)


add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

find_library(NFLIB libnf.a ../../lib/)

add_executable(iotask iotask.cpp)
add_executable(timer timer.cpp )
add_executable(echo_server echo_server.cpp )
add_executable(epoll epoll.cpp )
add_executable(epoll_inout epoll_in_out.cpp tcp_client.cpp )
add_executable(send_client send_client.cpp tcp_client.cpp )
add_executable(robot_client robot_client.cpp tcp_client.cpp )
add_executable(pthread pthread.cpp)
#add_executable(tmem_check tmem_check.cpp )
add_executable(idle idle.cpp )
add_executable(thread thread.cpp )
add_executable(thread_cond thread_cond.cpp )

#target_link_libraries(netframz -lboostsystem)
target_link_libraries(iotask ${NFLIB})
target_link_libraries(timer ${NFLIB})
target_link_libraries(echo_server ${NFLIB})
target_link_libraries(epoll ${NFLIB})
target_link_libraries(epoll_inout ${NFLIB})
target_link_libraries(send_client ${NFLIB})
target_link_libraries(robot_client -lpthread ${NFLIB})
target_link_libraries(pthread -lpthread ${NFLIB})
target_link_libraries(idle ${NFLIB})
target_link_libraries(thread -lpthread ${NFLIB})
target_link_libraries(thread_cond -lpthread ${NFLIB})

