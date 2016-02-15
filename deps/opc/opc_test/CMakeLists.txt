cmake_minimum_required (VERSION 2.6)

set ( PROJECT utgaw_test )

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../include/opc ${CMAKE_CURRENT_BINARY_DIR}/../../../include/)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall")

add_definitions (-D_GLIBCXX_USE_NANOSLEEP)


add_library (J_INTEROP STATIC IMPORTED)
set_property(TARGET J_INTEROP PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/j-interop.a)

add_library (UTGARD STATIC IMPORTED)
set_property(TARGET UTGARD PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/utgard.a)

add_library (UTGAW STATIC IMPORTED)
set_property(TARGET UTGAW PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libutgaw.a)


add_executable (utgaw_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries (utgaw_test -Wl,-whole-archive J_INTEROP UTGARD UTGAW -Wl,-no-whole-archive)

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_library (GCJ_LIB STATIC IMPORTED)
  set_property(TARGET GCJ_LIB PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/libgcj.a)
  
  target_link_libraries (utgaw_test -Wl,-whole-archive GCJ_LIB -Wl,-no-whole-archive ws2_32 )
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries (utgaw_test -lgcj )
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")






