# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_all VERSION 0.1.0)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)

# Public part of the cross platform nuhal library.  This contains include directories and flags
# That platform-specific targets should pass on to their dependents
add_library(nuhal_public INTERFACE)
target_include_directories(nuhal_public INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Private part of the cross platform nuhal library. This contains the source files that 
# platform-specific targets should compile but should not be passed to dependents
add_library(nuhal_private INTERFACE)

# The paths's to these source files will be automatically updated during the installation so that they
# can be found when importing nuhall_all in another project
target_sources(nuhal_private INTERFACE
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

cmakeme_install(TARGETS nuhal_public nuhal_private
                NAMESPACE nuhal
                ARCH_INDEPENDENT
                PACKAGE_NAME nuhal_all
                )

include(CTest)
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
target_link_libraries(nuhal_test nuhal_public nuhal_private cmakeme_flags) 
add_test(NAME nuhal_all COMMAND nuhal_test)

# Doxygen documentation
set(DOXYGEN_PROJECT_NAME "Northwestern Utilities and Hardware Abstraction Library (nuhal)")
set(DOXYGEN_EXCLUDE "${CMAKE_SOURCE_DIR}/include/nuhal/catch.hpp")
cmakeme_doxygen(${CMAKE_SOURCE_DIR})

  
