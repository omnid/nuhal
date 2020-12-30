# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), linux-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_linux VERSION 0.1.0)
find_package(nuhal_all)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)

# ROS buidls libraries as shared by default, so we use a shared library to make this compatible
option(BUILD_SHARED_LIBS "Build ${PROJECT} as a shared library" ON)

# Add platform-specific files to the nuhal library
add_library(nuhal
  src/error_host.c
  src/led_host.c
  src/time_host.c
  src/uart_host.c
  )

target_link_libraries(nuhal PRIVATE nuhal::nuhal_private cmakeme_flags PUBLIC nuhal::nuhal_public)
target_include_directories(nuhal PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
cmakeme_install(TARGETS nuhal NAMESPACE nuhal DEPENDS nuhal_all)

enable_testing()
find_package(Threads)
add_executable(nuhal_linux_test test/queue_concurrent_test.cpp)
target_link_libraries(nuhal_linux_test nuhal Threads::Threads cmakeme_flags)
add_test(NAME nuhal_linux COMMAND nuhal_linux_test)
