#include "common/time.h"
#include "tiva/common_tiva.h"
#include "driverlib/systick.h"

uint32_t time_current_ms(void)
{
    // tick count, counting up instead of down
    // the count is at most 24 bits, so we left shift by 8 to avoid overflow
    // while increasing precision
    // ticks * msec/ tick = current ms, right shifted 8 to compensate
    return (((SysTickPeriodGet() - SysTickValueGet()) << 8)
            / (tiva_clock_frequency()/1000u)) >> 8;
}

uint32_t time_current_us(void)
{
    // tick count, counting up instead of down
    // the count is at most 24 bits, so we left shift by 8 to avoid overflow
    // while increasing precision
    // ticks * usec/ tick = current us, right shifted 8 to compensate
    return (((SysTickPeriodGet() - SysTickValueGet()) << 8)
            / (tiva_clock_frequency()/1000000u)) >> 8;
}

uint32_t time_period_ms(void)
{
    // ticks per cycle * seconds/cycle * 1000 ms/second
    // beware of overflow.  The period is at most 24 bits, so we
    // left shift the numerator by 8 bits (guaranteed to not overflow)
    // then shift everything back at the end
    return ((SysTickPeriodGet() << 8)  / (tiva_clock_frequency()/1000u)) >> 8;
}

uint32_t time_period_us(void)
{
    // ticks per cycle * seconds/cycle * 1,000,000 us/second
    // beware of overflow.  The period is at most 24 bits, so we
    // left shift the numerator by 8 bits (guaranteed to not overflow)
    // then shift everything back at the end
    return ((SysTickPeriodGet() << 8)  / (tiva_clock_frequency()/1000000u)) >> 8;
}
