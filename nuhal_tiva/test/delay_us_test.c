#include "nuhal/tiva.h"
#include "nuhal/time.h"
#include "nuhal/error.h"
#include "nuhal/utilities.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "nuhal/pin_tiva.h"

// test delay_ms function and time_elapsed_ms
// PB2 will output a 4000 Hz square wave if this works
// Thus the pin will be high for 125us and low for 125us
// The period is approximately 250us
// Map PB2 As a GPIO. 
static const struct pin_configuration pin_table[] =
{
    {PIN('B', 2), PIN_OUTPUT}
};

int main(void)
{
    tiva_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));
    
    int val = 0;
    // main program loop
    for(;;)
    {
        struct time_elapsed_us stamp = time_elapsed_us_init();
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, val);
        val = ~val;
        // the high and low will each be approx 125 ms for a frequency of 250ms
        time_delay_us(125); 
        const uint32_t elapsed = time_elapsed_us(&stamp);
        // error of +/- 1% is acceptable
        if(elapsed < 113 || elapsed > 137)
        {
            error(FILE_LINE, "time elapsed failed");
        }
    }
}
