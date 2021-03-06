cmake_minimum_required(VERSION 3.7)
project(test)

include_directories(E:\\linux_inc\\include)
include_directories(../)


add_definitions(-DLINUX_EPOLL -W -Wall -DNF_TIMER_WHEEL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../nf_event.cpp
        ../nf_epoll.cpp
        ../nf_event_iotask.cpp
        ../nf_event_iotask_impl.cpp
        ../nf_event_timer_task.cpp
        ../nf_event_timer_impl.cpp
        ../nf_event_timer_mng.cpp
        ../nf_mem_check.cpp
        ../nf_event_idle_impl.cpp
        ../nf_event_idle_task.cpp
        )

add_executable(iotask iotask.cpp ${SOURCE_FILES})
add_executable(timer timer.cpp ${SOURCE_FILES})
add_executable(echo_server echo_server.cpp ${SOURCE_FILES})
add_executable(epoll epoll.cpp ${SOURCE_FILES})
add_executable(epoll_inout epoll_in_out.cpp tcp_client.cpp ${SOURCE_FILES})
add_executable(send_client send_client.cpp tcp_client.cpp ${SOURCE_FILES})
add_executable(robot_client robot_client.cpp tcp_client.cpp ${SOURCE_FILES})
add_executable(pthread pthread.cpp) 
add_executable(tmem_check tmem_check.cpp ${SOURCE_FILES})
add_executable(idle idle.cpp ${SOURCE_FILES})

#target_link_libraries(netframz -lboostsystem)
#target_link_libraries(iotask -lglog)
target_link_libraries(robot_client -lpthread)
target_link_libraries(pthread -lpthread)
