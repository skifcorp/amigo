cmake_minimum_required(VERSION 2.6)

cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set (CMAKE_BUILD_TYPE Debug)

include_directories(  ${CMAKE_CURRENT_BINARY_DIR}../../../include ../../core)

set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/ )

#add_library(v8 STATIC ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libv8.a)

add_library(js SHARED   js.cpp
                        ../../core/tag.h 
                        ../../core/tag.cpp  )
#                        ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libv8.a )

target_link_libraries(js ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libv8.a )
target_link_libraries(js ${Boost_LIBRARIES})

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(js ws2_32 mswsock winmm) 
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")

#set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread -DENABLE_LOGGING_AND_PROFILING")
#set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
#set (CMAKE_BUILD_TYPE Release)

#add_subdirectory(v8/src)

#include_directories( ../../core )
                       

#set ( LIBRARY_OUTPUT_PATH ../../../lib )
#add_library(js SHARED js.cpp
#                      ../../core/tag.h 
#                      ../../core/tag.cpp )

#link_directories   (/usr/lib)

