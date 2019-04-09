#include "common/uart.h"
#include "common/error.h"
#include "tiva/tiva.h"
#include "common/time.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include <stdio.h>
#include <string.h>

struct uart_port
{
    uint32_t base;      // the base uart register
    uint32_t sysctl;    // the peripheral as per the sysctl module
    uint32_t int_base; // the interrupt value for the interrupt controller
};

// list of the ports that can be opened
static const struct uart_port ports[] = {
    {UART0_BASE, SYSCTL_PERIPH_UART0, INT_UART0},
    {UART1_BASE, SYSCTL_PERIPH_UART1, INT_UART1},
    {UART2_BASE, SYSCTL_PERIPH_UART2, INT_UART2},
    {UART3_BASE, SYSCTL_PERIPH_UART3, INT_UART3},
    {UART4_BASE, SYSCTL_PERIPH_UART4, INT_UART4},
    {UART5_BASE, SYSCTL_PERIPH_UART5, INT_UART5},
    {UART6_BASE, SYSCTL_PERIPH_UART6, INT_UART6},
    {UART7_BASE, SYSCTL_PERIPH_UART7, INT_UART7}
};

void uart_passthrough(const struct uart_port * port1,
                      const struct uart_port * port2)
{
    if(!port1 || !port2)
    {
        error(FILE_LINE, "NULL ptr");
    }

    for(;;)
    {
        const int rx1 = UARTCharGetNonBlocking(port1->base);
        // exit the loop if we get a BREAK signal
        if(UARTRxErrorGet(port1->base) & UART_RXERROR_BREAK)
        {
            UARTRxErrorClear(port1->base);
            break;
        }

        if(rx1 >= 0)
        {
            const uint8_t rxchar = rx1;
            uart_write_nonblock(port2, &rxchar, 1);
        }


        const int rx2 = UARTCharGetNonBlocking(port2->base);
        if(rx2 >= 0)
        {
            const uint8_t rxchar = rx2;
            uart_write_nonblock(port1, &rxchar, 1);
        }
    }
}

/// @brief open the tiva uart
/// @param name - is the uart number '0' - '8'
/// @param baud - is the baud of the uart
/// @param flow - flow control setting. Hardware is only supported on UART1
///   and software flow control is not implemented yet
/// @param parity - even, odd, or no parity
/// NOTE: The pins that are used for the UART must be setup using
/// the TIVA_PIN_SETUP array and tiva_setup.  On reset, UART0 pins
/// are setup for UART already so it is only the other UARTs that need this
const struct uart_port * uart_open(const char name[], unsigned int baud,
                                   enum uart_flow flow, enum uart_parity parity)
{

    const int pindex = name[0] - '0';
    if(pindex < 0 || (size_t)pindex >= ARRAY_LEN(ports))
    {
        error(FILE_LINE, "Invalid UART port specified");
    }

    tiva_peripheral_enable(ports[pindex].sysctl);

    uint32_t parset = 0;
    switch(parity)
    {
    case UART_PARITY_ODD:
        parset = UART_CONFIG_PAR_ODD;
        break;
    case UART_PARITY_EVEN:
        parset = UART_CONFIG_PAR_EVEN;
        break;
    case UART_PARITY_NONE:
        parset = UART_CONFIG_PAR_NONE;
        break;
    default:
        error(FILE_LINE, "Unsupported parity setting.");
        break;
    }


    UARTConfigSetExpClk(ports[pindex].base,
                        tiva_clock_hz(), // clock speed for the uart
                        baud,
                        UART_CONFIG_WLEN_8     // 8 data bits
                        | UART_CONFIG_STOP_ONE // 1 stop bit
                        | parset // the desired parity
        );
    // enable the uart fifos
    UARTFIFOEnable(ports[pindex].base);

    // only uart 1 has flow control
    if(pindex == 1)
    {
        switch(flow)
        {
        case UART_FLOW_HARDWARE:
            UARTFlowControlSet(ports[pindex].base,
                               UART_FLOWCONTROL_TX | UART_FLOWCONTROL_RX);
            break;
        case UART_FLOW_SOFTWARE:
            error(FILE_LINE, "Software flow control unimplemented.");
            break;
        case UART_FLOW_NONE:
            UARTFlowControlSet(ports[pindex].base, UART_FLOWCONTROL_NONE);
            break;
        default:
            error(FILE_LINE, "Unknown flow control mode");
            break;
        }
    }
    else
    {
        // we don't implement software flow control
        if(UART_FLOW_NONE != flow)
        {
            error(FILE_LINE, "Flow control not implemented on this uart");
        }
    }

    // flush the uart to eliminate any spurious data
    /// NOTE: this replaces code that simply delayed 500ms for all electrical
    /// transients to settle out prior to starting the uart

    /// time for a single character to arrive, in us, assuming 10 bits
    /// per byte (byte + 1 stop and 1 start bit)
    const uint32_t byte_us = 10000000/baud;
    time_delay_us(byte_us);
    while(UARTCharsAvail(ports[pindex].base))
    {
        (void)UARTCharGetNonBlocking(ports[pindex].base);
        time_delay_us(byte_us);
    }
    UARTRxErrorClear(ports[pindex].base);

    return &ports[pindex];
}

int uart_read_nonblock(const struct uart_port * port, void * data, size_t len)
{
    if(!port)
    {
        error(FILE_LINE, "Null pointer");
    }

    size_t index = 0;
    // note: the docs specifically say to call UARTCharsAvail prior
    // to calling UARTCharsAvail, even though UARTCHarGetNonBlocking
    // returns -1 if there are no chars available.  
    while(UARTCharsAvail(port->base) && index < len)
    {
        const int32_t nextbyte = UARTCharGetNonBlocking(port->base);
        const uint32_t errors = UARTRxErrorGet(port->base);
        if(errors)
        {
            UARTRxErrorClear(port->base);
            error(FILE_LINE, "Uart read error");
        }
        if(nextbyte == -1)
        {
            return index;
        }
        ((uint8_t*)data)[index] = nextbyte;
        ++index;
    }

    return index;
}

int uart_write_nonblock(const struct uart_port * port, const void * data,
                        size_t len)
{
    if(!port)
    {
        error(FILE_LINE, "Null pointer");
    }
    for(size_t index = 0; index != len; ++index)
    {
        // the fifo is full
        if(!UARTCharPutNonBlocking(port->base, ((uint8_t*)data)[index]))
        {
            // terminate early so we don't block
            return index;
        }
    }
    return len;
}


void uart_close(const struct uart_port * port)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }
    UARTFIFODisable(port->base);
    UARTDisable(port->base);
}


void uart_transmit_sync(const struct uart_port * port, uint32_t timeout)
{
    if(!port)
    {
        error(FILE_LINE, "Null pointer");
    }
    struct time_elapsed_ms stamp = time_elapsed_ms_init();

    while(0 == timeout || time_elapsed_ms(&stamp) < timeout)
    {
        if(!UARTBusy(port->base))
	    {
            return;
	    }
    }
    // if we get here we have timed out
    error(FILE_LINE, "Timeout on transmit sync.");
    return;
}

void uart_rx_clear(const struct uart_port * port, uint32_t timeout)
{
    if(!port)
    {
        error(FILE_LINE, "Null pointer");
    }
    struct time_elapsed_ms stamp = time_elapsed_ms_init();
    while(0 == timeout || time_elapsed_ms(&stamp) < timeout)
    {
        if(UARTCharsAvail(port->base)) {
            UARTCharGetNonBlocking(port->base);
        }
        else
        {
            UARTRxErrorClear(port->base);
            return;
        }
    }
    // if we get here we have timed out
    error(FILE_LINE, "Timeout on transmit sync.");
    return;
}

bool uart_wait_for_data(const struct uart_port * port, uint32_t timeout)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }
    struct time_elapsed_ms stamp = time_elapsed_ms_init();
    while(!UARTCharsAvail(port->base))
    {
        if(time_elapsed_ms(&stamp) > timeout && timeout != 0)
        {
            return false;
        }
    }
    return true;
}

bool uart_data_available(const struct uart_port * port)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }
    return UARTCharsAvail(port->base);
}

void uart_send_break(const struct uart_port * port, uint32_t timeout)
{
    if(!port)
    {
        error(FILE_LINE, "NULL ptr");
    }
    UARTBreakCtl(port->base, true);
    if(timeout < 2)
    {
        time_delay_ms(2);
    }
    else
    {
        time_delay_ms(timeout/2);
    }
    UARTBreakCtl(port->base, false);
}
