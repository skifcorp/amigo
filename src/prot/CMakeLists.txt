cmake_minimum_required(VERSION 2.6)



set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set (CMAKE_BUILD_TYPE Debug)

include_directories( ../core ) 

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../include/mysql)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")

set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../lib/ )
add_library(prot SHARED 
                               xmlconfigreader.h 
                               xmlconfigreader.cpp 
                               compile.h 
                               configdata.h 
                               configdata.cpp 
                               ../core/monotone_timer.hpp
                               configreader.h 
                               basesaver.h 
                               basesaver.cpp 
                               mysqldriver.h 
                               mysqldriver.cpp
                               prot.cpp 
							   )

target_link_libraries(prot ${Boost_LIBRARIES})
IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(prot ws2_32 mswsock)
  
  
  #link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../lib)
  target_link_libraries(prot ${CMAKE_CURRENT_BINARY_DIR}/../../lib/mysqlcppconn.lib)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")

#            /usr/lib/libboost_system.so
#            /usr/lib/libboost_date_time.so
#            /usr/lib/libboost_filesystem.so
#            /usr/lib/libboost_thread.so
#            /usr/lib/libmysqlcppconn.so

 

#link_directories   (/usr/lib)

