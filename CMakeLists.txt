# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 0.1.0 LANGUAGES NONE)

include(ExternalProject)
set(FIRMWARE_COMPILER "GNU" CACHE STRING "Select which compiler to use for the firmware: GNU or TI")
set_property(CACHE FIRMWARE_COMPILER PROPERTY STRINGS GNU TI)

include(CTest)

# Compile the library for Linux
ExternalProject_Add(nuhal_linux
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
  INSTALL_DIR install/
  BINARY_DIR nuhal_linux-build/
  BUILD_ALWAYS TRUE
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  -DBUILD_TESTING=${BUILD_TESTING}
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
  -DBUILD_TESTING=${BUILD_TESTING}
  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FIRMWARE_COMPILER}-toolchain.cmake)

# Build the Linux version of the library
#ExternalProject_Add(nuhal_linux
#  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nuhal_linux
#  INSTALL_DIR install/
#  BINARY_DIR nuhal_linux-build
#  BUILD_ALWAYS True
#  CMAKE_ARGS
#  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
#  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
#  -Dnuhal_all_DIR=${CMAKE_BINARY_DIR}/install_private/nuhal_all_linux/lib/nuhal_all
#  -DBUILD_TESTING=${BUILD_TESTING}
#  DEPENDS nuhal_all_linux)

# Build the tiva version of the library
#ExternalProject_Add(nuhal_tiva
#  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nuhal_tiva
#  INSTALL_DIR install/
#  BINARY_DIR nuhal_tiva-build
#  BUILD_ALWAYS True
#  CMAKE_ARGS
#    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
#    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
#    -DBUILD_TESTING=${BUILD_TESTING}
#    -Dnuhal_all_DIR=${CMAKE_BINARY_DIR}/install_private/nuhal_all_tiva/lib/arm-none-eabi/nuhal_all
#    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FIRMWARE_COMPILER}-toolchain.cmake
#    DEPENDS nuhal_all_tiva)

install(DIRECTORY ${CMAKE_BINARY_DIR}/install/ DESTINATION .)

if(BUILD_TESTING)
 add_test(NAME nuhal_test COMMAND ${CMAKE_COMMAND} --build nuhal_linux-build --target test)
endif()





