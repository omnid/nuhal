#define _DEFAULT_SOURCE // enable posix so we can use clock_gettime
/// @brief implementation of common/uart.h interface on linux systems
#include"common/uart.h"
#include"common/error.h"

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>   // for STDOUT_FILENO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
// timeout to wait for pending writes to finish before closing the port
static const int CLOSE_TIMEOUT = 200;

// store old termios data with the port that is opened.
// All open ports are stored in a linked list to track them
struct uart_port
{
    int fd;
    bool is_open;
    struct termios old_tio;
    struct uart_port * next;
    struct uart_port * prev;
    // pointer to the struct itself. provides an internal non-constant reference
    // to this structure that can be used internally to modify it
    struct uart_port * self;
};

static struct uart_port * port_list_head = NULL;

// run at exit to close all the uart ports
static void uart_cleanup(void)
{
    struct uart_port * curr_port = port_list_head;
    while(NULL != curr_port)
    {
        struct uart_port * to_close = curr_port;
        // update the current port since uart_close frees it
        curr_port = curr_port->next;

        // close the port and free it
        uart_close(to_close);
    }
}

// open a serial port (using POSIX calls)
const struct uart_port * uart_open(const char name[], uint32_t baud,
                                   enum uart_flow flow, enum uart_parity parity)
{
    static bool first_run = true;
    // set an at_exit function to automatically close all uart ports on exit
    if(first_run)
    {
        first_run = false;
        if(0 != atexit(uart_cleanup))
        {
            error(FILE_LINE, "Failed to register uart cleanup function");
        }
    }

    // allocate a new port struct
    struct uart_port * port =  (struct uart_port *)malloc(sizeof(struct uart_port));
    if(NULL == port)
    {
        // could not allocate memory for the port structure
        error_with_errno(FILE_LINE);
    }
    memset(port, 0, sizeof(*port));
    port->next = NULL;
    port->prev = NULL;
    port->self = port;

    // add the port to the port list
    if(NULL == port_list_head)
    {
        port_list_head = port;
    }
    else
    {
        // add the port to the end of the list
        struct uart_port * curr_port = port_list_head;
        while(NULL != curr_port->next)
        {
            curr_port = curr_port->next;
        }
        curr_port->next = port;
        port->prev = curr_port;
    }


    // open serial port for non-blocking reads
    port->fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (-1 == port->fd)
    {
        // failed to open the port
        error_with_errno(FILE_LINE);
    }


    struct termios tio;
    // retrieve the original settings and save them
    if(0 != tcgetattr(port->fd, &port->old_tio) )
    {
        error_with_errno(FILE_LINE);
    }

    tio.c_cflag |= CS8 | CREAD | CLOCAL; // 8n1, see termios.h 

    // set raw input mode
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // set raw output mode
    tio.c_oflag &= ~OPOST;

    tio.c_cflag |= B38400;
    speed_t stdbaud = 0;
    switch(baud)
    {
    case 9600:
        stdbaud = B9600;
        break;
    case 115200:
        stdbaud = B115200;
        break;
    case 230400:
        stdbaud = B230400;
        break;
    case 1000000:
        stdbaud = B1000000;
        break;
    case 2000000:
        stdbaud = B2000000;
        break;
    case 3000000:
        stdbaud = B3000000;
        break;
    default:
        error(FILE_LINE, "Unsupported baud rate selected.");
        break;
    }

    switch(flow)
    {
    case UART_FLOW_HARDWARE:
        tio.c_cflag |= CRTSCTS;
        tio.c_iflag &= ~(IXON | IXOFF);
        break;
    case UART_FLOW_SOFTWARE:
        tio.c_cflag &= ~CRTSCTS;
        tio.c_iflag |= IXON | IXOFF;
        break;
    case UART_FLOW_NONE:
        tio.c_cflag &= ~CRTSCTS;
        tio.c_iflag &= ~(IXON | IXOFF);
        break;
    default:
        error(FILE_LINE, "Unsupported flow control mode selected.");
        break;
    }

    switch(parity)
    {
    case UART_PARITY_EVEN:
        tio.c_cflag |= PARENB;
        tio.c_cflag &= ~PARODD;
        break;
    case UART_PARITY_ODD:
        tio.c_cflag |= PARENB | PARODD;
        break;
    case UART_PARITY_NONE:
        tio.c_cflag &= ~PARENB;
        break;
    default:
        error(FILE_LINE, "Unsupported parity mode selected.");
        break;
    }

    if(cfsetospeed(&tio, stdbaud) != 0)
    {
        error_with_errno(FILE_LINE);
    }

    if(cfsetispeed(&tio, stdbaud) != 0)
    {
        error_with_errno(FILE_LINE);
    }


    // all modes use 8 data bits
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;


    // set serial port options
    if(tcsetattr(port->fd, TCSANOW, &tio) != 0)
    {
        error_with_errno(FILE_LINE);
    }

    // when using a USB serial driver, the driver does not fill
    // the uart buffer with data it has already received right away.
    // therefore, we need to delay after opening the port in order
    // to ensure that the buffers are flushed properly
    usleep(5000);

    // flush serial buffers
    if(tcflush(port->fd, TCIOFLUSH) != 0)
    {
        error_with_errno(FILE_LINE);
    }

    return port;
}



int uart_read_nonblock(const struct uart_port * port, void * data, size_t length)
{
    int val = read(port->fd, data, length);
    if(val < 0)
    {
        // this just indicates that there is no data ready
        // so read would block.  it is not a true error it
        // just means we read zero bytes of data
        if(EAGAIN == errno || EWOULDBLOCK == errno)
        {
            return 0;
        }
        error_with_errno(FILE_LINE);
    }
    return val;
}

int uart_write_nonblock(const struct uart_port * port,
                        const void * data, size_t length)
{
    int val = write(port->fd, data, length);
    if(val < 0)
    {
        error_with_errno(FILE_LINE);
    }
    return val;
}

void uart_close(const struct uart_port * port)
{
    // wait for all pending writes to finish, or there is a timeout
    struct pollfd events;
    memset(&events, 0, sizeof(events));
    events.fd = port->fd;
    events.revents |= POLLOUT;

    // wait for writes to finish or for a timeout.
    int pval = poll(&events, 1, CLOSE_TIMEOUT);
    if(pval < 0)
    {
        error_with_errno(FILE_LINE);
    }
    else if(0 == pval)
    {
        // this was a timeout. Flow control can prevent us from flushing
        // so disable it
        struct termios tio;
        // get the current settings
        if(0 != tcgetattr(port->fd, &tio) )
        {
            error_with_errno(FILE_LINE);
        }
        // disable flow control
        tio.c_cflag &= ~CRTSCTS;
        if(tcsetattr(port->fd, TCSANOW, &tio) != 0)
        {
            error_with_errno(FILE_LINE);
        }

        // flush the buffers
        if(0 != tcflush(port->fd, TCIOFLUSH))
        {
            error_with_errno(FILE_LINE);
        }
    }

    // restore settings to state when the program was open
    if(0 != tcsetattr(port->fd, TCSADRAIN, &port->old_tio))
    {
        error_with_errno(FILE_LINE);
    }

    if(0 != close(port->fd))
    {
        error_with_errno(FILE_LINE);
    }

    // remove the port from the list
    if(NULL != port->prev)
    {
        port->prev->next = port->next;
    }
    if(NULL != port->next)
    {
        port->next->prev = port->prev;
    }
    if(port == port_list_head)
    {
        port_list_head = port->next;
    }
    free(port->self);
}

void uart_send_break(const struct uart_port * port, uint32_t timeout)
{
    struct termios tio;
    // get the current settings
    if(0 != tcgetattr(port->fd, &tio) )
    {
        error_with_errno(FILE_LINE);
    }

    const tcflag_t cflag = tio.c_cflag;
    if(cflag & PARENB)
    {
        error(FILE_LINE, "Cannot send break when using parity");
    }

    // set even parity
    tio.c_cflag |= PARENB;
    tio.c_cflag &= ~PARODD;

    // set the attributes so we have even parity
    if(tcsetattr(port->fd, TCSANOW, &tio) != 0)
    {
        error_with_errno(FILE_LINE);
    }
    
    // send a zero byte
    static const uint8_t zero = 0x00;
    uart_write_block(port, &zero, 1, timeout);

    // restore the old settings
    tio.c_cflag = cflag;
    if(tcsetattr(port->fd, TCSANOW, &tio) != 0)
    {
        error_with_errno(FILE_LINE);
    }
}

bool uart_wait_for_data(const struct uart_port * port, uint32_t timeout)
{
    struct pollfd fds[] = {{.fd = port->fd, .events = POLLIN}};
    if(timeout > INT_MAX)
    {
        error(FILE_LINE,"invalid param");
    }
    int res = poll(fds, ARRAY_LEN(fds), timeout == 0 ? -1 : (int)timeout);

    if(res == 0)
    {
        return false;
    }
    else if(res == 1)
    {
        return true;
    }
    else
    {
        error_with_errno(FILE_LINE);
    }
}

bool uart_data_available(const struct uart_port * port)
{
    struct pollfd fds[] = {{.fd = port->fd, .events = POLLIN}};
    int res = poll(fds, ARRAY_LEN(fds), 0);
    if(res < 0)
    {
        error_with_errno(FILE_LINE);
    }
    else
    {
        return fds->revents & POLLIN;
    }
}
