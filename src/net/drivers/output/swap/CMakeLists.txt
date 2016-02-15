cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set (CMAKE_BUILD_TYPE Debug)

include_directories(   ../../../engine
                       ../../../sockets
                       ../../../../core
                       ../../../connection
                       ../../../engine/protocols
                       ../core )

set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../../../lib/net/drivers/output )

add_library(swap SHARED swap.cpp 
                        ../../../engine/entries.h 
                        ../core/outputdriverbase.h 
                        ../../../engine/entries.cpp 
                        ../../../../core/tag.h 
                        ../../../../core/tag.cpp )
target_link_libraries(swap ${Boost_LIBRARIES})
IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(swap ws2_32)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")


