cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../include/opc ${CMAKE_CURRENT_BINARY_DIR}/../../../include/)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread -fno-inline")
set (CMAKE_BUILD_TYPE Debug)
add_definitions (-D_GLIBCXX_USE_NANOSLEEP)

include_directories(   ../../core
                       ../../core/protocols
                       ../connection )

set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/ )
add_library(opccl SHARED opccl.cpp opcmodiface.h
                                   opcmodiface.cpp
                                   ../../core/tag.h
                                   ../../core/tag.cpp
                                   opcclconfparser.h
                                   opcclconfparser.cpp  
                                   opcclconf.h
                                   opcclconf.cpp )

target_link_libraries(opccl ${Boost_LIBRARIES})


add_library (J_INTEROP STATIC IMPORTED)
set_property(TARGET J_INTEROP PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/j-interop.a)

add_library (UTGARD STATIC IMPORTED)
set_property(TARGET UTGARD PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/utgard.a)

add_library (UTGAW STATIC IMPORTED)
set_property(TARGET UTGAW PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libutgaw.a)


IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_library (GCJ_LIB STATIC IMPORTED)
  set_property(TARGET GCJ_LIB PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libgcj.a)

  target_link_libraries (opccl -Wl,-whole-archive GCJ_LIB J_INTEROP UTGARD UTGAW -Wl,-no-whole-archive ws2_32 )
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries (opccl -lgcj J_INTEROP UTGARD UTGAW)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
