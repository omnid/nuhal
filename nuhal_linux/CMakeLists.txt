# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), linux-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_linux VERSION 0.1.0)
find_package(nuhal_all)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)

add_library(nuhal_obj OBJECT
  src/error_host.c
  src/led_host.c
  src/time_host.c
  src/uart_host.c
  )
target_link_libraries(nuhal_obj PRIVATE nuhal::nuhal_all PUBLIC nuhal::nuhal_flags)
target_include_directories(nuhal_obj PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

# create BUILD_SHARED_LIBRARY dependent and shared versions
add_library(nuhal $<TARGET_OBJECTS:nuhal_obj>)
target_link_libraries(nuhal INTERFACE nuhal_obj)

add_library(nuhal_shared $<TARGET_OBJECTS:nuhal_obj>)
target_link_libraries(nuhal_shared INTERFACE nuhal_obj)


cmakeme_install(TARGETS nuhal nuhal_shared nuhal_obj NAMESPACE nuhal DEPENDS nuhal_all )

enable_testing()
find_package(Threads)
add_executable(nuhal_linux_test test/queue_concurrent_test.cpp)
target_link_libraries(nuhal_linux_test nuhal Threads::Threads)
add_test(NAME nuhal_linux COMMAND nuhal_linux_test)
