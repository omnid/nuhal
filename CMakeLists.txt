# Northwetern Utilities and Hardware Abstraction Layer (NUHAL)
# This is the top-level cmake file, which is here to conveniently build all the sub-libraries with a single command
# without using a ROS build tool like catkin. This CMakeLists.txt file is not used when compiling in ROS, instead
# each individual sub-folder is compiled as a separate project
cmake_minimum_required(VERSION 3.14)
project(nuhal VERSION 0.1.0 LANGUAGES NONE)
include(ExternalProject)

# Compile each project as a sub-project
ExternalProject_Add(
    nuhal_all
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/nuhal_all
    CMAKE_ARGS
    -G${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    )

list(APPEND prefix ${CMAKE_BINARY_DIR}/install ${CMAKE_PREFIX_PATH})
# Use in conjunction with LIST_SEPARATOR to
# "escape" ; when passing to ExternalProject_Add
string(REPLACE ";" "|" prefix "${prefix}")

ExternalProject_Add(
    nuhal_linux
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/nuhal_linux
    LIST_SEPARATOR |
    CMAKE_ARGS
    -G${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
    -DCMAKE_PREFIX_PATH=${prefix}
    DEPENDS nuhal_all
    )

ExternalProject_Add(
    nuhal_tiva
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/nuhal_tiva
    LIST_SEPARATOR |
    CMAKE_ARGS
    -G${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
    -DCMAKE_PREFIX_PATH=${prefix}
    DEPENDS nuhal_all
    )

install(DIRECTORY ${CMAKE_BINARY_DIR}/install/ DESTINATION ${CMAKE_INSTALL_PREFIX})

