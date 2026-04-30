/// @brief test the printf and error reporting functionality
/// The program will print a message using uart_printf
/// and then call error
#include "nuhal/tiva.h"
#include "nuhal/uart.h"
#include "nuhal/error.h"
#include "nuhal/pin_tiva.h"

// the pins that we use
static const struct pin_configuration pin_table[] =
{
    {GPIO_PA0_U0RX, PIN_UART},
    {GPIO_PA1_U0TX, PIN_UART},
};

int main(void)
{
    tiva_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));
             
    const struct uart_port * port
        = uart_open("0", 230400, UART_FLOW_NONE, UART_PARITY_NONE);
    uart_printf(port, "\r\nHello there\r\n my number is %d\r\n", 18);
    const char str100[]
        = "abcdefghijklmnopqrstuvwxyz1234ABCDEFGHIJKLMNOPQRSTUVWXYZ5678";
    uart_printf(port,
                "This %s%s is %s%s a %s%s long %s%s string. Don't end it\r\n",
                str100, str100, str100, str100, str100, str100, str100, str100);
    error(FILE_LINE, "Ooops!");
    return 0;
}

