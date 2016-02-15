cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set (CMAKE_BUILD_TYPE Debug)

include_directories(   ../../core
                       ../../core/protocols
                       ../connection )

set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/ )
add_library(votlis_rmt SHARED votlis_rmt.cpp 
                                 linker.h 
                                 linker.cpp 
                                 tagconf.h 
                                 tagconf.cpp 
                                 votlismaster.h 
                                 votlismaster.cpp 
                                 votlisprotocol.h 
                                 ../connection/outputethernetconnection.h  
                                 ../connection/outputethernetconnection.cpp 
                                 ../../core/tag.h 
                                 ../../core/tag.cpp )

#link_directories   (/usr/lib)
target_link_libraries(votlis_rmt ${Boost_LIBRARIES})
IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(votlis_rmt ws2_32 mswsock)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
