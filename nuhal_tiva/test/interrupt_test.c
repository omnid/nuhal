#include "nuhal/tiva.h"
#include "nuhal/led.h"
#include <stddef.h>

// custom fault handler
void FaultISR(void)
{
    led_set(LED_COLOR_BLUE);
    for(;;)
    {
        ;
    }
}

int main(void)
{
    tiva_setup();
    led_setup();
    // intentionally cause a divide by zero nmi
    int divisor = 0;
    int quotient = 123/divisor;
    divisor = quotient;
    for(;;)
    {
        ;
    }
}
