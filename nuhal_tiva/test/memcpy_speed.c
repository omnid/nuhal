/// @brief test the speed of various memcpy operations, copying
/// different length blocks of data
#include"nuhal/tiva.h"
#include"nuhal/time.h"
#include"nuhal/uart.h"
#include"nuhal/error.h"
#include"nuhal/pin_tiva.h"
#include<inttypes.h>
#include<string.h>

static const struct pin_configuration pin_table[] =
{
    {GPIO_PA0_U0RX, PIN_UART},
    {GPIO_PA1_U0TX, PIN_UART}
};


static uint8_t src_buf[1024 * 8] = {0};
static uint8_t dest_buf[ARRAY_LEN(src_buf)] = {0};

int main(void)
{
    tiva_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));
    
    const struct uart_port * port =
        uart_open("0", 1000000, UART_FLOW_NONE, UART_PARITY_NONE);

    for(;;)
    {
        uart_printf(port, "Enter number of bytes to copy (max %u):\r\n",
                    ARRAY_LEN(src_buf));
        unsigned int to_copy = 0;
        if(1 != uart_scanf(port, "%u", &to_copy))
        {
            error(FILE_LINE, "scanf error");
        }

        if(to_copy > ARRAY_LEN(src_buf))
        {
            error(FILE_LINE, "requested number of bytes too long");
        }

        uart_printf(port, "Enter number of copies:\r\n");
        unsigned int num_copies = 0;
        if(1 != uart_scanf(port, "%u", &num_copies))
        {
            error(FILE_LINE, "scanf error");
        }
        uart_printf(port,
                    "Copying %u bytes %u times.\r\n",
                    to_copy,
                    num_copies);
        struct time_elapsed_us elapsed = time_elapsed_us_init();
        for(unsigned int i = 0; i != num_copies; ++i)
        {
            memcpy(dest_buf, src_buf, to_copy);
        }
        const uint32_t final_time = time_elapsed_us(&elapsed);
        uart_printf(port,
                    "Total time: %"PRIu32"us Time Per Copy: %"PRIu32"us\r\n",
                    final_time, final_time/num_copies);
    }
}
