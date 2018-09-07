#ifndef TIVA_H_INCLUDE_GUARD
#define TIVA_H_INCLUDE_GUARD
/// @brief tiva-specific utility functions that are useful to common_impl

#include<stdint.h>
#include<stdbool.h>
#include "driverlib/pin_map.h"

/// @brief general setup for tiva
void tiva_setup(void);

/// @brief get the clock frequency for the processor, in Hz
uint32_t tiva_clock_hz(void);

/// @brief enable the given peripheral and wait for it to be ready
/// @param peripheral - peripheral to enable, identified by its SYSCTL register
///          @see Tivaware SysCtlPeripheralEnable for valid values
void tiva_peripheral_enable(uint32_t peripheral);


/// @brief convert a frequency to a timer count 
/// @param hz - desired frequency in hertz
/// @return the number of processor clock ticks in a single
/// period of the desired frequency
uint32_t tiva_clock_hz_to_counts(uint32_t hz);


#endif
