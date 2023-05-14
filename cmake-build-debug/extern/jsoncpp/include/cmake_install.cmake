# Install script for directory: D:/projects/c++/miniShark/extern/jsoncpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/miniShark")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/json" TYPE FILE FILES
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/allocator.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/assertions.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/config.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/forwards.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/json.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/json_features.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/reader.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/value.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/version.h"
    "D:/projects/c++/miniShark/extern/jsoncpp/include/json/writer.h"
    )
endif()

