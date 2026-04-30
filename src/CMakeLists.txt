# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_all VERSION 0.1.0)
find_package(cmakeme)
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

target_link_libraries(nuhal PRIVATE cmakeme_flags)

cmakeme_install(TARGETS nuhal NAMESPACE nuhal)

#include(CTest)
#if(BUILD_TESTING)
#  add_executable(nuhal_all_test
#    test/bytestream_test.cpp
#    test/encoder_test.cpp
#    test/error_stub.cpp
#    test/led_stub.cpp
#    test/matrix_test.cpp
#    test/pid_test.cpp
#    test/queue_test.cpp
#    test/time_stub.cpp
#    test/uart_stub.cpp
#    test/utilities_test.cpp
#    test/error_test.cpp
#  )
#  target_link_libraries(nuhal_all_test nuhal_all)
#  target_compile_definitions(nuhal_all_test PRIVATE UNIT_TEST_MODE)
#  add_test(NAME nuhal_all COMMAND nuhal_all_test)
#endif()

# Doxygen documentation
#set(DOXYGEN_PROJECT_NAME "Northwestern Utilities and Hardware Abstraction Library (nuhal)")
#set(DOXYGEN_EXCLUDE "${CMAKE_SOURCE_DIR}/include/nuhal/catch.hpp")
#cmakeme_doxygen(${CMAKE_SOURCE_DIR})
