# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_all VERSION 0.1.0)
include(nuhal.cmake)
nuhal_defaults(RelWithDebInfo)

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

target_include_directories(nuhal_flags INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )
# This library is a pure compile-time library. It just brings in sources that must be compiled
# by the platform-specific libraries. We need different locations depending on if the source code is
# installed or just being used from the build directory
add_library(nuhal_all INTERFACE)

# Add source files to this list.  
set(nuhal_all_src bytestream.c;encoder.c;error.c;led.c;pid.c;matrix.c;protocol.c;queue.c;time.c;uart.c;utilities.c)

# Each source is added to a different path in the build and install configurations
foreach(src ${nuhal_all_src})
  target_sources(nuhal_all INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/nuhal_all/src/${src}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/${src}>)
endforeach()


nuhal_install(nuhal_all nuhal_flags)
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


  
