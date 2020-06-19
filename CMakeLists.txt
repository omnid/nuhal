# Create an nutilities_interface library that contains the source code common to all platforms
# Each platform implementation than creates and compiles the actual library that is needed
# Thus this project does not compile anything, it just installs some source files that can be used by other projects
# The reason for this is that we do not know a priori what compilers need to be used
cmake_minimum_required(VERSION 3.14)
project(nutilities)
enable_testing()

include(GNUInstallDirs)

add_library(nutilities_interface INTERFACE)

add_executable(nutilities_test src/utilities.c src/bytestream.c test/bytestream_test.cpp)
target_include_directories(nutilities_test PUBLIC include/)
add_test(NAME nutilities COMMAND nutilities_test)


set(INSTALL_ROOT ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/nutilities_interface)

target_sources(nutilities_interface INTERFACE
  ${INSTALL_ROOT}/src/encoder.c
  ${INSTALL_ROOT}/src/error.c
  ${INSTALL_ROOT}/src/led.c
  ${INSTALL_ROOT}/src/pid.c
  ${INSTALL_ROOT}/src/queue.c
  ${INSTALL_ROOT}/src/time.c
  ${INSTALL_ROOT}/src/uart.c
  ${INSTALL_ROOT}/src/utilities.c)

target_include_directories(nutilities_interface INTERFACE ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY src/ DESTINATION ${INSTALL_ROOT})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS nutilities_interface
  EXPORT Nutilities_InterfaceTargets)

install(EXPORT Nutilities_InterfaceTargets DESTINATION ${CMAKE_INSTALL_DATADIR}/nutilities)
install(FILES  NutilitiesInterfaceConfig.cmake DESTINATION ${CMAKE_INSTALL_DATADIR}/nutilities)
