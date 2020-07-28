#include"nuhal/tiva.h"
#include"nuhal/error.h"
#include"nuhal/uart.h"
#include"driverlib/sysctl.h"
#include"driverlib/pin_map.h"
#include"driverlib/fpu.h"
#include"driverlib/systick.h"
#include"driverlib/interrupt.h"
#include"driverlib/gpio.h"
#include"inc/hw_sysctl.h"
#include"inc/hw_types.h"
#include"inc/hw_nvic.h"
#include"inc/hw_memmap.h"
#include"inc/hw_gpio.h"
#include"inc/hw_ints.h"
#include<stdint.h>


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
void tiva_setup(void)
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

uint32_t tiva_clock_hz(void)
{
    return clock_freq;
}

uint32_t tiva_clock_hz_to_counts(uint32_t hz)
{
    return tiva_clock_hz()/hz;
}

