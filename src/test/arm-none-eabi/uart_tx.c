
#include "nuhal/tiva.h"
#include "nuhal/uart.h"
#include "nuhal/utilities.h"
#include "nuhal/uart_tiva.h"
#include "nuhal/led.h"
#include "nuhal/pin_tiva.h"
#include "nuhal/time.h"
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
    pin_setup(pin_table, ARRAY_LEN(pin_table));

    const struct uart_port * port =
        uart_open("0", 1000000, UART_FLOW_NONE, UART_PARITY_NONE);


    const uint8_t data = 'a';

    for(;;)
    {
        uart_write_block(port, &data, 1, 0);
        //time_delay_ms(1);
    }
}
