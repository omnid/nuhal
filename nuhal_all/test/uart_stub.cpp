/// \file
/// \brief stub functions for the platform-specific uart
/// Used to enable testing of the core code without requiring
/// platform-specific code
#include<stdexcept>
#include"nuhal/uart.h"

const struct uart_port *
uart_open(const char [], unsigned int,
          enum uart_flow, enum uart_parity )
{
    throw std::logic_error("uart_open is a stub function");
}

bool uart_wait_for_data(const struct uart_port *, uint32_t)
{
    throw std::logic_error("uart_wait_for_data is a stub function");
}

bool uart_data_available(const struct uart_port * )
{
    throw std::logic_error("uart_data_available is a stub function");
}

int uart_read_nonblock(const struct uart_port * , void  * , size_t )
{
    throw std::logic_error("uart_read_nonblock is a stub function");
}

int uart_write_nonblock(const struct uart_port * , const void * , size_t )
{
    throw std::logic_error("uart_write_nonblock is a stub function");
}
