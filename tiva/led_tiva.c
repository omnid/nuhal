#include "common/led.h"
#include "common/utilities.h"
#include "tiva/pin_tiva.h"

/// pin definitions for the led
const struct pin_configuration led_pin_table[] =
{
    {PIN_LED_RED  , PIN_OUTPUT},
    {PIN_LED_GREEN, PIN_OUTPUT},
    {PIN_LED_BLUE , PIN_OUTPUT},
};

void led_setup(void)
{
    pin_setup(led_pin_table, ARRAY_LEN(led_pin_table));
}

void led_set(enum led_color color)
{
    pin_write(PIN_LED_RED, color & LED_COLOR_RED);
    pin_write(PIN_LED_BLUE, color & LED_COLOR_BLUE);
    pin_write(PIN_LED_GREEN, color & LED_COLOR_GREEN);
}

enum led_color led_get(void)
{
    const bool red = pin_read(PIN_LED_RED);
    const bool green = pin_read(PIN_LED_GREEN);
    const bool blue = pin_read(PIN_LED_BLUE);
    uint8_t result = 0;
    result |= red ? LED_COLOR_RED : 0;
    result |= green ? LED_COLOR_GREEN: 0;
    result |= blue ? LED_COLOR_BLUE : 0;
    return (enum led_color)result;
}


