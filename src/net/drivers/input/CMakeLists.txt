cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set (CMAKE_BUILD_TYPE Debug)

include_directories(   ../../engine
                       ../../sockets
                       ../../../core
                       ../../connection
                       ../../engine/protocols )
                       

set (LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../../lib/net/drivers/input)

add_library(votlis SHARED votlis.cpp 
            votlis.h 
            inputdriver.cpp 
            inputdriver.h 
            ../../engine/entries.h  
            ../../engine/entries.cpp)
target_link_libraries(votlis ${Boost_LIBRARIES})

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(votlis ws2_32)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
#link_directories   (/usr/lib)

