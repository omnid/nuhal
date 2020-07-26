#include "nuhal/led.h"
#include "nuhal/error.h"
#include "nuhal/time.h"
#include "nuhal/bytestream.h"

void led_error_basic(void)
{
    led_set(LED_COLOR_WHITE);
}

/// @brief the fatal led error code is alternating red and white
/// when this is called it never returns
void led_error_fatal(void) 
{
    for(;;)
    {
        led_set(LED_COLOR_WHITE);
        time_delay_ms(500);
        led_set(LED_COLOR_RED);
        time_delay_ms(500);
    }
}

void led_error_startup(void)
{
    for(;;)
    {
        led_set(LED_COLOR_WHITE);
        time_delay_ms(500);
        led_set(LED_COLOR_BLUE);
        time_delay_ms(500);
    }
}

void led_color_inject(struct bytestream * bs, enum led_color color)
{
    if(!bs)
    {
        error(FILE_LINE, "NULL ptr");
    }
    if(color > 0x7)
    {
        error(FILE_LINE, "invalid color");
    }
    bytestream_inject_u8(bs, color);
}

/// @param bs - the bytestream
/// @return the color that was stored in the bytestream
enum led_color led_color_extract(struct bytestream * bs)
{
    if(!bs)
    {
        error(FILE_LINE, "NULL ptr");
    }
    enum led_color color = (enum led_color)bytestream_extract_u8(bs);
    if(color > 0x7)
    {
        error(FILE_LINE, "invalid color");
    }
    return color;
}
