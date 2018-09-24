#ifndef TIVA_TLIB_PIN_INCLUDE_GUARD
#define TIVA_TLIB_PIN_INCLUDE_GUARD
/// @brief functions for setting up and controlling pins for use by
/// various peripherals and for manipulating them as GPIO
#include<stdint.h>
#include<stdbool.h>
#include "driverlib/pin_map.h"

/// @brief the type of pin to setup
enum pin_type
{
    PIN_UART,        /// uart
    PIN_OUTPUT,      /// gpio output
    PIN_INPUT,       /// gpio input
    PIN_CAN,         /// can bus
    PIN_SSI         /// ssi
};

/// @param port - the port letter 'A' - 'F'
/// @param number - the pin number
/// The packing of these bits (port and pin index) is consistent with the
/// GPIO_PXX_ constants defined in driverlib/pin_map.h
#define PIN(port, number) (uint32_t)(((port-'A') << 16) | (((number) * 4) << 8))

/// @brief the configuration of the pin
struct pin_configuration
{
    /// The pin map constant from driverlib/pin_map.h or a PIN(port, number)
    uint32_t pin;
    enum pin_type type; /// The type of the pin's function
};


/// @brief setup the pins according to the given pin configuration table
/// @param table - table of pin_configurations for the pins to setup
/// @param length - the length of table.
/// The basic idea is for every module to declare the pins it uses
/// in a constant table, and then call pin_setup() in its setup function.
/// This way, it is easy to see what pins a module uses.  pin_setup
/// will issue an error if a pin is attempted to be configured more than once.
void pin_setup(const struct pin_configuration table[], uint32_t length);

/// @brief write the value to the pin. 
/// @param pin - PIN(port, pin number),
/// the pin must be configured
/// as an input or an output
/// @param value - true for high, false for low
void pin_write(uint32_t pin, bool value);

/// @brief read the value of the pin. if pin is an output
/// then reading provides the last value set
/// @param pin - PIN(port, pin number)
/// the pin must be configured
/// as an input or an output
/// @return - the pin value
bool pin_read(uint32_t pin);

/// @brief invert the value of the pin
/// @param port_pin - PIN(port, pin number)
/// the port_pin must be configured
/// as an input or an output
/// @post - the pin's value is the logical NOT of its previous value
void pin_invert(uint32_t pin);

#endif
