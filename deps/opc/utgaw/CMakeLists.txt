cmake_minimum_required (VERSION 2.6)

set ( PROJECT utgaw )

#include_directories(../../include)

set(INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../../include/opc )

include_directories( "${INCLUDE_DIR}" ${CMAKE_CURRENT_BINARY_DIR}/../../../include )

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -D_GLIBCXX_USE_NANOSLEEP")

set (MY_HEADERS  connectioninformation.h
                         group.h
                         item.h
                         jivariant.h
                         server.h
                         tools.h 
                         itemstate.h
                         nativejivariantwrapper.h
                         jisystem.h
                         jexception.h 
                         jinitializer.h)

set (MY_SOURCES connectioninformation.cpp 
                group.cpp 
                item.cpp 
                jivariant.cpp 
                server.cpp 
                itemstate.cpp
                jexception.cpp )

add_library(utgaw STATIC ${MY_HEADERS} ${MY_SOURCES}) 
                         
set_property (TARGET utgaw PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../../lib)

target_link_libraries (utgaw)

foreach (header ${MY_HEADERS})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${header} ${INCLUDE_DIR}/${header} )
endforeach(header)


#set(CMAKE_INSTALL_PREFIX .)

#file ()

#install ( FILES ${MY_HEADERS} DESTINATION ../../include )

