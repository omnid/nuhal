# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 0.1.0 LANGUAGES NONE)

include(ExternalProject)
set(FIRMWARE_COMPILER "GNU" CACHE STRING "Select which compiler to use for the firmware: GNU or TI")
set_property(CACHE FIRMWARE_COMPILER PROPERTY STRINGS GNU TI)


# Compile the common code for the linux system
ExternalProject_Add(nuhal_all_linux
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nuhal_all
  INSTALL_DIR install_private/nuhal_all_linux
  BUILD_ALWAYS TRUE
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  -DBUILD_TESTING=${BUILD_TESTING}
  STEP_TARGETS test
  TEST_EXCLUDE_FROM_MAIN TRUE)

# Compile the common code for the tiva system
ExternalProject_Add(nuhal_all_tiva
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nuhal_all
  INSTALL_DIR install_private/nuhal_all_tiva
  BUILD_ALWAYS TRUE
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FIRMWARE_COMPILER}-toolchain.cmake)

# Build the Linux version of the library
ExternalProject_Add(nuhal_linux
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nuhal_linux
  INSTALL_DIR install/nuhal_linux
  BUILD_ALWAYS True
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
  -Dnuhal_all_DIR=${CMAKE_BINARY_DIR}/install_private/nuhal_all_linux/lib/nuhal_all
  STEP_TARGETS test
  TEST_EXCLUDE_FROM_MAIN True
  DEPENDS nuhal_all_linux)

# Build the tiva version of the library
ExternalProject_Add(nuhal_tiva
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nuhal_tiva
  INSTALL_DIR install/nuhal_tiva
  BUILD_ALWAYS True
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -Dnuhal_all_DIR=${CMAKE_BINARY_DIR}/install_private/nuhal_all_tiva/lib/arm-none-eabi/nuhal_all
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${FIRMWARE_COMPILER}-toolchain.cmake
    DEPENDS nuhal_all_tiva)

#enable_testing()
#add_test(NAME nuhal_all_test COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target nuhal_all-test)
#add_test(NAME nuhal_linux_test COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target nuhal_linux-test)


#install(DIRECTORY ${CMAKE_BINARY_DIR}/install/ DESTINATION ${CMAKE_INSTALL_PREFIX})


