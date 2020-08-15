#ifndef COMMON_TIME_H_INCLUDE_GUARD
#define COMMON_TIME_H_INCLUDE_GUARD
/// @file
/// @brief basic cross platform timing functions
/// Note that these are not suitable for ultra-precise work
#include<stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief get a current timestamp, in ms.
/// @return the ms elapsed since the timer started
/// Note that time is expected to count upwards
uint32_t time_current_ms(void);

/// @brief get the current timestamp, in us
/// @return the us elapsed since the timer started
/// Note that time is expected to count upwards
uint32_t time_current_us(void);

/// @brief the rollover period for the timer, in ms
uint32_t time_period_ms(void);

/// @brief the rollover period for the timer in us
uint32_t time_period_us(void);


/// @brief delay for a fixed amount of ms
/// @param ms amount of time to delay for
/// @pre the ms to delay must be less than
///   time_period_ms
void time_delay_ms(uint32_t ms);

/// @brief delay for a fixed amount of us
/// @param us amount of time to delay for
/// @pre the us to delay must be less than time_delay_us
void time_delay_us(uint32_t us);

/// @brief used for tracking elapsed time. Aids in
/// implemented loops with a timeout
struct time_elapsed
{
    uint32_t curr_time; /// the current time 
    uint32_t prev_time; /// the previous time
    uint32_t elapsed_time; /// the elapsed time
};

/// @brief time_elapsed_t wrapper for ms
/// prevents confusing ms and us versions
struct time_elapsed_ms
{
    struct time_elapsed ms;
}; 


/// @brief time_elapsed_t wrapper for us
/// prevents confusing ms and us versions
struct time_elapsed_us
{
    struct time_elapsed us;
};

/// @brief initialize an elapsed time counter. Call this to initialize
/// a time stamp to be used with time_elapsed_ms
/// @param stamp - a starting time stamp to  initialize
struct time_elapsed_ms time_elapsed_ms_init(void);

/// @brief initialize an elapsed time counter.  Creates a time
/// stamp to be used with time_elapsed_us
/// @param stamp - a starting time stamp to initialize
struct time_elapsed_us time_elapsed_us_init(void);

/// @brief get total time elapsed from initialization of the time stamp, in ms
/// @param stamp - the initial time stamp, elapsed time is taken from when
/// time_elapsed_ms_init(stamp) was called
/// @return the time elapsed since the call to time_elapsed_ms_init
/// Note that this function must be called at least once per rollover period
/// (that is time_period_ms() time) to maintain the proper elapsed time
uint32_t time_elapsed_ms(struct time_elapsed_ms * stamp);

/// @brief get total time elapsed from the initialization of time stamp, in us
/// @param stamp - the initial time stamp, elapsed time is taken from
/// time_elapsed_us_init(stamp) was called
/// @return the time elapsed since the call to time_elapsed_us_init
/// Note that this function must be called at least once per rollover period
/// (that is time_period_us() time) to maintain the proper elapsed time
uint32_t time_elapsed_us(struct time_elapsed_us * stamp);


/// @brief given a frequency return the period
/// @param hz - frequency, in hz
/// @return period, in us
static inline uint32_t time_frequency_to_period_us(uint32_t hz)
{
    return 1000000/hz;
}

/// @brief given a frequency return the period
/// @param hz - frequency, in hz
/// @return period, in us
static inline uint32_t time_frequency_to_period_ms(uint32_t hz)
{
    return time_frequency_to_period_us(hz)/1000;
}
#ifdef __cplusplus
}
#endif

#endif
