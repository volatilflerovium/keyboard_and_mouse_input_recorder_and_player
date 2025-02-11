cmake_minimum_required(VERSION 3.18)

project(simple_image_diff_lib)

########################################################################

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;${CMAKE_FIND_LIBRARY_SUFFIXES}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT(CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
	add_definitions(-DDEBUG)
endif()

#[[
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions(-DNDEBUG)
endif()
]]

add_compile_options("-O3")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

option(BUILD_STATIC_LIBS "Build the static library" ON)
option(BUILD_TEST "Build test" OFF)

if(BUILD_STATIC_LIBS)
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE SHARED)
endif()

#=====================================================================

set(TINYUSB_LINK_LIB _tinyusb_link)

add_library(
	"${TINYUSB_LINK_LIB}"
	"${LIB_TYPE}"
	"serial_port.cpp"
	"udp_client.cpp"
)

target_include_directories(
	"${TINYUSB_LINK_LIB}"
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(
	"${TINYUSB_LINK_LIB}"
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

#=====================================================================

if(BUILD_TEST)
	add_subdirectory(tests)
endif()

######################################################################
