#ifndef NUTILITIES_X64_LINUX_UART_HOST_INCLUDE_GUARD
#define NUTILITIES_X64_LINUX_UART_HOST_INCLUDE_GUARD
/// @file
/// @brief host-specific uart functions

#ifdef __cplusplus
extern "C" {
#endif

/// @brief acquire a lock on the uart.  no other program
/// may access the uart while the lock is held.
/// Useful to mediate multiple programs accessing the uart
/// simultaneously. uart_lock blocks until the lock is acquired
/// locking is cooperative, so a program that does not acquire the lock can still access the port
/// @param port - the uart port to obtain exclusive access to
void uart_lock(const struct uart_port * port);

/// @brief release a lock on the uart
/// @param port - the uart port to obtain exclusive access to
void uart_unlock(const struct uart_port * port);

#ifdef __cplusplus
}
#endif

#endif
