#include "common/led.h"
#include <stdio.h>

static enum led_color color = LED_COLOR_BLACK;

void led_setup(void)
{
}

void led_set(enum led_color new_color)
{
    printf("LED: 0x%x\n", new_color);
    color = new_color;
}

enum led_color led_get(void)
{
    return color;
}


