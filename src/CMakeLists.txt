# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 1.0.0)
find_package(cmakeme 1.0.0 REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "arm-none-eabi")
  find_package(TivaCMake)
endif()

cmakeme_defaults(RelWithDebInfo)

add_library(nuhal
  common/bytestream.c
  common/encoder.c
  common/error.c
  common/led.c
  common/pid.c
  common/matrix.c
  common/protocol.c
  common/queue.c
  common/time.c
  common/uart.c
  common/utilities.c

  # If building for Linux include Linux specific files
  $<$<PLATFORM_ID:Linux>:
  linux/error_host.c
  linux/led_host.c
  linux/time_host.c
  linux/uart_host.c>

  # If building for Tiva include Tiva specific files
  $<$<PLATFORM_ID:arm-none-eabi>:
  tiva/error_tiva.c
  tiva/led_tiva.c
  tiva/pin_tiva.c
  tiva/time_tiva.c
  tiva/tiva.c
  tiva/uart_tiva.c>
)

target_include_directories(nuhal
  PUBLIC
  # Headers used on all platforms
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/common>
  # Linux specific headers
  $<BUILD_INTERFACE:$<$<PLATFORM_ID:Linux>:${CMAKE_CURRENT_SOURCE_DIR}/../include/linux>>
  # Tiva specific headers
  $<BUILD_INTERFACE:$<$<PLATFORM_ID:arm-none-eabi>:${CMAKE_CURRENT_SOURCE_DIR}/../include/tiva>>)


target_link_libraries(nuhal PRIVATE cmakeme_flags
  PUBLIC
  $<$<PLATFORM_ID:arm-none-eabi>:TivaCMake::driverlib>
  m)

cmakeme_install(TARGETS nuhal NAMESPACE nuhal)

# Doxygen documentation
set(DOXYGEN_PROJECT_NAME "Northwestern Utilities and Hardware Abstraction Library (nuhal)")
set(DOXYGEN_EXCLUDE "${CMAKE_SOURCE_DIR}/include/nuhal/catch.hpp")
cmakeme_doxygen(${CMAKE_SOURCE_DIR})
