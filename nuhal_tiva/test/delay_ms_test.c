#include "nuhal/tiva.h"
#include "nuhal/error.h"
#include "nuhal/time.h"
#include "nuhal/pin_tiva.h"
#include "inc/hw_memmap.h"

// test delay_ms function and time_elapsed_ms
// PB2 will output a 4 Hz square wave if this works
// Thus the pin will be high for 125ms and low for 125ms
// The period is approximately 250ms.
// due to computations the timing will not be exact
// Map PB2 As a GPIO. The

#define IO_PIN PIN('B', 2)
static const struct pin_configuration pin_table[] =
{
    {IO_PIN, PIN_OUTPUT},
};

int main(void)
{
    tiva_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));
    
    // main program loop
    for(;;)
    {
        struct time_elapsed_ms stamp = time_elapsed_ms_init();

        pin_invert(IO_PIN);
        // the high and low will each be approx 125 ms for a frequency of 250ms
        time_delay_ms(125);
        const uint32_t elapsed = time_elapsed_ms(&stamp);
        if(125 != elapsed)
        {
            error(FILE_LINE, "time elapsed failed");
        }
    }
}
