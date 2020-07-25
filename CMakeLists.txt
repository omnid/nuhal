# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(NUHAL)
enable_testing()

include(GNUInstallDirs)


set(INSTALL_ROOT ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/nutilities_interface)

add_library(nutilities_interface INTERFACE)

target_sources(nutilities_interface INTERFACE
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/bytestream.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/encoder.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/error.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/led.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/pid.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/queue.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/time.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/uart.c
  $<INSTALL_INTERFACE:${INSTALL_ROOT}>$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src/utilities.c
  )

target_include_directories(nutilities_interface INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  )

add_executable(nutilities_test
  test/bytestream_test.cpp
  test/encoder_test.cpp
  test/error_stub.cpp
  test/pid_test.cpp
  test/queue_test.cpp
  test/time_stub.cpp
  test/utilities_test.cpp
  )
target_link_libraries(nutilities_test PUBLIC nutilities_interface) 
add_test(NAME nutilities COMMAND nutilities_test)


install(DIRECTORY src/ DESTINATION ${INSTALL_ROOT})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS nutilities_interface
  EXPORT Nutilities_InterfaceTargets)

install(EXPORT Nutilities_InterfaceTargets DESTINATION ${CMAKE_INSTALL_DATADIR}/nutilities)
install(FILES  NutilitiesInterfaceConfig.cmake DESTINATION ${CMAKE_INSTALL_DATADIR}/nutilities)
