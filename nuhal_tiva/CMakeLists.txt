# Northwetern Utilities and Hardware Abstraction Layer (NUHAL), tiva-specific version
cmake_minimum_required(VERSION 3.14)
project(nuhal_tiva VERSION 0.1.0)

find_package(nuhal_all)
find_package(cmakeme)
cmakeme_defaults(RelWithDebInfo)
find_package(TivaCMake)

add_library(nuhal
  src/error_tiva.c
  src/led_tiva.c
  src/pin_tiva.c
  src/time_tiva.c
  src/tiva.c
  src/uart_tiva.c
  )

target_include_directories(nuhal PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  )

target_link_libraries(nuhal
  PRIVATE nuhal::nuhal_all
  PUBLIC nuhal::nuhal_flags TivaCMake::driverlib)

cmakeme_install(TARGETS nuhal driverlib NAMESPACE nuhal DEPENDS nuhal_all TivaCMake) 

# Enable testing so that test is a well-defined target
enable_testing()
add_test(NAME null_test COMMAND echo "TODO: No unit tests exist for tiva_cmake")



# Enable some integration tests. These are not automated because they require a microcontroller to be plugged in
add_executable(blank test/blank.c)


add_executable(delay_ms_test test/delay_ms_test.c)
target_link_libraries(delay_ms_test nuhal)

add_executable(delay_us_test test/delay_us_test.c)
target_link_libraries(delay_us_test nuhal)

add_executable(interrupt_test test/interrupt_test.c)
target_link_libraries(interrupt_test nuhal)

add_executable(int_priority_test test/int_priority_test.c)
target_link_libraries(int_priority_test nuhal)

add_executable(led_test test/led_test.c)
target_link_libraries(led_test nuhal)

add_executable(memcpy_speed test/memcpy_speed.c)
target_link_libraries(memcpy_speed  nuhal)

add_executable(pin_input_test test/pin_input_test.c)
target_link_libraries(pin_input_test nuhal)

add_executable(printf_error_test test/printf_error_test.c)
target_link_libraries(printf_error_test nuhal)

add_executable(uart_hw_flow_test test/uart_hw_flow_test.c)
target_link_libraries(uart_hw_flow_test  nuhal)

add_executable(uart_test test/uart_test.c)
target_link_libraries(uart_test  nuhal)

add_executable(uart_tx test/uart_tx.c)
target_link_libraries(uart_tx  nuhal)

add_executable(uart_reflect test/uart_reflect.c)
target_link_libraries(uart_reflect  nuhal)
