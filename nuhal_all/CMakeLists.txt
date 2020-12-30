# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_all VERSION 0.1.0)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)

add_library(nuhal_all_int INTERFACE)
target_include_directories(nuhal_all_int INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# This library is a pure compile-time library. It just brings in sources that must be compiled
# by the platform-specific libraries. We need different locations depending on if the source code is
# installed or just being used from the build directory
add_library(nuhal_all INTERFACE)

# The paths's to these source files will be automatically updated during the installation so that they
# can be found when importing nuhall_all in another project
target_sources(nuhal_all INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/bytestream.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/encoder.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/error.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/led.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pid.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/matrix.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/protocol.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/queue.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/time.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/uart.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/utilities.c>)


cmakeme_install(TARGETS nuhal_all nuhal_all_int
                NAMESPACE nuhal
                ARCH_INDEPENDENT
                PACKAGE_NAME nuhal_all
                )

enable_testing()
add_executable(nuhal_test
  test/bytestream_test.cpp
  test/encoder_test.cpp
  test/error_stub.cpp
  test/led_stub.cpp
  test/matrix_test.cpp
  test/pid_test.cpp
  test/queue_test.cpp
  test/time_stub.cpp
  test/uart_stub.cpp
  test/utilities_test.cpp
  )
target_link_libraries(nuhal_test PRIVATE nuhal_all nuhal_all_int cmakeme_flags) 
add_test(NAME nuhal_all COMMAND nuhal_test)

# Doxygen documentation
set(DOXYGEN_PROJECT_NAME "Northwestern Utilities and Hardware Abstraction Library (nuhal)")
set(DOXYGEN_EXCLUDE "${CMAKE_SOURCE_DIR}/include/nuhal/catch.hpp")
cmakeme_doxygen(${CMAKE_SOURCE_DIR})

  
