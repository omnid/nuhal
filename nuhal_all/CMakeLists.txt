# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 1.0)
enable_testing()

add_library(nuhal_all
  src/bytestream.c
  src/encoder.c
  src/error.c
  src/led.c
  src/pid.c
  src/queue.c
  src/time.c
  src/uart.c
  src/utilities.c
  )

target_include_directories(nuhal_all PUBLIC
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  )

add_executable(nuhal_test
  test/bytestream_test.cpp
  test/encoder_test.cpp
  test/error_stub.cpp
  test/pid_test.cpp
  test/queue_test.cpp
  test/time_stub.cpp
  test/utilities_test.cpp
  )
target_link_libraries(nuhal_test PUBLIC nuhal_all) 
add_test(NAME nuhal_all COMMAND nuhal_test)




include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS nuhal_all
  EXPORT ${PROJECT_NAME}-target
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_NAME}-config-version.cmake
  COMPATIBILITY SameMajorVersion
  )

# Used in case we need to export directories from NuhalConfig.cmake
configure_package_config_file(${PROJECT_NAME}-config.cmake.in ${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/${PROJECT_NAME} PATH_VARS)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/${PROJECT_NAME})
  
