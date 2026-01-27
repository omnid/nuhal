#include "nuhal/uart.h"
#include "nuhal/time.h"
#include "nuhal/error.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int uart_read_block_error(const struct uart_port * port, void * data,
                          size_t len, uint32_t timeout,
                          enum uart_term term,
                          bool timeout_error)
{
    // if we try to wait for 0 data to be available we will wait forever
    // if the user is trying to read 0 bytes, then we have done that
    if(0 == len)
    {
        return 0;
    }
    size_t read = 0;
    struct time_elapsed_ms stamp = time_elapsed_ms_init();
    // on some platforms we can avoid taking cpu cycles to wait for data
    // in which case uart_wait_for_data is the most efficient way to go.
    // no need to check if it timed out as the next loop will be skipped
    // if it did
    (void)uart_wait_for_data(port, timeout);
    while(timeout == 0 || time_elapsed_ms(&stamp) < timeout)
    {
        read += uart_read_nonblock(port, (uint8_t*)data + read, len - read);
        if(read == len) 
        {
            return read;
        }

        switch(term)
        {
            case UART_TERM_NONE:
                ; // no early termination based on a character
                break;
            case UART_TERM_CR:
                if('\r' == ((uint8_t*)data)[read - 1])
                {
                    return read;
                }
                break;
            case UART_TERM_LF:
                if('\n' == ((uint8_t*)data)[read-1])
                {
                    return read;
                }
                break;
            case UART_TERM_NULL:
                if('\0' == ((uint8_t*)data)[read-1])
                {
                    return read;
                }
                break;
            case UART_TERM_CR_OR_LF:
                if('\r' == ((uint8_t*)data)[read-1]
                || '\n' == ((uint8_t*)data)[read-1])
                {
                    return read;
                }
                break;
            default:
            {
                char error_msg[128 + MAX_LENGTH_UART_DEVICE_PATH]; // adjust size as needed
                snprintf(error_msg, sizeof(error_msg), "Invalid termination condition on UART port %s", uart_get_device_path(port));
                error(FILE_LINE, error_msg);
                break;
            }
        }
    }
    
    // if we get here we have timed out
    if(timeout_error)
    {
        char uart_msg[read + 1];
        memcpy(uart_msg, data, read);
        uart_msg[read] = '\0';

        char error_msg[128 + MAX_LENGTH_UART_DEVICE_PATH + read]; // adjust size as needed
        snprintf(error_msg, sizeof(error_msg), "Timeout on blocking read. Trying to read on UART port %s\nHave read up to: %s\nNumber of bytes left to read: %zu", uart_get_device_path(port), uart_msg, len - read);

        error(FILE_LINE, error_msg);
    }
    return read;
}

int uart_read_block(const struct uart_port * port, void * data,
                    size_t len, uint32_t timeout, enum uart_term term)
{
    return uart_read_block_error(port, data, len, timeout, term, true);
}

int uart_write_block(const struct uart_port * port, const void * data,
                     size_t len, uint32_t timeout)
{
    size_t written = 0;
    struct time_elapsed_ms stamp  = time_elapsed_ms_init();
    
    while(0 == timeout || time_elapsed_ms(&stamp) < timeout)
    {
        written +=
            uart_write_nonblock(port, (uint8_t*)data + written, len - written);
        if(written == len)
        {
            return written;
        }
    }
    // if we get here we have timed out
    char written_uart_msg[written + 1];
    memcpy(written_uart_msg, data, written);
    written_uart_msg[written] = '\0';

    char to_write_uart_msg[len - written + 1];
    memcpy(to_write_uart_msg, (uint8_t*)data + written, len - written);
    to_write_uart_msg[len - written] = '\0';

    char error_msg[128 + MAX_LENGTH_UART_DEVICE_PATH + len]; // adjust size as needed
    snprintf(error_msg, sizeof(error_msg), "Timeout on blocking write. Trying to write from UART port %s\nHave written up to: %s\nStill left to write: %s", uart_get_device_path(port), written_uart_msg, to_write_uart_msg);

    error(FILE_LINE, error_msg);
    return -1;
}

int uart_printf(const struct uart_port * port, const char * fmt, ...)
{
    char buffer[1024] = "";
    va_list args;
    va_start(args, fmt);
    const int len = vsnprintf(NULL, 0, fmt, args);
    if(len < 0)
    {
        char error_msg[128 + MAX_LENGTH_UART_DEVICE_PATH]; // adjust size as needed
        snprintf(error_msg, sizeof(error_msg), "printf encoding error. Trying to print from UART port %s", uart_get_device_path(port));
        error(FILE_LINE, error_msg);
    }

    // true if the message to be printed fits in the buffer
    const bool fits = (size_t)len < ARRAY_LEN(buffer);

    // the real length to be written
    const int real_len =  fits ? len : (int)ARRAY_LEN(buffer);

    vsnprintf(buffer, ARRAY_LEN(buffer), fmt, args); 
    int result = uart_write_block(port, (uint8_t*)buffer, real_len, real_len/2);

    // at this point we already know that len >= 0, but len might be
    // too long for buffer[]. In this case we have
    // printed as much as we can and then a warning.
    // The coder must divide up the printf call into smaller statements.
    // TODO: eliminate this upper limit on the length of printf by
    // writing a printf driver. Having the upper limit is simpler and should
    // suffice for our purposes
    if(!fits)
    {
        static const char warning[] =
            "...\r\nWarning: uart.c: uart_printf(): printf was truncated.\r\n";
        uart_write_block(port, (uint8_t*)warning, ARRAY_LEN(warning),
                         ARRAY_LEN(warning)/2);
    }
    return result;
}

int uart_scanf(const struct uart_port * port, const char * fmt, ...)
{
    char buffer[1024] = "";
    const int len = uart_read_block(port,
                                    (uint8_t*)buffer,
                                    ARRAY_LEN(buffer),
                                    0,
                                    UART_TERM_CR_OR_LF);
    if(buffer[len - 1] != '\r' && buffer[len - 1] != '\n')
    {
        char error_msg[128 + MAX_LENGTH_UART_DEVICE_PATH]; // adjust size as needed
        snprintf(error_msg, sizeof(error_msg), "uart_scanf input too long. Trying to scan from UART port %s", uart_get_device_path(port));
        error(FILE_LINE, error_msg);
    }
    va_list args;
    va_start(args, fmt);
    return vsscanf(buffer, fmt, args);
}
