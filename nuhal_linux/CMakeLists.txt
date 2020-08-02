# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), linux-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_linux VERSION 0.1.0)
find_package(nuhal_all)
nuhal_defaults(Release)

add_library(nuhal_linux
  src/error_host.c
  src/led_host.c
  src/time_host.c
  src/uart_host.c
  )

target_link_libraries(nuhal_linux PUBLIC nuhal::nuhal_all)

nuhal_install(nuhal_linux)
# Header files are in nuhal not nuhal_tiva so they don't get installed by nuhal_install
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nuhal DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
