# Tip from https://cmake.org/pipermail/cmake/2017-October/066366.html
# make find_package "nice" for within a configure
include(CMakeFindDependencyMacro)

include("${CMAKE_CURRENT_LIST_DIR}/common-target.cmake")
# use find_dependcy to allow loading a dependency
#find_dependency(dependency)

