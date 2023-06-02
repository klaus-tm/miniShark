cmake_minimum_required(VERSION 3.25)
project(miniShark)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/jsoncpp" "extern/jsoncpp")
#add_subdirectory("${PROJECT_SOURCE_DIR}/extern/PcapPlusPlus" "extern/PcapPlusPlus")
add_executable(miniShark
        .idea/.gitignore
        .idea/miniShark.iml
        .idea/modules.xml
        .idea/vcs.xml
        backend/PcapRead.cpp
        README.md)
if (WIN32)
    target_link_libraries(miniShark PRIVATE ws2_32)
endif ()
if (UNIX)
    target_link_libraries(miniShark PRIVATE socket)
endif ()

find_package(jsoncpp CONFIG REQUIRED)
#find_package(PcapPlusPlus CONFIG REQUIRED)
target_link_libraries(miniShark PUBLIC jsoncpp_lib)
#target_link_libraries(miniShark PUBLIC PcapPlusPlus_lib)
