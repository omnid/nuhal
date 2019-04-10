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


#endif
