#include "nuhal/tiva.h"
#include "nuhal/time.h"
#include "nuhal/utilities.h"
#include "nuhal/led.h"
#include "nuhal/pin_tiva.h"

#define USER_BUTTON_1 PIN('F',4)
static const struct pin_configuration pins[] =
{
    {USER_BUTTON_1, PIN_INPUT_PULL_UP}
};


int main(void)
{
    tiva_setup();
    pin_setup(pins, ARRAY_LEN(pins));
    led_setup();
    // main program loop
    for(;;)
    {
        if(pin_read(USER_BUTTON_1))
        {
            led_set(LED_COLOR_GREEN);
        }
        else
        {
            led_set(LED_COLOR_BLUE);
        }
    }
}
