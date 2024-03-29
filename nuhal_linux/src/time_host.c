#define _POSIX_C_SOURCE 199309L // enable posix so we can use clock_gettime
#include "nuhal/time.h"
#include "nuhal/error.h"
#include <time.h>

uint32_t time_period_ms(void)
{
    return UINT32_MAX;
}

uint32_t time_period_us(void)
{
    return UINT32_MAX;
}

uint32_t time_current_ms(void)
{
    struct timespec tspec;
    // tspec.tv_sec is whole seconds and tv_nsec is nanoseconds
    const int ret = clock_gettime(CLOCK_REALTIME, &tspec);
    if(0 != ret)
    {
        error_with_errno(FILE_LINE);
    }

    unsigned long newtime = tspec.tv_sec * 1000ul // seconds to ms
        + (unsigned long)tspec.tv_nsec/1000000ul; // nsec to ms
    return newtime % UINT32_MAX;
}

uint32_t time_current_us(void)
{
    struct timespec tspec;
    const int ret = clock_gettime(CLOCK_REALTIME, &tspec);
    if(0 != ret)
    {
        error_with_errno(FILE_LINE);
    }
    unsigned long newtime = tspec.tv_sec * 1000000ul // seconds to us
        + (unsigned long)tspec.tv_nsec/1000ul; // nsec to us
    return newtime % UINT32_MAX;
}
