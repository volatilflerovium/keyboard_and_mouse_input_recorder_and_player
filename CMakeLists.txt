# --------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)
project("K_and_M_recorder_player")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# --------------------------------------------------------------------

find_package(Threads REQUIRED)

#--------------------------------------------------------------------- 
## Please set your wxWidgets configuration here
#--------------------------------------------------------------------- 
 
# Here you can define what libraries of wxWidgets you need for your
# application. You can figure out what libraries you need here;
# https://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
# We need the Find package for wxWidgets to work
# NOTE: if you're using aui, include aui in this required components list.

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS core base REQUIRED)
 
#---------------------------------------------------------------------
## Actual config file starts here
#---------------------------------------------------------------------
 
# wxWidgets include (this will do all the magic to configure everything)
include("${wxWidgets_USE_FILE}")

#--------------------------------------------------------------------- 

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin)

get_filename_component(PARENT_DIR ../ ABSOLUTE)

if(NDEBUG)
	add_definitions(-DNDEBUG=1)
endif() 

if(NOT(CMAKE_BUILD_TYPE))
	set(CMAKE_BUILD_TYPE "Debug")
	add_definitions(-DDEBUG)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_compile_options("-Wno-comment")
	add_definitions(-DDEBUG_DIR="${CMAKE_SOURCE_DIR}")
endif()


add_compile_options("-O3")

#--------------------------------------------------------------------- 


set(IMG_DIFF_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ImageDiff_Lib")

find_library(
	simple_img_diff_lib
	NAMES simple_img_diff
	PATHS "${IMG_DIFF_LIB_DIR}/lib" 
	REQUIRED
)

set(TINYUSB_LINK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinyusb_link_lib")

set(LINK_LIB _tinyusb_link)

add_subdirectory("${TINYUSB_LINK_DIR}")

#--------------------------------------------------------------------- 

set(App kmRecPlayerApp)

set(SOURCES1
	main.cpp
	km_recorder_player.cpp
	error_reporting.cpp
	keyboard_emulator.cpp
	uinput_keyboard.cpp
	tinyusb_keyboard.cpp
	hid_manager.cpp
	mouse_emulator.cpp
	uinput_mouse.cpp
	tinyusb_mouse.cpp
	input_command.cpp
	image_panel.cpp
	command_parser.cpp
	ext_scrolled_window.cpp
	command_wrapper.cpp
	event_definitions.cpp
	wx_textctrl.cpp
	extended_popup.cpp
	utilities.cpp
	file_scrolled_window.cpp
	dedicated_popups.cpp
	inputblocker.cpp
	tinyusb_connector.cpp
	settings_manager.cpp
	key_conversion.cpp
)

add_executable(
	"${App}"
	${SOURCES1}
)

target_link_libraries(
	"${App}"
	PRIVATE
	Threads::Threads
	"${wxWidgets_LIBRARIES}"
	"${simple_img_diff_lib}"
	"${LINK_LIB}"
)

target_include_directories(
	"${App}"
	#PRIVATE
	PUBLIC
	"${IMG_DIFF_LIB_DIR}/include"
	"${TINYUSB_LINK_DIR}/include"
)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS_EQUAL "8.3.0")
	target_link_libraries(
		"${App}"
		PRIVATE
		stdc++fs
	)
endif()


message("wxWidgets version: ${wxWidgets_VERSION_STRING}")

if("${wxWidgets_VERSION_MAJOR}.${wxWidgets_VERSION_MINOR}.${wxWidgets_VERSION_PATCH}" VERSION_LESS "3.3.0")

	target_compile_definitions(
		"${App}"
		PRIVATE
		WX_STRING_ARRAY
	)
endif()

if(DEPLOYING)
	# install resources
	add_subdirectory(resources)
endif()

#---------------------------------------------------------------------

if(NOT WIN32)
	#https://stackoverflow.com/questions/18968979/how-to-make-colorized-message-with-cmake
	string(ASCII 27 Esc)
	set(ColourReset "${Esc}[m")
	set(Yellow      "${Esc}[32m")
endif()

#---------------------------------------------------------------------
