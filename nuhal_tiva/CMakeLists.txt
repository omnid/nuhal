# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), tiva-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_tiva VERSION 0.1.0)

find_package(nuhal_all)
nuhal_defaults(RelWithDebInfo)

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
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

# TivaCMake::startup is an interface library so that the executable it links against
# will include all fo the startup code and it is not just incorporated into the archive
# If you make a library that links PUBLIC on nuhal, you will also need to explicitly link
# against TivaCMake::startup as an interface library if you want resulting executables
# to properly include the startup code
target_link_libraries(nuhal
  PRIVATE nuhal::nuhal_all
  PUBLIC nuhal::nuhal_flags TivaCMake::driverlib
  INTERFACE TivaCMake::startup)  
nuhal_install(nuhal driverlib)

