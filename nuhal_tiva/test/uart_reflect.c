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
///@brief Immediately send all data received on the uart back
// over the uart.  This is for testing purposes

static const struct pin_configuration pin_table[] =
{
    {GPIO_PA0_U0RX,  PIN_UART},
    {GPIO_PA1_U0TX, PIN_UART},
    {PIN('A', 3), PIN_OUTPUT}// the estop trigger pin
};

#define BUF_SIZE 12

int main(void)
{
    tiva_setup();
    led_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));

    // disable estop
    pin_write(PIN('A', 3), 1);
    
    led_set(LED_COLOR_CYAN);
    const struct uart_port * port =
        uart_open("0", 1000000, UART_FLOW_NONE, UART_PARITY_NONE);

    char buffer[BUF_SIZE] = {0};


    for(;;)
    {
        int len = uart_read_nonblock(port, buffer, BUF_SIZE);
        uart_write_block(port, buffer, len, 0);
    }
}
