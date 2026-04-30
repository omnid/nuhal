#include "nuhal/tiva.h"
#include "nuhal/led.h"
#include "nuhal/pin_tiva.h"
#include"driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "nuhal/utilities.h"

/// test the priority of interrupts
/// The RED led flashes at 1 Hz
/// The blue led flashes at .5 Hz 5 times
/// Then the blue led is off and the green led is on
/// GPIO pins D1, D2, and D3, are also toggled, in sync
/// with the RED, BLUE, and GREEN led's respectively

#define PIN_RED PIN('D', 1)
#define PIN_BLUE PIN('D', 2)
#define PIN_GREEN PIN('D', 3)

static const struct pin_configuration pin_table[] =
{
    {PIN_RED, PIN_OUTPUT},
    {PIN_BLUE, PIN_OUTPUT},
    {PIN_GREEN, PIN_OUTPUT},
};

int main(void)
{
    tiva_setup();
    led_setup();
    pin_setup(pin_table, ARRAY_LEN(pin_table));
             
    // enable two timer interrupts that go at different frequencies.
    // timer 0 is the higher priority than timer 1.
    tiva_peripheral_enable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, tiva_clock_hz_to_counts(100));
    // priority only responds to upper 3 bits
    IntPrioritySet(INT_TIMER0A, 2 << 5) ;
    TimerEnable(TIMER0_BASE, TIMER_A);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    tiva_peripheral_enable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, tiva_clock_hz_to_counts(50));
    IntPrioritySet(INT_TIMER1A, 4 << 5);
    TimerEnable(TIMER1_BASE, TIMER_A);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    for(;;)
    {
        ;
    }

}

void Timer0AISR(void)
{
    static int count = 0;
    if(count % 100 == 0)
    {
        pin_invert(PIN_LED_RED);
        pin_invert(PIN_RED);
    }
    ++count;
    TimerIntClear(TIMER0_BASE, TIMER_A);
}

void Timer1AISR(void)
{
    static int count = 0;
    if(count % 100 == 0)
    {
        pin_invert(PIN_LED_BLUE);
        pin_invert(PIN_BLUE);
    }
    // stop toggling the blue led. red led should still blink
    // even though we never exit this interrupt since timer0 is
    // a higher priority
    if(count > 500)
    {
        pin_write(PIN_LED_BLUE, LED_OFF);
        pin_write(PIN_BLUE, false);
        pin_write(PIN_LED_GREEN, LED_ON);
        pin_write(PIN_GREEN, true);
        for(;;)
        {
            ;
        }
    }
    ++count;
    TimerIntClear(TIMER1_BASE, TIMER_A);
}
