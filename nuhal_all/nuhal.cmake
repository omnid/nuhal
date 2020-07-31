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
