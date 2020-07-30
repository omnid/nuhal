# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 1.0.0)

# Add useful compiler options here, for each system.  This is anything beyond the necessary ones defined in the platform files
add_library(nuhal_flags INTERFACE)
target_compile_options(nuhal_flags INTERFACE
  $<$<C_COMPILER_ID:GNU>:  -pedantic -pipe -Wall -Wextra>    
  $<$<C_COMPILER_ID:CLANG>:-pedantic -pipe -Wall -Wextra>  
  $<$<C_COMPILER_ID:TI>:-pden -pdr -pdv --c99 --diag_suppress=270>
  $<$<PLATFORM_ID:${CMAKE_HOST_SYSTEM_NAME}>:-march=native>
  )

target_compile_features(nuhal_flags INTERFACE
  cxx_std_17
  c_std_99
  )

# This library is a pure compile-time library. It just brings in sources that must be compiled
# by the platform-specific libraries. We need different locations depending on if the source code is
# installed or just being used from the build directory
add_library(nuhal_all INTERFACE)
target_sources(nuhal_all INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/bytestream.c>
  $<BUILD_INTERFACE:src/bytestream.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/encoder.c>
  $<BUILD_INTERFACE:src/encoder.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/error.c>
  $<BUILD_INTERFACE:src/error.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/led.c>
  $<BUILD_INTERFACE:src/led.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/pid.c>
  $<BUILD_INTERFACE:src/pid.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/matrix.c>
  $<BUILD_INTERFACE:src/matrix.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/queue.c>
  $<BUILD_INTERFACE:src/queue.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/time.c>
  $<BUILD_INTERFACE:src/time.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/uart.c>
  $<BUILD_INTERFACE:src/uart.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/utilities.c>
  $<BUILD_INTERFACE:src/utilities.c>
  )

target_include_directories(nuhal_all INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

enable_testing()
add_executable(nuhal_test
  test/bytestream_test.cpp
  test/encoder_test.cpp
  test/error_stub.cpp
  test/matrix_test.cpp
  test/pid_test.cpp
  test/queue_test.cpp
  test/time_stub.cpp
  test/utilities_test.cpp
  )
target_link_libraries(nuhal_test PRIVATE nuhal_all nuhal_flags) 
add_test(NAME nuhal_all COMMAND nuhal_test)



  
