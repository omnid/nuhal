# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal_outer LANGUAGES NONE)

include(ExternalProject)
set(FIRMWARE_COMPILER "GNU" CACHE STRING "Select which compiler to use for the firmware: GNU or TI")
set_property(CACHE FIRMWARE_COMPILER PROPERTY STRINGS GNU TI)

include(CTest)

# Compile the library for Linux
ExternalProject_Add(nuhal_linux
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
  INSTALL_DIR install/
  BUILD_ALWAYS TRUE
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  )

# Compile the library for tiva
ExternalProject_Add(nuhal_tiva
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
  INSTALL_DIR install/
  BUILD_ALWAYS TRUE
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FIRMWARE_COMPILER}-toolchain.cmake)

install(DIRECTORY ${CMAKE_BINARY_DIR}/install/ DESTINATION .)

if(BUILD_TESTING)
  ExternalProject_Add(nuhal_test_linux
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/linux
    BINARY_DIR nuhal_test_linux
    BUILD_ALWAYS TRUE
    INSTALL_COMMAND ""
    CMAKE_ARGS
    -DBUILD_TESTING=ON
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -Dnuhal_DIR=${CMAKE_BINARY_DIR}/install/lib/nuhal
    DEPENDS nuhal_linux)

  # Compile the tiva tests
  ExternalProject_Add(nuhal_test_tiva
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/tiva
    BINARY_DIR nuhal_test_tiva
    BUILD_ALWAYS TRUE
    INSTALL_COMMAND ""
    CMAKE_ARGS
    -DBUILD_TESTING=ON
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -Dnuhal_DIR=${CMAKE_BINARY_DIR}/install/lib/arm-none-eabi/nuhal
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FIRMWARE_COMPILER}-toolchain.cmake
    DEPENDS nuhal_tiva)

 add_test(NAME nuhal_test COMMAND ${CMAKE_COMMAND} --build nuhal_test_linux --target test)
endif()





