# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_inner VERSION 0.1.0)
find_package(cmakeme)

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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/common>

  # If building for Linux include Linux-specific includes
  $<$<PLATFORM_ID:Linux>:$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/linux>>

  # If building for Tiva include Tiva-specific includes
  $<$<PLATFORM_ID:arm-none-eabi>:$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/tiva>>)

target_link_libraries(nuhal PRIVATE cmakeme_flags
  PUBLIC
  $<$<PLATFORM_ID:arm-none-eabi>:TivaCMake::driverlib>
  m)

cmakeme_install(TARGETS nuhal NAMESPACE nuhal)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  add_subdirectory(test/linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL arm-none-eabi)
  add_subdirectory(test/tiva)
endif()

# Doxygen documentation
set(DOXYGEN_PROJECT_NAME "Northwestern Utilities and Hardware Abstraction Library (nuhal)")
set(DOXYGEN_EXCLUDE "${CMAKE_SOURCE_DIR}/include/nuhal/catch.hpp")
cmakeme_doxygen(${CMAKE_SOURCE_DIR})
