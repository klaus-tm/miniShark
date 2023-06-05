cmake_minimum_required(VERSION 3.25)
project(miniShark)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/jsoncpp" "extern/jsoncpp")
set(WPCAP_DIR "${PROJECT_SOURCE_DIR}/extern/winpcap")
include_directories(${WPCAP_DIR}/include)
add_executable(miniShark
        .idea/.gitignore
        .idea/miniShark.iml
        .idea/modules.xml
        .idea/vcs.xml
        backend/PcapRead.cpp
        README.md)
add_executable(PacketSniff backend/PcapSniff.cpp)
if (WIN32)
    target_link_libraries(miniShark PRIVATE ws2_32)
    target_link_libraries(PacketSniff PRIVATE ws2_32)
endif ()
if (UNIX)
    target_link_libraries(miniShark PRIVATE socket)
endif ()

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(miniShark PUBLIC jsoncpp_lib)
find_library(WPCAP_LIBRARY wpcap HINTS ${WPCAP_DIR}/lib)
target_link_libraries(PacketSniff PRIVATE ${WPCAP_LIBRARY})


