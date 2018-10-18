#ifndef COMMON_HOST_UART_HOST_INCLUDE_GUARD
#define COMMON_HOST_UART_HOST_INCLUDE_GUARD
/// @file
/// @brief host-specific uart functions

#ifdef __cplusplus
extern "C" {
#endif

/// @brief acquire a lock on the uart.  no other program
/// may access the uart while the lock is held.
/// this is useful to mediate multiple programs accessing the uart
/// simultaneously (for example, a ros node and packet_console.
/// uart_lock will block until the lock is acquired
/// @param port - the uart port to obtain exclusive access to
void uart_lock(const struct uart_port * port);

/// @brief release a lock on the uart
/// @param port - the uart port to obtain exclusive access to
void uart_unlock(const struct uart_port * port);

#ifdef __cplusplus
}
#endif

#endif
