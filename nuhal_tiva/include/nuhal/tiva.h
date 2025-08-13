#ifndef TIVA_H_INCLUDE_GUARD
#define TIVA_H_INCLUDE_GUARD
/// @brief tiva-specific utility functions that are useful to nuhal_impl

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

//*****************************************************************************
//
// System Control registers (SYSCTL), CPIO registers (PORTE), ADC registers (ADC0) for reading analog signals
//
//*****************************************************************************
#define SYSCTL_RCGCADC_R        (*((volatile uint32_t *)0x400FE638))
#define SYSCTL_RCGCADC_R0       0x00000001  // ADC Module 0 Run Mode Clock
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTE_AFSEL_R      (*((volatile uint32_t *)0x40024420))
#define SYSCTL_RCGCGPIO_R4      0x00000010  // GPIO Port E Run Mode Clock
#define GPIO_PORTE_DEN_R        (*((volatile uint32_t *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile uint32_t *)0x40024528))
#define ADC0_ACTSS_R            (*((volatile uint32_t *)0x40038000))
#define ADC_ACTSS_ASEN3         0x00000008  // ADC SS3 Enable
#define ADC0_EMUX_R             (*((volatile uint32_t *)0x40038014))
#define ADC_EMUX_EM3_M          0x0000F000  // SS3 Trigger Select
#define ADC0_SSMUX3_R           (*((volatile uint32_t *)0x400380A0))
#define ADC0_SSCTL3_R           (*((volatile uint32_t *)0x400380A4))
#define ADC_SSCTL3_IE0          0x00000004  // Sample Interrupt Enable
#define ADC_SSCTL3_END0         0x00000002  // End of Sequence
#define ADC0_PSSI_R             (*((volatile uint32_t *)0x40038028))
#define ADC_PSSI_SS3            0x00000008  // SS3 Initiate
#define ADC0_RIS_R              (*((volatile uint32_t *)0x40038004))
#define ADC_RIS_INR3            0x00000008  // SS3 Raw Interrupt Status
#define ADC0_SSFIFO3_R          (*((volatile uint32_t *)0x400380A8))
#define ADC0_ISC_R              (*((volatile uint32_t *)0x4003800C))
#define ADC_ISC_IN3             0x00000008  // SS3 Interrupt Status and Clear

#endif
