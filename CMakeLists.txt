# Create an nutilities_interface library that contains the source code common to all platforms
# Each platform implementation than creates and compiles the actual library tha tis needed
cmake_minimum_required(VERSION 3.14)
project(nutilities)

include(GNUInstallDirs)

add_library(nutilities_interface INTERFACE)

set(INSTALL_ROOT ${CMAKE_INSTALL_DATADIR}/nutilities_interface)

target_sources(nutilities_interface INTERFACE
  ${INSTALL_ROOT}/src/encoder.c
  ${INSTALL_ROOT}/src/error.c
  ${INSTALL_ROOT}/src/led.c
  ${INSTALL_ROOT}/src/pid.c
  ${INSTALL_ROOT}/src/queue.c
  ${INSTALL_ROOT}/src/time.c
  ${INSTALL_ROOT}/src/uart.c
  ${INSTALL_ROOT}/src/utilities.c)

target_include_directories(nutilities_interface INTERFACE ${CMAKE_INSTALL_INCLUDEDIR})


install(TARGETS nutilities_interface
  EXPORT Nutilities_InterfaceTargets)

install(FILES ${CMAKE_CURRENT DESTINATION ${INSTALL_SRC_DIR})

install(EXPORT Nutilities_InterfaceTargets DESTINATION ${CMAKE_INSTALL_DATADIR}/nutilities_interface)
install(FILES  NutilitiesInterfaceConfig.cmake DESTINATION ${CMAKE_INSTALL_DATADIR}/nutilities_interface)
