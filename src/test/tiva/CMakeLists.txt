# Enable testing so that test is a well-defined target
include(CTest)
if(BUILD_TESTING)
  add_test(NAME null_test COMMAND echo "TODO: No unit tests exist for tiva_cmake")

  # Enable some integration tests. These are not automated because they require a microcontroller to be plugged in
  add_executable(blank blank.c)


  add_executable(delay_ms_test delay_ms_test.c)
  target_link_libraries(delay_ms_test nuhal cmakeme_flags)

  add_executable(delay_us_test delay_us_test.c)
  target_link_libraries(delay_us_test nuhal cmakeme_flags)

  add_executable(interrupt_test interrupt_test.c)
  target_link_libraries(interrupt_test nuhal cmakeme_flags)

  add_executable(int_priority_test int_priority_test.c)
  target_link_libraries(int_priority_test nuhal cmakeme_flags)

  add_executable(led_test led_test.c)
  target_link_libraries(led_test nuhal cmakeme_flags)

  add_executable(memcpy_speed memcpy_speed.c)
  target_link_libraries(memcpy_speed  nuhal cmakeme_flags)

  add_executable(pin_input_test pin_input_test.c)
  target_link_libraries(pin_input_test nuhal cmakeme_flags)

  add_executable(printf_error_test printf_error_test.c)
  target_link_libraries(printf_error_test nuhal cmakeme_flags)

  add_executable(uart_hw_flow_test uart_hw_flow_test.c)
  target_link_libraries(uart_hw_flow_test  nuhal cmakeme_flags)

  add_executable(uart_test uart_test.c)
  target_link_libraries(uart_test  nuhal cmakeme_flags)

  add_executable(uart_tx uart_tx.c)
  target_link_libraries(uart_tx  nuhal cmakeme_flags)

  add_executable(uart_reflect uart_reflect.c)
  target_link_libraries(uart_reflect  nuhal cmakeme_flags)
endif()

