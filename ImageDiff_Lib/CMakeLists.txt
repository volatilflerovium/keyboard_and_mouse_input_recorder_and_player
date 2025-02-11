cmake_minimum_required(VERSION 3.18)

project(simple_image_diff_lib)

########################################################################

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;${CMAKE_FIND_LIBRARY_SUFFIXES}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions(-DNDEBUG)
endif()

add_compile_options("-O3")

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS_EQUAL "8.3.0")
	find_package(OpenCV 4.0.0 REQUIRED 
		PATHS "/home/dan/Libs/share/release/lib/cmake" NO_DEFAULT_PATH
		COMPONENTS imgproc highgui
	)
else()
	find_package(OpenCV 4.0.0 REQUIRED COMPONENTS imgproc highgui)
endif()

message("OpenCV version : ${OpenCV_VERSION}") 

######################################################################
######################################################################

add_library(
	simple_img_diff
	SHARED
	"simple_image_difference.cpp"
)

target_link_libraries(
	simple_img_diff
	PRIVATE
	opencv_imgproc
	opencv_highgui
)

target_include_directories(
	simple_img_diff
	PUBLIC
	"${CMAKE_SOURCE_DIR}/include"
)

set_target_properties(
	simple_img_diff
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
)

######################################################################



