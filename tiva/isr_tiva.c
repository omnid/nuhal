#include <stdint.h>
#include "common/utilities.h"
#include "common/error.h"
#include "driverlib/fpu.h"

// define the ResetISR, which is
// different betweent he two compilers, but for some reason seems to need
// to be defined in this translation unit to work

#ifdef __TI_ARM__ // used when compiling for ti-cgt
/// @brief c library startup code
extern void c_int00(void);

/// @brief This ISR is called on processor reset. It calls the c initialization routine
static void ResetISR(void)
{
    // Go to the TI C initialization routine which sets up the C runtime
    // and enables floating point. The code then calls main
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}
#else // arm-none-eabi gcc startup code


// defined in linker script indicating location of various segments
extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

// the main applciation function
extern int main(void);

// called on a reset. must not be static to expose this entry point to the linker
void ResetISR(void)
{
    uint32_t *pui32Src, *pui32Dest;

    // Copy the data segment initializers from flash to SRAM.
    pui32Src = &__data_load__;
    for(pui32Dest = &__data_start__; pui32Dest < &__data_end__; )
    {
        *pui32Dest++ = *pui32Src++;
    }

    // Zero fill the bss segment.
    __asm("    ldr     r0, =__bss_start__\n"
          "    ldr     r1, =__bss_end__\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    // Enable the floating-point unit.  This must be done here to handle the
    // case where main() uses floating-point and the function prologue saves
    // floating-point registers (which will fault if floating-point is not
    // enabled).  Any configuration of the floating-point unit using DriverLib
    // APIs must be done here prior to the floating-point unit being enabled.
    FPUEnable();
    main();
}
#endif

/// @brief non-maskable interrupts (these occur when bad conditions happen on the processor
__attribute__((weak))
void NmiISR(void)
{
    error(FILE_LINE, "NMI");
}

/// @brief fault isr. These are errors that the processor can detect (such as divide by zero)
/// See section 2.6.1 of the datasheet for more information about this fault.
/// By default, loop forever so state is preserved and can be captured by the debugger.
/// This is weak so it can be overridden by user code
__attribute__((weak))
void FaultISR(void)
{
    error(FILE_LINE, "FAULT");
}

__attribute__((weak))
void Can0ISR(void)
{
    error(FILE_LINE, "CANISR");
}

// uart interrupts are triggered on error conditions, however,
// this feature must be explicitly enabled.
__attribute__((weak))
void Uart0ISR(void)
{
    error(FILE_LINE, "UART0_ISR");
}

__attribute__((weak))
void Uart1ISR(void)
{
    error(FILE_LINE, "UART1_ISR");
}

__attribute__((weak))
void Uart2ISR(void)
{
    error(FILE_LINE, "UART2_ISR");
}

__attribute__((weak))
void Uart3ISR(void)
{
    error(FILE_LINE, "UART3_ISR");
}

__attribute__((weak))
void Uart4ISR(void)
{
    error(FILE_LINE, "UART4_ISR");
}

__attribute__((weak))
void Uart5ISR(void)
{
    error(FILE_LINE, "UART5_ISR");
}

__attribute__((weak))
void Uart6ISR(void)
{
    error(FILE_LINE, "UART6_ISR");
}

__attribute__((weak))
void Uart7ISR(void)
{
    error(FILE_LINE, "UART7_ISR");
}

__attribute__((weak))
void Timer0AISR(void)
{
    error(FILE_LINE, "TIMER0A_ISR");
}

__attribute__((weak))
void Timer0BISR(void)
{
    error(FILE_LINE, "TIMER0B_ISR");
}

__attribute__((weak))
void Timer1AISR(void)
{
    error(FILE_LINE, "TIMER1A_ISR");
}

__attribute__((weak))
void Timer1BISR(void)
{
    error(FILE_LINE, "TIMER1B_ISR");
}


__attribute__((weak))
void Timer2AISR(void)
{
    error(FILE_LINE, "TIMER2A_ISR");
}

__attribute__((weak))
void Timer2BISR(void)
{
    error(FILE_LINE, "TIMER2B_ISR");
}


__attribute__((weak))
void Timer3AISR(void)
{
    error(FILE_LINE, "TIMER3A_ISR");
}

__attribute__((weak))
void Timer3BISR(void)
{
    error(FILE_LINE, "TIMER3B_ISR");
}


__attribute__((weak))
void Timer4AISR(void)
{
    error(FILE_LINE, "TIMER4A_ISR");
}

__attribute__((weak))
void Timer4BISR(void)
{
    error(FILE_LINE, "TIMER4B_ISR");
}


__attribute__((weak))
void Timer5AISR(void)
{
    error(FILE_LINE, "TIMER5A_ISR");
}

__attribute__((weak))
void Timer5BISR(void)
{
    error(FILE_LINE, "TIMER5B_ISR");
}
/// @brief default isr handler
__attribute__((weak))
void IntDefaultHandler(void)
{
    // as you need interrupts add them here.  for now this
    // default is to remind you
    error(FILE_LINE, "UNDEFINED");
}


// a variable holding the top of the stack address
// we need t
extern uintptr_t __STACK_END;

// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
// this is placed at .intvecs section by the linker script
__attribute__ ((section(".intvecs"))) 
void (* const tlib_int_vectors[])(void) =
{
    // initial stack pointer (defined in linker script)
    (void (*)(void))(uint32_t)&__STACK_END, 

    ResetISR,                 // The reset handler
    NmiISR,                   // The NMI handler
    FaultISR,                 // The hard fault handler
    IntDefaultHandler,        // The MPU fault handler
    IntDefaultHandler,        // The bus fault handler
    IntDefaultHandler,        // The usage fault handler
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    IntDefaultHandler,        // SVCall handler
    IntDefaultHandler,        // Debug monitor handler
    0,                        // Reserved
    IntDefaultHandler,        // The PendSV handler
    IntDefaultHandler,        // The SysTick handler
    IntDefaultHandler,        // GPIO Port A
    IntDefaultHandler,        // GPIO Port B
    IntDefaultHandler,        // GPIO Port C
    IntDefaultHandler,        // GPIO Port D
    IntDefaultHandler,        // GPIO Port E
    Uart0ISR,                 // UART0 Rx and Tx
    IntDefaultHandler,        // UART1 Rx and Tx
    IntDefaultHandler,        // SSI0 Rx and Tx
    IntDefaultHandler,        // I2C0 Master and Slave
    IntDefaultHandler,        // PWM Fault
    IntDefaultHandler,        // PWM Generator 0
    IntDefaultHandler,        // PWM Generator 1
    IntDefaultHandler,        // PWM Generator 2
    IntDefaultHandler,        // Quadrature Encoder 0
    IntDefaultHandler,        // ADC Sequence 0
    IntDefaultHandler,        // ADC Sequence 1
    IntDefaultHandler,        // ADC Sequence 2
    IntDefaultHandler,        // ADC Sequence 3
    IntDefaultHandler,        // Watchdog timer
    Timer0AISR,               // Timer 0 subtimer A
    Timer0BISR,               // Timer 0 subtimer B
    Timer1AISR,               // Timer 1 subtimer A
    Timer1BISR,               // Timer 1 subtimer B
    Timer2AISR,        // Timer 2 subtimer A
    Timer2BISR,        // Timer 2 subtimer B
    IntDefaultHandler,        // Analog Comparator 0
    IntDefaultHandler,        // Analog Comparator 1
    IntDefaultHandler,        // Analog Comparator 2
    IntDefaultHandler,        // System Control (PLL, OSC, BO)
    IntDefaultHandler,        // FLASH Control
    IntDefaultHandler,        // GPIO Port F
    IntDefaultHandler,        // GPIO Port G
    IntDefaultHandler,        // GPIO Port H
    IntDefaultHandler,        // UART2 Rx and Tx
    IntDefaultHandler,        // SSI1 Rx and Tx
    Timer3AISR,        // Timer 3 subtimer A
    Timer3BISR,        // Timer 3 subtimer B
    IntDefaultHandler,        // I2C1 Master and Slave
    IntDefaultHandler,        // Quadrature Encoder 1
    Can0ISR,                  // CAN0
    IntDefaultHandler,        // CAN1
    0,                        // Reserved
    0,                        // Reserved
    IntDefaultHandler,        // Hibernate
    IntDefaultHandler,        // USB0
    IntDefaultHandler,        // PWM Generator 3
    IntDefaultHandler,        // uDMA Software Transfer
    IntDefaultHandler,        // uDMA Error
    IntDefaultHandler,        // ADC1 Sequence 0
    IntDefaultHandler,        // ADC1 Sequence 1
    IntDefaultHandler,        // ADC1 Sequence 2
    IntDefaultHandler,        // ADC1 Sequence 3
    0,                        // Reserved
    0,                        // Reserved
    IntDefaultHandler,        // GPIO Port J
    IntDefaultHandler,        // GPIO Port K
    IntDefaultHandler,        // GPIO Port L
    IntDefaultHandler,        // SSI2 Rx and Tx
    IntDefaultHandler,        // SSI3 Rx and Tx
    IntDefaultHandler,        // UART3 Rx and Tx
    IntDefaultHandler,        // UART4 Rx and Tx
    IntDefaultHandler,        // UART5 Rx and Tx
    IntDefaultHandler,        // UART6 Rx and Tx
    IntDefaultHandler,        // UART7 Rx and Tx
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    IntDefaultHandler,        // I2C2 Master and Slave
    IntDefaultHandler,        // I2C3 Master and Slave
    Timer4AISR,        // Timer 4 subtimer A
    Timer4BISR,        // Timer 4 subtimer B
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    0,                        // Reserved
    Timer5AISR,        // Timer 5 subtimer A
    Timer5BISR,        // Timer 5 subtimer B
    IntDefaultHandler,        // Wide Timer 0 subtimer A
    IntDefaultHandler,        // Wide Timer 0 subtimer B
    IntDefaultHandler,        // Wide Timer 1 subtimer A
    IntDefaultHandler,        // Wide Timer 1 subtimer B
    IntDefaultHandler,        // Wide Timer 2 subtimer A
    IntDefaultHandler,        // Wide Timer 2 subtimer B
    IntDefaultHandler,        // Wide Timer 3 subtimer A
    IntDefaultHandler,        // Wide Timer 3 subtimer B
    IntDefaultHandler,        // Wide Timer 4 subtimer A
    IntDefaultHandler,        // Wide Timer 4 subtimer B
    IntDefaultHandler,        // Wide Timer 5 subtimer A
    IntDefaultHandler,        // Wide Timer 5 subtimer B
    IntDefaultHandler,        // FPU
    0,                        // Reserved
    0,                        // Reserved
    IntDefaultHandler,        // I2C4 Master and Slave
    IntDefaultHandler,        // I2C5 Master and Slave
    IntDefaultHandler,        // GPIO Port M
    IntDefaultHandler,        // GPIO Port N
    IntDefaultHandler,        // Quadrature Encoder 2
    0,                        // Reserved
    0,                        // Reserved
    IntDefaultHandler,        // GPIO Port P (Summary or P0)
    IntDefaultHandler,        // GPIO Port P1
    IntDefaultHandler,        // GPIO Port P2
    IntDefaultHandler,        // GPIO Port P3
    IntDefaultHandler,        // GPIO Port P4
    IntDefaultHandler,        // GPIO Port P5
    IntDefaultHandler,        // GPIO Port P6
    IntDefaultHandler,        // GPIO Port P7
    IntDefaultHandler,        // GPIO Port Q (Summary or Q0)
    IntDefaultHandler,        // GPIO Port Q1
    IntDefaultHandler,        // GPIO Port Q2
    IntDefaultHandler,        // GPIO Port Q3
    IntDefaultHandler,        // GPIO Port Q4
    IntDefaultHandler,        // GPIO Port Q5
    IntDefaultHandler,        // GPIO Port Q6
    IntDefaultHandler,        // GPIO Port Q7
    IntDefaultHandler,        // GPIO Port R
    IntDefaultHandler,        // GPIO Port S
    IntDefaultHandler,        // PWM 1 Generator 0
    IntDefaultHandler,        // PWM 1 Generator 1
    IntDefaultHandler,        // PWM 1 Generator 2
    IntDefaultHandler,        // PWM 1 Generator 3
    IntDefaultHandler         // PWM 1 Fault
};


