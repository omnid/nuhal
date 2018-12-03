#ifndef LED_H_INCLUDE_GUARD
#define LED_H_INCLUDE_GUARD
/// @file
/// @brief led driver. Assumes there is an RGB led.  On the host
/// this emulates an led, using printf.  Having basic led functionality
/// in the common library makes it easier to write some code that
/// works on both host and tiva

struct bytestream;
/// @brief colors for leds. lower 3 bits of an 8 bit byte
/// correspond to the red, green, and blue leds being on (1) or off (0)
enum led_color
{
    LED_COLOR_BLACK = 0x0,
    LED_COLOR_RED   = 0x4,
    LED_COLOR_GREEN = 0x2,
    LED_COLOR_BLUE  = 0x1,
    LED_COLOR_YELLOW = 0x6,
    LED_COLOR_CYAN = 0x3,
    LED_COLOR_MAGENTA = 0x5,
    LED_COLOR_WHITE = 0x7,
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief initialize the led pins
/// This function requires a platform-specific implementation
void led_setup(void);

/// @brief set the leds to the given color
/// @param color the color combination to set the led to
/// This function requires a platform-specific implementation
void led_set(enum led_color color);

/// @brief get the current color of the led
/// This is a platform_specific function 
/// This functionr equires a platfr
enum led_color led_get(void);

/// @brief basic led error code is just a white light
void led_error_basic(void);

/// @brief the fatal led error code is alternating red and white
/// This should be called if we are already in the error handler
/// and another error occurs
void led_error_fatal(void) __attribute__((noreturn));

/// @brief an error occurred before basic facilities were initialized
/// this flashed white and blue
void led_error_startup(void) __attribute__((noreturn));


/// @brief serialize the led color
/// @param bs - the bytestream
/// @param color -t he color to place in the bytestream
void led_color_inject(struct bytestream * bs, enum led_color color);


/// @param bs - the bytestream
/// @return the color that was stored in the bytestream
enum led_color led_color_extract(struct bytestream * bs);

#ifdef __cplusplus
}
#endif
#endif
