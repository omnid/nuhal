# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 1.0.0)
include(nuhal.cmake)
nuhal_disable_in_source_builds()
nuhal_set_default_build_type(Release)

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
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/bytestream.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/bytestream.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/encoder.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/encoder.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/error.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/error.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/led.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/led.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/pid.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/pid.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/matrix.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/matrix.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/protocol.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/protocol.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/queue.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/queue.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/time.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/time.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/uart.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/uart.c>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/utilities.c>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/utilities.c>
  )

target_include_directories(nuhal_all INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

nuhal_install(nuhal_all)
# Header files are in nuhal not nuhal_all  which is a special case because of the cross-platform nature
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nuhal DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/nuhal.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all)

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


  
