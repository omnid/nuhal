#include "nuhal/tiva.h"
#include "nuhal/led.h"
#include <stddef.h>
/// This tests interrupts by adding a custom fault handler
/// The fault is then triggered. If the handler is called
/// The LED should be blue

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
