cmake_minimum_required(VERSION 3.14)
project(nuhal_test_linux)

include(CTest)

if(BUILD_TESTING)
  find_package(cmakeme)
  cmakeme_defaults(Debug)

  find_package(Threads)
  find_package(nuhal)

  # These are hardware independent tests
  # That test functionality common to all platforms
  # They use stub functions to handle any hardware-specific calls
  add_executable(nuhal_common_test
    common/bytestream_test.cpp
    common/encoder_test.cpp
    common/error_stub.cpp
    common/led_stub.cpp
    common/matrix_test.cpp
    common/pid_test.cpp
    common/queue_test.cpp
    common/time_stub.cpp
    common/uart_stub.cpp
    common/utilities_test.cpp
    common/error_test.cpp
  )
  target_link_libraries(nuhal_common_test PRIVATE nuhal::nuhal cmakeme_flags)
  target_compile_definitions(nuhal_common_test PRIVATE UNIT_TEST_MODE)
  add_test(NAME nuhal_common_test COMMAND nuhal_common_test)

  # This test needs the real functionality of the host systej
  add_executable(nuhal_linux_test
    host/queue_concurrent_test.cpp)
  target_link_libraries(nuhal_linux_test PRIVATE nuhal::nuhal cmakeme_flags Threads::Threads)
  add_test(NAME nuhal_linux_test COMMAND  nuhal_linux_test)

endif()
