/// \file
/// \brief stub functions for the platform-specific uart
#include<stdexcept>
#include"nuhal/uart.h"

bool uart_wait_for_data(const struct uart_port *, uint32_t)
{
    throw std::logic_error("uart_wait_for_data is a stub function");
}

int uart_read_nonblock(const struct uart_port * , void  * , size_t )
{
    throw std::logic_error("uart_read_nonblock is a stub function");
}

int uart_write_nonblock(const struct uart_port * , const void * , size_t )
{
    throw std::logic_error("uart_write_nonblock is a stub function");
}
