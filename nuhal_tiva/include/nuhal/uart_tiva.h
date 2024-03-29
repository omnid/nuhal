#ifndef NUHAL_IMPL_UART_TIVA_H_INCLUDE_GUARD
#define NUHAL_IMPL_UART_TIVA_H_INCLUDE_GUARD
/// @brief tiva-specific api for uarts

#include "nuhal/uart.h"
#include "driverlib/uart.h"

/// @brief pass through the traffic between port1 and port2.
/// all data received on port1 will be transmitted to port2
/// all data received on port2 will be transmitted to port1
/// This function will only return if a break character is sent to port 1.
/// While it is running, it expects to have exclusive access to port1 and port2
/// A break consists of all zeros, followed by a zero for the stop bit 
/// @param port1 - one of the ports
/// @param port2 - another one of the ports
/// @param timeout - time in ms to wait for break condition to clear
///   if more than this time elapses, a fatal error ocdcurs. if 0, the
///   timeout is disabled
void uart_passthrough(const struct uart_port * port1,
                      const struct uart_port * port2,
                      uint32_t timeout);

/// @brief enables or disables the uart receive functionality
/// @param port - the port
/// @param enable - if true, the uart receiver is enabled. If false, the uart
///    receiver is disabled
void uart_set_receive_enable(const struct uart_port * port, bool enable);

/// @brief sends a single character over the uart, then waits until the uart is
///    finished transmitting all characters in the buffer,
///    including the stop bit of the last character. This function has no timeout
///    and will block indefinitely if the transmitter cannot send the character!
/// @param port - the port
/// @param data - the single character (8 bits) to be written
/// @param timeout - time in ms to wait for new characters.
///   if more than this time elapses, a fatal error occurs. if 0,
///   the timeout is disabled 
/// @post errors (including timeouts) result in program termination
void uart_transmit_single_block(const struct uart_port * port, const uint8_t data);

#endif
