cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED PICO_SDK_PATH)
  set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

if(NOT PICO_SDK_PATH)
  message(FATAL_ERROR "Set PICO_SDK_PATH to the Raspberry Pi Pico SDK path before configuring.")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_morse_key C CXX ASM)
pico_sdk_init()

add_executable(pico_morse_key
  src/main.cpp
  src/morse_keyer.cpp
)

target_include_directories(pico_morse_key PRIVATE src)
target_link_libraries(pico_morse_key
  pico_stdlib
  hardware_adc
  hardware_pwm
)

pico_enable_stdio_usb(pico_morse_key 1)
pico_enable_stdio_uart(pico_morse_key 0)
pico_add_extra_outputs(pico_morse_key)
