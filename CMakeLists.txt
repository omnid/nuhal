# Code Layout (directories relative to all/common):
# The common library contains basic functionality that is
# shared by multiple platforms. Most of the code here is divided
# into a generic interface and a platform-specific component.
# /              common library implmentation
# /include       api for common library, works for all platforms
# host          host-specific implementation of common library
# tiva          tiva-specific implementation of common library
# tiva/include  tiva-specific extensions of the common library
# Functionality included in platform-specific common library extensions should
# be both generally useful and necessary for implementing the common library.
# If code is not generally useful, it should not be made public as an extension
# If code is not necessary for implementing the common library, it should be
# in a separate platform-specific library and not included in common.

cmake_minimum_required(VERSION 3.9)
project(common C)

# touch timestamp.h so any file including it gets recompiled
add_custom_target(timestamp ALL
  COMMAND ${CMAKE_COMMAND} -E touch "${PROJECT_SOURCE_DIR}/include/common/timestamp.h"
  COMMENT "Updating timestamp.h"
  )

# Add files that are compiled for any platform
add_library(common
  utilities.c uart.c time.c queue.c error.c
  motor.c bytestream.c pid.c delta_robot.c
  encoder.c type.c matrix.c)
add_dependencies(common timestamp)

# host-specific sources
target_sources(common PRIVATE
  $<$<PLATFORM_ID:${CMAKE_HOST_SYSTEM_NAME}>:
  host/error_host.c host/time_host.c host/uart_host.c>)

# tiva-specific sources
target_sources(common PRIVATE
  $<$<PLATFORM_ID:arm-none-eabi>:
  tiva/tiva.c tiva/error_tiva.c
  tiva/isr_tiva.c tiva/time_tiva.c tiva/uart_tiva.c tiva/pin_tiva.c>)

#suppress warning about inline asm disabling link time optimization
#or using weak symbols when using the ti compiler
set_source_files_properties(queue.c error.c
  PROPERTIES COMPILE_FLAGS $<$<C_COMPILER_ID:TI>:-pds=1463>)

# don't issue a warning about link time optimization being disabled
# for files with weak symbols, when using the ti compiler
set_source_files_properties(tiva/isr_tiva.c tiva/error_tiva.c
  PROPERTIES COMPILE_FLAGS $<$<C_COMPILER_ID:TI>:-pds=1463>)

# on the embedded platform, linking against common provides the proper linker script
target_link_libraries(common PUBLIC 
  $<$<PLATFORM_ID:arm-none-eabi>: mytivaware
    $<$<C_COMPILER_ID:TI>:${PROJECT_SOURCE_DIR}/tiva/tiva_ti_cgt.cmd>
    $<$<C_COMPILER_ID:GNU>:-lm -Wl,-T${PROJECT_SOURCE_DIR}/tiva/tiva_arm_gcc.lds>
    >
  )

# Set up header file paths.
# include/ headers are available on all platforms
# include/tiva headers are only added for the embedded platforms
target_include_directories(common PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<$<PLATFORM_ID:arm-none-eabi>:$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/tiva/include>>
  $<INSTALL_INTERFACE:include>
  )

#use gnuinstalldirs to get the correct directories
#gnuinstalldirs defines CMAKE_INSTAL_xxxx directories
include(GNUInstallDirs)

# add platform specific files. Basically we need to only do an install
# for the host target
if(${CMAKE_SYSTEM_NAME} STREQUAL ${CMAKE_HOST_SYSTEM_NAME})

  # add include directories for host-specific extensions
  # currently there are none
  #target_include_directories(common PUBLIC
  #  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/host/include>
  #  $<INSTALL_INTERFACE:host/include>
  #  PRIVATE host)


  # make install target, and also create the export target
  # we export common-target.cmake and then provide our own
  # common-config.cmake that includes common-target and
  # also finds the dependencies.  This prevents the client
  # from needing to manually include dependencies
  install(TARGETS common EXPORT common-target
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  # install the public headers
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # host-specific include files. Currently there are none
  #install(DIRECTORY host/include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # make the project importable from the install directory
  install(EXPORT common-target DESTINATION share/common/cmake)
  install(FILES common-config.cmake DESTINATION share/common/cmake)

  # make the project importable from the build directory
  export(TARGETS common FILE common-config.cmake)
endif()
