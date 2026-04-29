# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_all VERSION 0.1.0)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)

# nuhal_all is an object library since it includes components common
# to the tiva and non_tiva builds and is meant to be directly integrated
# into those libraries instead of being a separate library dependency
add_library(nuhal_all
  OBJECT
  src/bytestream.c
  src/encoder.c
  src/error.c
  src/led.c
  src/pid.c
  src/matrix.c
  src/protocol.c
  src/queue.c
  src/time.c
  src/uart.c
  src/utilities.c)

target_include_directories(nuhal_all
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(nuhal_all PRIVATE cmakeme_flags)

cmakeme_install(TARGETS nuhal_all
                NAMESPACE nuhal
                PACKAGE_NAME nuhal_all)

include(CTest)
if(BUILD_TESTING)
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
    test/error_test.cpp
  )
  target_link_libraries(nuhal_test nuhal_all)
  target_compile_definitions(nuhal_test PRIVATE UNIT_TEST_MODE)
  add_test(NAME nuhal_all COMMAND nuhal_test)
endif()

# Doxygen documentation
set(DOXYGEN_PROJECT_NAME "Northwestern Utilities and Hardware Abstraction Library (nuhal)")
set(DOXYGEN_EXCLUDE "${CMAKE_SOURCE_DIR}/include/nuhal/catch.hpp")
cmakeme_doxygen(${CMAKE_SOURCE_DIR})
