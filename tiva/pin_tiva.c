#include "tiva/pin_tiva.h"
#include "common/error.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "tiva/tiva.h"

#define TIVA_NUM_PORTS 6 // number of tiva ports
#define TIVA_PINS_PER_PORT 8 // number of pins on each port


/// @brief Get the port number (with 'A' = 0) from the pin
/// @param pin - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h,
/// or PIN(port, number).
/// @return the port number
/// @pre the pin must be a valid pin
/// @post the port returned is always valid, otherwise an error occurs
static uint8_t pin_port(uint32_t pin)
{
    const uint8_t port = (pin >> 16) & 0xFF;
    if(port >= TIVA_NUM_PORTS)
    {
        error(FILE_LINE, "Invalid port");
    }
    return port;
}

/// @brief Get the pin number from the pin
/// @param pin - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h
/// or PIN(port, number).
/// @return the pin number.
/// @pre pin must be a valid pin
/// @post the pin number returned is guaranteed to be valid otherwise
/// an error occurs
static uint8_t pin_number(uint32_t pin)
{
    const uint8_t number = ((pin >> 8) & 0xFF) / 4;
    if(number >= TIVA_PINS_PER_PORT)
    {
        error(FILE_LINE, "Invalid pin");
    }
    return number;
}

/// @brief get the base address of an gpio port that holds the given pin
/// @param pin - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h
/// or PIN(port, number).
/// @return the address of the port base register for the port containing pin
static uint32_t pin_base(uint32_t pin)
{
    // port SFR bases
    static const uint32_t port_bases[TIVA_NUM_PORTS] = {
            GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE,
            GPIO_PORTD_BASE, GPIO_PORTE_BASE, GPIO_PORTF_BASE
        };

    const uint32_t port = pin_port(pin);
    return port_bases[port];
}

/// @brief get the pin mask corresponding to the pin number
/// @param pin - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h
/// or PIN(port, number).
/// @return a mask with all bits equal to zero except a 1 in the position
/// of the pin number
static uint32_t pin_mask(uint32_t pin)
{
    // pin constants
    static const uint32_t port_pins[TIVA_PINS_PER_PORT] = {
        GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
        GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7
    };

    const uint32_t number = pin_number(pin);
    return port_pins[number];
}

/// @brief setup the pin to have a desired function
/// @param pin - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h
/// or PIN(port, number).
/// @param type - the type of peripheral to use with the pin
/// @post - the pin is setup to be used by the peripheral of the given type
static void pin_configure(uint32_t pin, enum pin_type type)
{
    // system control port handles
    static const uint32_t sysctl_ports[TIVA_NUM_PORTS] = {
        SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF
    };

    // contains pinmasks of previously assigned pins
    // reassignment is not allowed and will trigger an error
    static uint32_t assigned_pins[TIVA_NUM_PORTS] = {0};

    const uint8_t port = pin_port(pin); // the port number
    const uint8_t number = pin_number(pin);
    const uint32_t base = pin_base(pin);
    const uint32_t mask = pin_mask(pin);

    if(0 == (assigned_pins[port] & mask))
    {
        // this port does not have the pin assigned
        assigned_pins[port] |= mask;
    }
    else
    {
        // the pin was already assigned
        error(FILE_LINE, "Pin already assigned.");
    }

    // enable the peripheral and perform the pin configuration
    tiva_peripheral_enable(sysctl_ports[port]);

    // PINS PC0, PC1, PC2, and PC3 are the JTAG pins.  They are locked.
    // This function will not automatically unlock them and instead trigger
    // an error. Users should explicitly unlock and change the function of these
    // pins, as changing their values can disable JTAG and require
    // using LMFlashProgrammer to execute a JTAG unlock sequence.
    if(port == 2 && number <= 3)
    {
        error(FILE_LINE, "Will not change JTAG pins");
    }

    // pins PD7 PF0 are also locked, since they are NMI signals.
    // This function will automatically unlock those
    if((port == 3 && number == 7) || (port == 5 && number == 0))
    {
        // unlock the sequence for the pin
        HWREG(base + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(base + GPIO_O_CR) |= mask;
    }

    switch(type)
    {
    case PIN_UART:
        GPIOPinConfigure(pin);
        GPIOPinTypeUART(base, mask);
        break;
    case PIN_OUTPUT:   /// gpio output
        // outputs don't get pin configured
        GPIOPinTypeGPIOOutput(base, mask);
        break;
    case PIN_CAN:
        GPIOPinConfigure(pin);
        GPIOPinTypeCAN(base, mask);
        break;
    case PIN_SSI: // ssi
        GPIOPinConfigure(pin);
        GPIOPinTypeSSI(base, mask);
        break;
    default:
        error(FILE_LINE, "Invalid peripheral type");
        break;
    }
}

void pin_setup(const struct pin_configuration table[], uint32_t length)
{
    if(!table)
    {
        error(FILE_LINE, "NULL ptr");
    }

    for(uint32_t i = 0; i != length; ++i)
    {
        pin_configure(table[i].pin, table[i].type);
    }
}

void pin_write(uint32_t pin, bool value)
{
    const uint32_t base = pin_base(pin);
    const uint32_t mask = pin_mask(pin);
    GPIOPinWrite(base, mask, value ? mask : ~mask);
}

bool pin_read(uint32_t pin)
{
    const uint32_t base = pin_base(pin_port(pin));
    const uint32_t mask = pin_mask(pin_number(pin));
    return GPIOPinRead(base, mask) & 0xFF;
}

void pin_invert(uint32_t pin)
{
    pin_write(pin, !pin_read(pin));
}
