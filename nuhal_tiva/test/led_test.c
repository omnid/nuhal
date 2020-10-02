/// \file
/// \brief Display Led's RED, GREEN, BLUE, WHITE, each for one second
/// Designed for use with the EK-TM4C123GXL board
#include "nuhal/tiva.h"
#include "nuhal/time.h"
#include "nuhal/led.h"

int main(void)
{
    tiva_setup();
    led_setup();
    // main program loop
    for(;;)
    {
        led_set(LED_COLOR_RED);
        time_delay_ms(1000);
        led_set(LED_COLOR_GREEN);
        time_delay_ms(1000);
        led_set(LED_COLOR_BLUE);
        time_delay_ms(1000);
        led_set(LED_COLOR_WHITE);
        time_delay_ms(1000);
    }
}
