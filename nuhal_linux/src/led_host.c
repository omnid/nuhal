#include "nuhal/led.h"
#include "nuhal/error.h"
#include <stdio.h>

static enum led_color color = LED_COLOR_BLACK;

void led_setup(void)
{
}

void led_set(enum led_color new_color)
{
    switch(new_color)
    {
    case LED_COLOR_BLACK:
        printf("LED: BLACK\n");
        break;
    case LED_COLOR_RED:
        printf("LED: RED\n");
        break;
    case LED_COLOR_GREEN:
        printf("LED: GREEN\n");
        break;
    case LED_COLOR_BLUE :
        printf("LED: BLUE \n");
        break;
    case LED_COLOR_YELLOW:
        printf("LED: YELLOW\n");
        break;
    case LED_COLOR_CYAN:
        printf("LED: CYAN\n");
        break;
    case LED_COLOR_MAGENTA:
        printf("LED: MAGENTA\n");
        break;
    case LED_COLOR_WHITE:
        printf("LED: WHITE\n");
        break;
    default:
        error(FILE_LINE, "Unknown color");
    }
    color = new_color;
}

enum led_color led_get(void)
{
    return color;
}


