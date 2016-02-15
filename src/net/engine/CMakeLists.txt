cmake_minimum_required(VERSION 2.6)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set (CMAKE_BUILD_TYPE Debug)

include_directories(  ../../core
                      ../connection
                      ../drivers/input
                      ../drivers/output/core )

set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/ )
add_library(communication SHARED
                         engine.h 
                         engine.cpp 
                         entries.h 
                         entries.cpp 
                         init.h 
                         init.cpp 
                         ../../core/tag.h 
                         ../../core/tag.cpp 
                         netmodule.h 
                         netmodule.cpp 
                         inputdevice.h 
                         inputdevice.cpp 
                         tools.h 
                         tools.cpp 
                         outputdevice.h 
                         outputdevice.cpp  
                         ../connection/outputethernetconnection.h 
                         ../connection/outputethernetconnection.cpp
                         ../drivers/output/core/outputdriverbase.h )
target_link_libraries(communication ${Boost_LIBRARIES})
IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(communication ws2_32 mswsock)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")

#link_directories   (/usr/lib)

