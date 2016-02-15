cmake_minimum_required(VERSION 2.6)

set (PROJECT qtapp)

set (HEADERS dialog.h)
set (SOURCES_ dialog.cpp main.cpp ../../core/tag.cpp)
set (FORMS projman.ui)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=gnu++0x -Wall -pthread")
set ( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../lib/ )

include_directories(../defs  ../../core ../core ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt4 REQUIRED QtCore QtXml QtGui)

include(${QT_USE_FILE})

qt4_wrap_cpp(MOC_SOURCES ${HEADERS} )
qt4_wrap_ui(UI_HEADERS ${FORMS})

add_library(${PROJECT} SHARED ${HEADERS} ${SOURCES_} ${MOC_SOURCES} ${UI_HEADERS})

target_link_libraries(${PROJECT} ${QT_LIBRARIES} ${Boost_LIBRARIES})