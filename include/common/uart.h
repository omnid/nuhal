#ifndef DEVICES_UART_H_INCLUDE_GUARD
#define DEVICES_UART_H_INCLUDE_GUARD
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/// @file
/// @brief generic interface to a uart,
/// implemented with different uart_<platform>.c files

/// @brief a description of the port
struct uart_port;

/// @brief types of flow control
enum uart_flow
{
    UART_FLOW_HARDWARE, /// use hardware flow control
    UART_FLOW_SOFTWARE, /// use software Xon/Xoff flow control
      ///NOTE: on most platforms software flow control is unimplimented
    UART_FLOW_NONE /// do not use flow control
};

/// @brief types of parity
enum uart_parity
{
    UART_PARITY_NONE, /// do note use parity checking
    UART_PARITY_ODD,  /// odd parity
    UART_PARITY_EVEN  /// even parity
};

/// @brief types of termination sequences to look for when doing blocking reads
enum uart_term
{
    UART_TERM_NONE,  /// no termination character
    UART_TERM_LF,   /// line-feed '\n' character ends the read
    UART_TERM_CR,   /// carriage return '\r' character ends the read
    UART_TERM_CR_OR_LF, // either a '\r' or a '\n' ends the read
    UART_TERM_NULL, /// Null character terminates the read
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief open a uart with given baud, flow control, parity, and 1 stop bit.
///  The port should be opened in non-blocking mode
/// @param name - the name of the port to open
/// @param baud - the baud rate of the port
/// @param flow - the type of flow control to use
/// @param parity - the type of parity to use
/// @return a handle to the port
/// @post all errors result in program termination
/// @post on the host, a cleanup function will be registered 
/// that closes the port and flushes its buffers upon program exit. 
const struct uart_port *
uart_open(const char name[], unsigned int baud,
          enum uart_flow flow, enum uart_parity parity); 

/// @brief non-blocking read of the uart.  Reads up to len bytes from the uart,
///       storing them in data
/// @param port - the port to read from. must be opened with uart_open
/// @param data - buffer in which to store the data read from the uart
/// @param len - the maximum amount of data to read, in bytes
/// @return the number of bytes read. 
/// @post all errors result in program termination
int uart_read_nonblock(const struct uart_port * port, void  * data, size_t len);

/// @brief blocking read of the uart. Reads len bytes into data or times out
/// @param port - the port from which to read. must be opened with uart_open
/// @param data - buffer in which to store the data read from the uart
/// @param len - the length of data to read, in bytes
/// @param timeout - time in ms to wait for new characters.
///   if more than this time elapses, a fatal error occurs. if 0,
///   the timeout is disabled 
/// @param term - termination character sequence to look for.
///    if this sequence is read the read stops
/// even if len characters have not been read
/// @return the number of characters read
/// @post errors (including timeouts) result in program termination
int uart_read_block(const struct uart_port * port, void * data, size_t len,
                    uint32_t timeout, enum uart_term term);


/// @brief non-blocking write data to uart. Up to len bytes will be written
/// @param port - the port to write to. must be opened with uart_open
/// @param data - buffer storing the data to be written
/// @param len - the maximum amount of data to write, in bytes
/// @return the number of bytes written
/// @post all errors (including timeouts) result in program termination
int uart_write_nonblock(const struct uart_port * port,
                        const void * data, size_t len);

/// @brief blocking write to the uart. len bytes will be written
/// @param port - the port to write to. must be opened with uart_open
/// @param data - buffer storing the data to be written
/// @param len - the amount of data to write, in bytes
/// @param timeout - time in ms to wait for transmission.
///    if it takes longer a fatal error occurs. 0 disables timeout
/// @return the number of characters written
/// @post all errors (including timeouts) result in program termination
int uart_write_block(const struct uart_port * port, const void * data,
                     size_t len, uint32_t timeout);

/// @brief close the given serial port. The handle will no longer be valid
/// @param port - the port to close
/// @post all errors result in program termination.
void uart_close(const struct uart_port * port);

/// @brief blocking write to the uart using a printf format string
/// @param port - the port to use
/// @param fmt - printf style format string
/// @param ... - arguments as they would be provided to printf
/// @return the number of characters written
/// @post this will block until the data can be written or a timeout 
/// equal to 0.5 ms per character occurs. Such a timeout is a fatal error  
int uart_printf(const struct uart_port * port, const char * fmt, ...)
    __attribute__ ((format (printf, 2, 3)));

/// @brief a blocking write analagous to scanf.  There is an infinite timeout
/// and a maximum length of text in a line that can be input (1024) without
/// causing an error
/// @param port - the port to read
/// @param fmt - scanf style format string
/// @param ... - arguments as the would be provided to printf
/// @return - the number of arguments that were filled by the scanf
int uart_scanf(const struct uart_port * port, const char * fmt, ...)
    __attribute__((format (scanf, 2, 3)));

/// @brief send a break signal.  This consists of all 0 data bits plus
///  a stop bit of zero.  Some USB-serial converters cannot do this,
/// therefore, on the host system a break is emulated by
/// switching the port to even parity and sending all zeros.
/// If your protocol is using even parity then the break generated
/// by this function is indistinguishable from a parity error
/// @param port - the port over which to send the break
/// @param timeout - 0 no timeout, otherwise may timeout when sending the break
/// @post error results in program termination
void uart_send_break(const struct uart_port * port, uint32_t timeout);

/// @brief wait for data to be available on the uart.
/// @param port - the uart port on which to wait for data
/// @param timeout - timeout in ms.
/// @return true if data becomes available within the timeout period, else false
bool uart_wait_for_data(const struct uart_port * port, uint32_t timeout);

/// @brief determine if their is data availabe on the uart
/// @param port - the uart port to check
/// @return true if data is available to be read
bool uart_data_available(const struct uart_port * port);

#ifdef __cplusplus
}
#endif
#endif
