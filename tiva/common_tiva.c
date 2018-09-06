#include"common/error.h"
#include"common/uart.h"
#include"tiva/common_tiva.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/fpu.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include<stdint.h>

/// @brief tiva specific code for the common library
#define TIVA_NUM_PORTS 6 // number of tiva ports
#define TIVA_PINS_PER_PORT 8 // number of pins on each port

#define TIVA_PIN_TABLE_DEPTH 5 // maximum recursive depth of tiva pin tables

/// @brief extract the index of the port from the pin_map constant
/// @param pinmap - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h
///  this constant is used to set pin Pxx to peripheral function yyyy
/// @return the port number
static uint8_t tiva_port_index(uint32_t pinmap)
{
    const uint8_t port = (pinmap >> 16) & 0xFF;
    if(port >= TIVA_NUM_PORTS)
    {
        error(FILE_LINE, "Invalid port");
    }
    return port;
}

/// @brief extract the index of the port from the pin_map constant
/// @param pinmap - constant of the form GPIO_Pxx_yyyy from driverlib/pin_map.h
///  this constant is used to set pin Pxx to peripheral function yyyy
/// @return the pin number
static uint8_t tiva_pin_index(uint32_t pinmap)
{
    const uint8_t pin = ((pinmap >> 8) & 0xFF) / 4;
    if(pin >= TIVA_PINS_PER_PORT)
    {
        error(FILE_LINE, "Invalid pin");
    }
    return pin;
}

/// @brief get the base address of an gpio port
static uint32_t tiva_port_base(uint8_t port_index)
{
    // port SFR bases
    static const uint32_t port_bases[] = {
            GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE,
            GPIO_PORTD_BASE, GPIO_PORTE_BASE, GPIO_PORTF_BASE
        };
    // should be one entry in port bases for each port
    STATIC_ASSERT(ARRAY_LEN(port_bases) == TIVA_NUM_PORTS, unknown_base);

    if(port_index > ARRAY_LEN(port_bases))
    {
        error(FILE_LINE, "out of range");
    }
    return port_bases[port_index];
}

/// @brief get the pin constant corresponding to the pin number
static uint32_t tiva_pin_mask(uint8_t pin_index)
{
    // pin constants
    static const uint32_t port_pins[] = {
        GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
        GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7
    };
    if(pin_index > ARRAY_LEN(port_pins))
    {
        error(FILE_LINE, "out of range");
    }
    return port_pins[pin_index];
}

// determine if the port and poin are unlocked
static bool tiva_unlocked(uint8_t port, uint8_t pin)
{
    const uint32_t port_base = tiva_port_base(port);
    return HWREG(port_base + GPIO_O_CR) & (1 << pin); // pin is committed
}


/// @brief unlock a special port/pin, allowing its function to be changed.
/// must be called prior to common_setup if a locked pin is to be configured
/// @param portpin - TIVA_GPIO('PORTLETTER', PINNUMBER) of the pin to unlock
static void tiva_unlock(uint8_t port, uint8_t pin)
{
    const uint32_t port_base = tiva_port_base(port);
    HWREG(port_base + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(port_base + GPIO_O_CR) |= 1 << pin;
}

static void tiva_lock(uint8_t port, uint8_t pin)
{
    const uint32_t port_base = tiva_port_base(port);
    HWREG(port_base + GPIO_O_CR) &= ~(1 << pin);
    HWREG(port_base + GPIO_O_LOCK) = 0;
}

/// @brief setup the pin to have a desired function
/// @param pinmap - pin function mapping constant GPIO_Pxx_yyyy from pin_map.h
/// @param the type of the port to setup
/// @param unlock -if true unlock the pin
static void tiva_pin_setup(uint32_t pinmap, enum peripheral type)
{

    // system control port handles
    static const uint32_t sysctl_ports[] = {
        SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF
    };


    // contains pinmasks of previously assigned pins
    // reassignment is not allowed and will trigger an error
    static uint32_t assigned_pins[TIVA_NUM_PORTS] = {0};

    const uint8_t port = tiva_port_index(pinmap); // the port number
    const uint8_t pin = tiva_pin_index(pinmap);   // the pin number
    const uint32_t port_base = tiva_port_base(port);
    const uint32_t pin_mask = tiva_pin_mask(pin);

    if(0 == (assigned_pins[port] & pin_mask))
    {
        // this port does not have the pin assigned
        assigned_pins[port] |= pin_mask;
    }
    else
    {
        error(FILE_LINE, "pin already assigned");
    }

    // enable the peripheral and perform the pin configuration
    tiva_peripheral_enable(sysctl_ports[port]);

    /// check if the pin function should be unlocked
    bool relock = false;
    if(type == TIVA_UART_UNLOCK)
    {
        tiva_unlock(port, pin);
        relock = true;
        type = TIVA_UART;
    }

    // if pin function is not unlocked and it needs to be this is an error
    if(!tiva_unlocked(port, pin))
    {
        error(FILE_LINE, "special pin must be unlocked");
    }

    switch(type)
    {
    case TIVA_UART:
        GPIOPinConfigure(pinmap);
        GPIOPinTypeUART(port_base, pin_mask);
        break;
    case TIVA_OUTPUT:   /// gpio output
        // outputs don't get pin configured
        GPIOPinTypeGPIOOutput(port_base, pin_mask);
        break;
    case TIVA_CAN:
        GPIOPinConfigure(pinmap);
        GPIOPinTypeCAN(port_base, pin_mask);
        break;
    case TIVA_SSI: // ssi
        GPIOPinConfigure(pinmap);
        GPIOPinTypeSSI(port_base, pin_mask);
        break;
    default:
        error(FILE_LINE, "Invalid peripheral type");
        break;
    }

    if(relock)
    {
        tiva_lock(port, pin);
    }
}

/// @brief the maximum value for the systick counter.
/// It is 24 bits on the TM4C123G
static const uint32_t SYSTICK_MAX = 0x00FFFFFF;

/// @brief store the clock frequency
static uint32_t clock_freq = 0;

/// @brief basic setup routines for the EK-TM4C123GXL launchpad board.
/// Initializing the the USB UART, the clock, and the FPU
/// Also sets up some fault handling behavior. Enables interrupts globally
/// (individual interrupts must still be configured)
/// Sets up printf and scanf to use the USB uart (UART0)
/// It also sets up the pins to use the peripherals, according to
/// the array TIVA_PIN_MAP.  By default the LED pins and UART0 pins are setup,
/// but changing those pins in TIVA_PIN_MAP overrides this (not tested)
void common_setup(void)
{
    if(CLASS_IS_TM4C123)
    {
        // set the clock to 80 MHz. We use the main oscillator
        // which is a 16 MHz crystal. The PLL runs at 400 MHz
        // but is divided by 2
        SysCtlClockSet(
            SYSCTL_USE_PLL      // use the PLL (which runs at 400 Mhz)
            | SYSCTL_XTAL_16MHZ // 16 MHz crystal
            | SYSCTL_OSC_MAIN   // the main oscillator
            | SYSCTL_SYSDIV_2_5   // (400 MHz / 2) / 2.5  = 80 MHz
            );
        clock_freq = SysCtlClockGet();
    }
    else
    {
        // When using a TM4C123, setting the clock
        // frequency is different.  therefore,
        // we would need to look into things more to
        // get htings write.  we Would do something like
        // clock_freq = SysCtlClockFreqSet()
    }

    if(0 == clock_freq)
    {
        // This would also currently trigger if CLASS_IS_TM4C123 is false
        error(FILE_LINE, "Could not set clock frequency\n");
    }

    // enable the SysTick timer of the processor
    SysTickPeriodSet(SYSTICK_MAX);
    SysTickEnable();

    // a pin table and how many pins have been explored
    // each time we encounter a table within a table we add it to the stack
    const struct tiva_pin_setup * pin_table_stack[TIVA_PIN_TABLE_DEPTH] = {0};
    int indices[TIVA_PIN_TABLE_DEPTH] = {0};
    int top = 0; // current pin we are looking at
    pin_table_stack[0] = tiva_pin_table;
    const struct tiva_pin_setup * curr = &pin_table_stack[0][0];
    // loop over pin configurations until the sentinal is found
    while(!(top == 0 && TIVA_END == curr->type))
    {
        // we have iterated too many items and are likely missing the
        // TIVA_PIN_SETUP_END marker
        if(indices[top] > 30)
        {
            error(FILE_LINE,
                  "Too many pin setups. Missing TIVA_PIN_SETUP_END");
        }

        if(curr->type == TIVA_SETUP_TABLE)
        {
            // push the next layer to the top of stack
            ++top;
            if(top >= TIVA_PIN_TABLE_DEPTH)
            {
                error(FILE_LINE, "pin_tables are nested too deeply");
            }
            indices[top] = 0;
            pin_table_stack[top] = curr->setup.table;
        }
        else if(curr->type == TIVA_END)
        {
            // pop the current table off the stack
            // and advance the lower level's index
            --top;
            ++indices[top];
        }
        else
        {
            // setup the pin
            tiva_pin_setup(curr->setup.map, curr->type);
            ++indices[top];
        }
        curr = &pin_table_stack[top][indices[top]];
    }

    // make sure floating point is enabled
    FPUEnable();

    // only save floating point state to stack if it is necessary
    FPULazyStackingEnable();

    // trap on divide by zero.
    // do not trap on unaligned access ( add | NVIC_CFG_CTRL_UNALIGNED to trap)
    // processor can handle unaligned accesses, though there might be
    // performance hits. leave it to the compiler to optimize appropriately
    // (setting it to fault on unaligned access causes memcpy to crash sometimes.
    // unless the -mno-unaligned-access flag is passed to the compiler
    HWREG(NVIC_CFG_CTRL) |= NVIC_CFG_CTRL_DIV0; // | NVIC_CFG_CTRL_UNALIGNED;

    // Enable interrupts
    IntMasterEnable();
}


/// @brief enable the given peripheral and wait for it to be ready
/// @param ui32Peripheral - The peripheral to use.
///          see Tivaware SysCtlPeripheralEnable for valid values
void tiva_peripheral_enable(uint32_t ui32Peripheral)
{
    if(!SysCtlPeripheralReady(ui32Peripheral))
    {
        SysCtlPeripheralEnable(ui32Peripheral);
        int count = 100;
        while(!SysCtlPeripheralReady(ui32Peripheral) && count > 0)
        {
            --count;
        }
        if(0 == count)
        {
            error(FILE_LINE, "failed to enable peripheral");
        }
    }
}

/// @brief get the clock frequency for the processor, in Hz
uint32_t tiva_clock_frequency(void)
{
    return clock_freq;
}

/// @brief convert a frequency to a timer count 
/// @param hz - desired frequency in hertz
/// @return the number of processor clock ticks in a single
/// period of the desired frequency
uint32_t tiva_frequency_to_counts(uint32_t hz)
{
    return tiva_clock_frequency()/hz;
}

void tiva_gpio_write(uint32_t port_pin, bool value)
{
    const uint32_t port_base = tiva_port_base(tiva_port_index(port_pin));
    const uint32_t pin_mask = tiva_pin_mask(tiva_pin_index(port_pin));
    GPIOPinWrite(port_base, pin_mask, value ? pin_mask : ~pin_mask);
}

/// @brief read the value of the pin. if pin is an output
/// then reading provides the last value set
/// @param pin - TIVA_GPIO(port, pin number)
/// @return - the pin value
bool tiva_gpio_read(uint32_t port_pin)
{
    const uint32_t port_base = tiva_port_base(tiva_port_index(port_pin));
    const uint32_t pin_mask = tiva_pin_mask(tiva_pin_index(port_pin));
    return GPIOPinRead(port_base, pin_mask) & 0xFF;
}

/// @brief invert the value of the pin
/// @param pin - TIVA_GPIO(port, pin number)
/// @post - the pin's value is the logical NOT of its previous value
void tiva_gpio_invert(uint32_t port_pin)
{
    tiva_gpio_write(port_pin, !tiva_gpio_read(port_pin));
}
