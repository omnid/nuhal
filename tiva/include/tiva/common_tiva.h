#ifndef COMMON_TIVA_H_INCLUDE_GUARD
#define COMMON_TIVA_H_INCLUDE_GUARD
/// @brief tiva-specific utility functions that are useful to common_impl

#include<stdint.h>
#include<stdbool.h>
#include "driverlib/pin_map.h"

/// @brief common setup for tiva
void common_setup(void);

uint32_t tiva_clock_frequency(void);

void tiva_peripheral_enable(uint32_t ui32Peripheral);

uint32_t tiva_frequency_to_counts(uint32_t frequency);

/// @brief the type of pin to setup
enum peripheral
{
    TIVA_UART,        /// uart
    TIVA_UART_UNLOCK, /// uart with unlocking for pin function
    TIVA_OUTPUT,      /// gpio output
    TIVA_CAN,         /// can bus
    TIVA_SSI,         /// ssi
    TIVA_SETUP_TABLE, /// pointer to a pin setup table with more definitions
    TIVA_END          /// don't setup the pin
};

/// @brief set up a pin to have a specific function
struct tiva_pin_setup
{
    union
    {
        uint32_t map;         /// The pin map constant from driverlib/pin_map.h
        const struct tiva_pin_setup *  table; // a tiva_pin_setup table
    } setup;
    enum peripheral type; /// The type of the pin's function
};

/// @brief setup for tiva pins.
/// This array is defined by each program.
/// common_setup reads this array and configures the appropriate peripheral
/// and pins, including enabling the port that owns the pin and setting it
/// to use the appropriate peripheral. Depending on the device,
/// additional configuration might be required. (in other words, this
/// only handles the GPIO part of setting up the peripheral).
///
/// each entry is either a pin setup or a pointer to another
/// pin_setup table.
/// Pins are set using the macros in driverlib/pin_map.h,
/// If the pin is an ordinary GPIO than the macro
/// TIVA_GPIO() should be used since pin_map.h does not have
/// macros to set up the port as a regular pin.
/// Every module can provide its own <module>_pin_table
/// Which can then be included in this table as well
/// (up to a maximum nesting depth).
/// Every table ends with TIVA_PIN_SETUP_END
/// BY default no pins are included
extern const struct tiva_pin_setup tiva_pin_table[];

/// @brief mark the end of  a tiva_pin_setup table
#define TIVA_PIN_SETUP_END {{0xFFFFFFFF}, TIVA_END}

/// @brief a GPIO_Pxx_GPIO constant. Set the port and pin number if you want 

/// @param port - the port letter
/// The packing of these bits (port and pin index) is consistent with the
/// GPIO_PXX_ constants defined in driverlib/pin_map.h
#define TIVA_GPIO(port,pin) (uint32_t)(((port-'A') << 16) | (((pin) * 4) << 8))

/// @brief write the value to the pin
/// @param port_pin - TIVA_GPIO(port, pin number)
/// @param value - true for high, false for low
void tiva_gpio_write(uint32_t port_pin, bool value);

/// @brief read the value of the pin. if pin is an output
/// then reading provides the last value set
/// @param port_pin - TIVA_GPIO(port, pin number)
/// @return - the pin value
bool tiva_gpio_read(uint32_t port_pin);

/// @brief invert the value of the pin
/// @param pin - TIVA_GPIO(port, pin number)
/// @post - the pin's value is the logical NOT of its previous value
void tiva_gpio_invert(uint32_t port_pin);

#endif
