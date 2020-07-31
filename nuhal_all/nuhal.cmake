# Default cmake settings and options.
# type - the default release type
function(nuhal_defaults type)
  nuhal_disable_in_source_builds()
  nuhal_set_default_build_type(type)
  nuhal_no_lang_extensions()
endfunction()

# set a default build type if not specified and store it in the cache
# type - the build type to set
function(nuhal_set_default_build_type type)
  set(DEFAULT_BUILD_TYPE ${type})
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE
      STRING "Type of build." FORCE)

    message(STATUS "CMAKE_BUILD_TYPE=\"\": Defaulting to ${DEFAULT_BUILD_TYPE}")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
  endif()
endfunction()

# disable in source builds
function(nuhal_disable_in_source_builds)
  if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
      "No in source builds allowed. Create a separate build directory.
       SOURCE_DIR=${CMAKE_SOURCE_DIR}  BINARY_DIR=${CMAKE_BINARY_DIR} ")
  endif()
endfunction()

# disable c/c++ language extensions
function(nuhal_no_lang_extensions)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_C_EXTENSIONS OFF)
endfunction()

# Install a cmake target-specific library to the proper directories
include(GNUInstallDirs)

# Install a target with the given name, according to nuhal conventions
# name - The name of the target.
# findable - if true, export information so that other projects can call find_package(name) and use the targets
#             you must then provide a file called ${name}-config.cmake.in that can be configured
#             with configure_package_config_file(). It should include ${name}-target.cmake
#             and use find_dependency() to bring in all of the dependencies
#           if the target has INTEFACE_SOURCES it will be considered architecture independent
function(nuhal_install name findable)
  install(TARGETS ${name} 
    EXPORT ${name}-target
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION  ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

  if(findable)
    install(EXPORT ${name}-target
      NAMESPACE nuhal::
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/${name}
      )

    get_target_property(interface ${name} INTERFACE_SOURCES)
    
    include(CMakePackageConfigHelpers)
    if(interface)
      write_basic_package_version_file(
        ${name}-config-version.cmake
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT
        )
    else()
      write_basic_package_version_file(
        ${name}-config-version.cmake
        COMPATIBILITY SameMajorVersion
        )
    endif()

    # Used in case we need to export directories from NuhalConfig.cmake
    configure_package_config_file(${name}-config.cmake.in ${name}-config.cmake
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${name} PATH_VARS)

    install(FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/${name}-config-version.cmake
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/${name})
  endif()
endfunction()

# Whenever make all (the default target) is built, update git_hash.h
#add_custom_target(git_hash ALL
#  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/make_git_hash_h.sh ${CMAKE_CURRENT_BINARY_DIR}/include/common/git_hash.h
#  COMMENT "Updating git_hash.h"
#  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/include/common/git_hash.h
#  )

# depent on git_hash so that git_hash.h will be modified before common is built
#add_dependencies(common git_hash)
