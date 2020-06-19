# Tip from https://cmake.org/pipermail/cmake/2017-October/066366.html
# make find_package "nice" for within a configure
include(CMakeFindDependencyMacro)

include("${CMAKE_CURRENT_LIST_DIR}/common-target.cmake")
# use find_dependcy to allow loading a dependency
#find_dependency(dependency)


# Whenever make all (the default target) is built, update git_hash.h
add_custom_target(git_hash ALL
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/make_git_hash_h.sh ${CMAKE_CURRENT_BINARY_DIR}/include/common/git_hash.h
  COMMENT "Updating git_hash.h"
  BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/include/common/git_hash.h
  )

# depent on git_hash so that git_hash.h will be modified before common is built
add_dependencies(common git_hash)
