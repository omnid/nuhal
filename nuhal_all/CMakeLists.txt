# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_all VERSION 0.1.0)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)

# Add useful compiler options here, for each system.  This is anything beyond the necessary ones defined in the platform files
# target_compile_features don't seem to be working currently for C so I am adding the flags explicitly here.
# Overall I am not using target_compile_features because it also causes problems when CXX is not an enabled language
add_library(nuhal_flags INTERFACE)
target_compile_options(nuhal_flags INTERFACE
  $<$<C_COMPILER_ID:GNU,CLANG>:  -pedantic -pipe -Wall -Wextra $<$<COMPILE_LANGUAGE:C>:-std=c99>>    
  $<$<CXX_COMPILER_ID:GNU,CLANG>:  -pedantic -pipe -Wall -Wextra $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>>    
  $<$<C_COMPILER_ID:TI>:-pden -pdr -pdv --c99 --diag_suppress=270>
  )

target_include_directories(nuhal_flags INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

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


cmakeme_install(TARGETS nuhal_all nuhal_flags
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
target_link_libraries(nuhal_test PRIVATE nuhal_all nuhal_flags) 
add_test(NAME nuhal_all COMMAND nuhal_test)


  
