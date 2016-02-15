cmake_minimum_required(VERSION 2.6)

#MESSAGE("test environment:" "$ENV{CPU}" "and" "$ENV{ccc}")

#MESSAGE( ${CMAKE_SYSTEM_NAME} )
#MESSAGE( ${CMAKE_SYSTEM_VERSION} )
#MESSAGE( ${CMAKE_SYSTEM_PROCESSOR} )





#MESSAGE( ${CMAKE_HOST_SYSTEM_NAME} )

#MESSAGE( ${CMAKE_HOST_SYSTEM_NAME} )


add_subdirectory(v8/src/)
add_subdirectory(opc/utgard/)
add_subdirectory(opc/utgaw/)
add_subdirectory(opc/opc_test)

file ( COPY ${CMAKE_CURRENT_SOURCE_DIR}/boost DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../include/ )
IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  file ( COPY ${CMAKE_CURRENT_SOURCE_DIR}/gcj/gcj DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)
  file ( COPY ${CMAKE_CURRENT_SOURCE_DIR}/gcj/include DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)
  file ( COPY ${CMAKE_CURRENT_SOURCE_DIR}/gcj/lib/libgcj.a DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../lib)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")


#add_subdirectory(net)
#add_subdirectory(prot)
#add_subdirectory(qt)