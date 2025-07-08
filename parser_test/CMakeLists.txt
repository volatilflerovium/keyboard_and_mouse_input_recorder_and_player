cmake_minimum_required(VERSION 3.13)
project("Json Tests")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# --------------------------------------------------------------------

find_package(Threads REQUIRED)

#--------------------------------------------------------------------- 

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

get_filename_component(PARENT_DIR ../ ABSOLUTE)

if(NOT(CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

add_compile_options("-O3")

#--------------------------------------------------------------------- 

get_filename_component(PARENT_DIR ../ ABSOLUTE)

add_library(Profiler_lib INTERFACE)

target_include_directories(
	Profiler_lib
	INTERFACE
	"${CMAKE_SOURCE_DIR}/profiler_lib"
)

add_library(cstr_split_lib INTERFACE)

target_include_directories(
	cstr_split_lib
	INTERFACE
	"${CMAKE_SOURCE_DIR}/cstr_split_lib"
)

#--------------------------------------------------------------------- 

add_library(json_lib INTERFACE)

target_include_directories(
	json_lib 
	INTERFACE 
	"${CMAKE_SOURCE_DIR}/json_lib/include"
)

#--------------------------------------------------------------------- 

add_executable(
	Test
	test.cpp
)

target_link_libraries(
	Test
	PRIVATE
	Threads::Threads
	json_lib
	Profiler_lib
	cstr_split_lib
)

target_compile_definitions(
	Test
	PUBLIC
	JSON_DATA_FILE="${CMAKE_SOURCE_DIR}/test_json"
	CSPLIT_DATA_FILE="${CMAKE_SOURCE_DIR}/test_csplit"
)

#---------------------------------------------------------------------
