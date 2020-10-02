#include "nuhal/tiva.h"
#include "nuhal/uart.h"
#include "nuhal/utilities.h"
#include "nuhal/uart_tiva.h"
#include "nuhal/led.h"
#include "nuhal/pin_tiva.h"
#include "driverlib/uart.h"
#include"driverlib/interrupt.h"
#include"inc/hw_ints.h"
#include"inc/hw_memmap.h"
///@brief test the basic uart functionality
/// recieve data and send it back over the uart

#define MAX_MESSAGE_LENGTH 200

static const struct pin_configuration pin_table[] =
{
    {GPIO_PA0_U0RX,  PIN_UART},
    {GPIO_PA1_U0TX, PIN_UART}
};

int main(void)
{
    tiva_setup();
    led_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));

    const struct uart_port * port =
        uart_open("0", 1000000, UART_FLOW_NONE, UART_PARITY_NONE);

    char message[MAX_MESSAGE_LENGTH] = "";

    const enum led_color colors[] =
        {LED_COLOR_GREEN, LED_COLOR_CYAN, LED_COLOR_BLUE,
         LED_COLOR_MAGENTA, LED_COLOR_RED, LED_COLOR_YELLOW};

    led_set(LED_COLOR_RED);
    int i = 0;

    for(;;)
    {
        int len = uart_read_block(port, message, MAX_MESSAGE_LENGTH - 1,
                        0, UART_TERM_CR_OR_LF);
        // append cr/lf
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
