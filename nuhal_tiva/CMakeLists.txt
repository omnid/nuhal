# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), tiva-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_tiva VERSION 1.0.0)

find_package(nuhal_all)
nuhal_defaults(Release)

find_package(TivaCMake)

add_library(nuhal_tiva
  src/error_tiva.c
  src/led_tiva.c
  src/pin_tiva.c
  src/time_tiva.c
  src/tiva.c
  src/uart_tiva.c
  )

target_include_directories(nuhal_tiva PUBLIC
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )


target_link_libraries(nuhal_tiva PUBLIC nuhal::nuhal_all TivaCMake::driverlib)
