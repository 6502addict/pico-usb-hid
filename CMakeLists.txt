cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

#include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
#include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-usb-hid C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(pico-usb-hid main.c)

target_sources(pico-usb-hid PRIVATE
			main.c
			usb_hid.c
			kbd_decode_vt100.c
			kbd_decode_tvi950.c
			mouse_decode.c
			kbd_ringbuffer.c
			mouse_ringbuffer.c) 

pico_set_program_name(pico-usb-hid "pico-usb-hid")
pico_set_program_version(pico-usb-hid "0.1")

pico_enable_stdio_uart(pico-usb-hid 1)

# Add the standard library to the build
target_link_libraries(pico-usb-hid pico_stdlib)
target_include_directories (pico-usb-hid PUBLIC .)

# Add any user requested libraries
target_link_libraries(pico-usb-hid
        pico_stdlib			
        hardware_timer
	tinyusb_host			
	tinyusb_board
        )

pico_add_extra_outputs(pico-usb-hid)



