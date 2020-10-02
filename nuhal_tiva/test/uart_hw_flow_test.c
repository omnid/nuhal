#include "nuhal/tiva.h"
#include "nuhal/uart.h"
#include "nuhal/utilities.h"
#include "nuhal/led.h"
#include "driverlib/pin_map.h"
#include "nuhal/pin_tiva.h"

///@brief test the basic uart functionality
/// recieve data and send it back over UART1,
/// The only tiva uart to have flow control

#define MAX_MESSAGE_LENGTH 200

static const struct pin_configuration pin_table[] = {
    {GPIO_PB0_U1RX,  PIN_UART},
    {GPIO_PB1_U1TX,  PIN_UART},
    {GPIO_PC5_U1CTS, PIN_UART},
    {GPIO_PC4_U1RTS, PIN_UART}
};

int main(void)
{
    tiva_setup();
    led_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));
    
    // note: parity works but cannot be set with screen, so if we use parity,
    // need to connect with another terminal emulator such as putty
    const struct uart_port * port
        = uart_open("1", 1000000, UART_FLOW_HARDWARE, UART_PARITY_NONE);

    char message[MAX_MESSAGE_LENGTH] = "";

    static const enum led_color colors[] =
        {LED_COLOR_RED, LED_COLOR_MAGENTA, LED_COLOR_BLUE,
         LED_COLOR_CYAN, LED_COLOR_GREEN, LED_COLOR_YELLOW};
    int i = 0;

    for(;;)
    {
        int len = uart_read_block(port, message, MAX_MESSAGE_LENGTH - 1,
                                  0, UART_TERM_CR_OR_LF);
        // echo the message back after appending cr/lf
        message[len - 1] = '\r';
        message[len] = '\n';
        uart_write_block(port, message, len + 1, 0);

        // toggle leds in sequence
        led_set(colors[i]);
        ++i;
        if(i == ARRAY_LEN(colors))
        {
            i = 0;
        }
    }
}
