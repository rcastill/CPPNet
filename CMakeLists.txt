cmake_minimum_required(VERSION 2.8.4)
project(CPPNet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "C:/Users/Rodolfo/Development/Tests/CPPNet/bin")

add_library(NetCore net/core.cc)
add_library(Address net/address.cc)
add_library(Packet net/packet.cc)
add_library(DatagramSocket net/dgsocket.cc)

target_link_libraries(DatagramSocket Address)
target_link_libraries(DatagramSocket Packet)

set(SOURCE_FILES main.cc)
add_executable(CPPNet ${SOURCE_FILES})
add_executable(SClient sclient.cc)
add_executable(RClient rclient.cc)


target_link_libraries(CPPNet NetCore DatagramSocket)
target_link_libraries(SClient NetCore DatagramSocket)
target_link_libraries(RClient NetCore DatagramSocket)

if (WIN32)
target_link_libraries(CPPNet wsock32 ws2_32)
target_link_libraries(Address wsock32 ws2_32)
target_link_libraries(SClient wsock32 ws2_32)
target_link_libraries(RClient wsock32 ws2_32)

add_definitions(-DWIN32)
endif()
