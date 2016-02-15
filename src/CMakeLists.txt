cmake_minimum_required(VERSION 2.6)

#MESSAGE("test environment:" "$ENV{CPU}" "and" "$ENV{ccc}")

#MESSAGE( ${CMAKE_SYSTEM_NAME} )
#MESSAGE( ${CMAKE_SYSTEM_VERSION} )
#MESSAGE( ${CMAKE_SYSTEM_PROCESSOR} )





#MESSAGE( ${CMAKE_HOST_SYSTEM_NAME} )

#MESSAGE( ${CMAKE_HOST_SYSTEM_NAME} )


find_package(Boost COMPONENTS REQUIRED system filesystem date_time)



if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../include)

add_subdirectory(main)
add_subdirectory(net)
add_subdirectory(prot)
add_subdirectory(qt)
#add_subdirectory(qt)