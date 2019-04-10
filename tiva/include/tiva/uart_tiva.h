#ifndef COMMON_IMPL_UART_TIVA_H_INCLUDE_GUARD
#define COMMON_IMPL_UART_TIVA_H_INCLUDE_GUARD
/// @brief tiva-specific api for uarts

#include "common/uart.h"
#include "driverlib/uart.h"

/// @brief pass through the traffic between port1 and port2.
/// all data received on port1 will be transmitted to port2
/// all data received on port2 will be transmitted to port1
/// This function will only return if a break character is sent to port 1.
/// While it is running, it expects to have exclusive access to port1 and port2
/// A break consists of all zeros, followed by a zero for the stop bit 
/// @param port1 - one of the ports
/// @param port2 - another one of the ports
void uart_passthrough(const struct uart_port * port1,
                      const struct uart_port * port2);

/// @brief enables or disables the uart receive functionality
/// @param port - the port
/// @param enable - if true, the uart receiver is enabled. If false, the uart
///    receiver is disabled
void uart_set_receive_enable(const struct uart_port * port, bool enable);

/// @brief waits until the uart is finished transmitting all characters in the buffer,
///    including the stop bit of the last character
/// @param port - the port
/// @param timeout - time in ms to wait for new characters.
///   if more than this time elapses, a fatal error occurs. if 0,
///   the timeout is disabled 
/// @post errors (including timeouts) result in program termination
void uart_transmit_sync(const struct uart_port * port, uint32_t timeout);

#endif
