#include "common/time.h"
#include "common/error.h"
#include <stdbool.h>

void time_delay_ms(uint32_t ms)
{
    struct time_elapsed_ms stamp = time_elapsed_ms_init();

    while(time_elapsed_ms(&stamp) < ms)
    {
        ;
    }
}

void time_delay_us(uint32_t us)
{
    struct time_elapsed_us stamp =  time_elapsed_us_init();

    while(time_elapsed_us(&stamp) < us)
    {
        ;
    }
}

struct time_elapsed_ms time_elapsed_ms_init(void)
{
    struct time_elapsed_ms stamp = {{time_current_ms(), 0, 0}};
    return stamp;
}

struct time_elapsed_us time_elapsed_us_init()
{
    struct time_elapsed_us stamp = {{time_current_us(), 0, 0}};
    return stamp;
}

/// if ms is true use ms else use microseconds (us)
/// @param stamp - the timestamp,
/// @param currtime - the current time
static uint32_t time_elapsed(struct time_elapsed * stamp, bool ms, uint32_t currtime)
{
    if(!stamp)
    {
        error(FILE_LINE, "stamp is NULL");
    }
    stamp->prev_time = stamp->curr_time;
    stamp->curr_time = currtime;

    const uint32_t period = ms ? time_period_ms() : time_period_us();

    if(stamp->curr_time >= stamp->prev_time)
    {
        stamp->elapsed_time += stamp->curr_time - stamp->prev_time;
    }
    else
    {
        // period - stamp->prev_time is the time it took to rollover
        // stamp->curr_time is the time since the rollover
        stamp->elapsed_time += (period - stamp->prev_time) + stamp->curr_time;
    }
    return stamp->elapsed_time;
}

uint32_t time_elapsed_ms(struct time_elapsed_ms * stamp)
{
    const uint32_t currtime = time_current_ms();
    return time_elapsed(&(stamp->ms), true, currtime);
}

uint32_t time_elapsed_us(struct time_elapsed_us * stamp)
{
    const uint32_t currtime = time_current_us();
    return time_elapsed(&(stamp->us), false, currtime);
}
