# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), tiva-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_tiva VERSION 0.1.0)

find_package(nuhal_all)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)
find_package(TivaCMake)

add_library(nuhal
  src/error_tiva.c
  src/led_tiva.c
  src/pin_tiva.c
  src/time_tiva.c
  src/tiva.c
  src/uart_tiva.c
  )

target_include_directories(nuhal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

target_link_libraries(nuhal
  PRIVATE nuhal::nuhal_all
  PUBLIC nuhal::nuhal_flags TivaCMake::driverlib)

cmakeme_install(TARGET nuhal
                NAMESPACE nuhal
                INCLUDEDIRS ${CMAKE_CURRENT_LIST_DIR}/include/nuhal
                ARCH_INDEPENDENT
                DEPENDS driverlib
                CONFIG nuhal-config.cmake.in
                )
