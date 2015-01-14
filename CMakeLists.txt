cmake_minimum_required(VERSION 2.8.4)
project(CPPNet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

add_library(NetCore src/net/core.cc)
add_library(Address src/net/address.cc)
add_library(Packet src/net/packet.cc)
add_library(DatagramSocket src/net/dgsocket.cc)
add_library(ByteBuffer src/net/bytebuffer.cc)

target_link_libraries(DatagramSocket Address)
target_link_libraries(DatagramSocket Packet)

set(SOURCE_FILES main.cc)
add_executable(CPPNet ${SOURCE_FILES})
add_executable(SClient sclient.cc)
add_executable(Server server.cc)
add_executable(Lab lab.cc)

target_link_libraries(CPPNet NetCore DatagramSocket)
target_link_libraries(SClient NetCore DatagramSocket)
target_link_libraries(Server DatagramSocket Address Packet NetCore)
target_link_libraries(Lab ByteBuffer)

if (WIN32)
target_link_libraries(CPPNet wsock32 ws2_32)
target_link_libraries(Address wsock32 ws2_32)
target_link_libraries(SClient wsock32 ws2_32)
target_link_libraries(ByteBuffer wsock32 ws2_32)
add_definitions(-DWIN32)
endif()

add_definitions(-std=c++11)
