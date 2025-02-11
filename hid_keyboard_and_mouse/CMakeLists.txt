cmake_minimum_required(VERSION 3.18)

if(NOT(PICO_BOARD))
	set(PICO_BOARD pico_w)
endif()

#include build functions from Pico SDK

if(NOT(PICO_SDK_PATH))
	message(FATAL_ERROR "Set a valid path for pico-sdk -DPICO_SDK_PATH")
endif()	

include(${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/hw/bsp/family_support.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ Standards

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD 17)

project(hid_keyboard_and_mouse_udp C CXX ASM)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

##====================================================================

include(${CMAKE_CURRENT_SOURCE_DIR}/udp_server_for_pico_w/cmake/utilities.cmake)

message_yellow("Use -DBOARD_TYPE=pico build project for pico board. Default: pico_w")

generate_server_definition_file(
"${CMAKE_CURRENT_SOURCE_DIR}/udp_server_for_pico_w/" 
"${CMAKE_CURRENT_SOURCE_DIR}/udp_server_for_pico_w/include/udp_server/"
)

set(PROJECT hid_keyboard_and_mouse_udp)

if(PICO_BOARD STREQUAL "pico_w")
	set(UDP_SERVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/udp_server_for_pico_w/src/picow_udp_server.c")
	set(UDP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/udp_server_for_pico_w/include")
	set(UDP_HELPER_LIB "pico_cyw43_arch_lwip_threadsafe_background")
endif()

add_executable(
	${PROJECT}
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/usb_descriptors.c
	"${UDP_SERVER_SRC}"
)

target_include_directories(
	${PROJECT} 
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	"${UDP_INCLUDE_DIR}"
	"${UDP_INCLUDE_DIR}/udp_server"
)

if(PICO_BOARD STREQUAL "pico_w")
	target_compile_definitions(
		${PROJECT}
		PRIVATE
		WIFI_SSID=\"${WIFI_SSID}\"
		WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
	)
endif()

target_link_libraries(
	${PROJECT} 
	PUBLIC
	pico_stdlib
	${UDP_HELPER_LIB}
)

# Configure compilation flags and libraries for the example without RTOS.
# See the corresponding function in hw/bsp/FAMILY/family.cmake for details.

family_configure_device_example(${PROJECT} noos)

##====================================================================
