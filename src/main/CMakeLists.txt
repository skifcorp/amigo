cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")

#include_directories(/usr/local/include ../core ${CMAKE_CURRENT_BINARY_DIR}/../include)
include_directories(/usr/local/include ../core )

link_directories   (/usr/lib)
set(CMAKE_BUILD_TYPE Debug)

add_executable(amigo ../core/amigofs.h 
                     ../core/amigofs.cpp 
                     ../core/tag.h 
                     ../core/tag.cpp 
                     ../core/tag.cpp 
                     ../core/amodule.h 
                     ../core/amodule.cpp
                     main.cpp )


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../bin)

#find_package(Boost COMPONENTS REQUIRED system filesystem)


IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries (amigo dl)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

#target_link_libraries (amigo libboost_filesystem.so)
target_link_libraries (amigo ${Boost_LIBRARIES})



