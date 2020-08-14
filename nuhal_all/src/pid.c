#include "nuhal/pid.h"
#include "nuhal/error.h"
#include "nuhal/bytestream.h"
#include<math.h>

float pid_compute(const struct pid_gains * gains,
                  struct pid_state * st,
                  float reference,
                  float measurement)
{
    if(!st || !gains)
    {
        error(FILE_LINE, "NULL ptr");
    }

    const float error = reference - measurement;
    const float i_error = st->i_error + error;

    st->d_error = error - st->p_error;
    st->p_error = error;

    // control effort
    const float u = gains->kp * st->p_error
        + gains->ki * i_error
        + gains->kd * st->d_error;

    // integral anti-windup:
    // only increase the integral error under certain conditions
    // if input is not saturated.
    // if input is saturated HIGH but integral error is getting smaller
    // if input is saturated LOW but integral error is getting bigger
    // see https://jagger.berkeley.edu/~pack/me132/Section15.pdf
    if((gains->u_min < u && u < gains->u_max)
       || (error < 0 && u > gains->u_max)
       || (error > 0 && u < gains->u_min))
    {
        st->i_error = i_error;
    }

    return u;
}

void pid_gains_inject(struct bytestream * bs, const struct pid_gains * gains)
{
    if(!bs || !gains)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, gains->kp);
    bytestream_inject_f(bs, gains->ki);
    bytestream_inject_f(bs, gains->kd);
    bytestream_inject_f(bs, gains->u_max);
    bytestream_inject_f(bs, gains->u_min);
}

void pid_gains_extract(struct bytestream * bs, struct pid_gains * gains)
{
    if(!bs || !gains)
    {
        error(FILE_LINE, "NULL ptr");
    }
    gains->kp = bytestream_extract_f(bs);
    gains->ki = bytestream_extract_f(bs);
    gains->kd = bytestream_extract_f(bs);
    gains->u_max = bytestream_extract_f(bs);
    gains->u_min = bytestream_extract_f(bs);
}

void pid_state_inject(struct bytestream * bs, const struct pid_state * state)
{
    if(!bs || !state)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, state->p_error);
    bytestream_inject_f(bs, state->i_error);
    bytestream_inject_f(bs, state->d_error);
}


void pid_state_extract(struct bytestream * bs, struct pid_state * state)
{
    if(!bs || !state)
    {
        error(FILE_LINE, "NULL ptr");
    }
    state->p_error = bytestream_extract_f(bs);
    state->i_error = bytestream_extract_f(bs);
    state->d_error = bytestream_extract_f(bs);
}

/// @brief serialize pid signals
/// @param bs - bytestream into which the state should be inserted
/// @param signals - the state to insert into the bytestream
/// @pre - there must be enough space in the bytestream for the gains
void pid_signals_inject(struct bytestream * bs, const struct pid_signals * signals)
{
    if(!bs || !signals)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, signals->reference);
    bytestream_inject_f(bs, signals->measurement);
    bytestream_inject_f(bs, signals->effort);
}

/// @brief deserialize pid signals 
/// @param bs - the bytestream fromw hich to load the signals
/// @param state [out] - data from the bytestream is used to write the signal
/// @pre the bytestream must contain the data corresponding to signals
void pid_signals_extract(struct bytestream * bs, struct pid_signals * signals)
{
    if(!bs || !signals)
    {
        error(FILE_LINE, "NULL ptr");
    }
    signals->reference = bytestream_extract_f(bs);
    signals->measurement = bytestream_extract_f(bs);
    signals->effort = bytestream_extract_f(bs);
}


/// @brief serialize pid debug_info
/// @param bs - bytestream into which the state should be inserted
/// @param debug_info - the state to insert into the bytestream
/// @pre - there must be enough space in the bytestream for the gains
void pid_debug_info_inject(struct bytestream * bs, const struct pid_debug_info * debug_info)
{
    if(!bs || !debug_info)
    {
        error(FILE_LINE, "NULL ptr");
    }
    pid_state_inject(bs, &debug_info->state);
    pid_signals_inject(bs, &debug_info->signals);
    bytestream_inject_u8(bs, debug_info->sequence);
    bytestream_inject_u8(bs, debug_info->missed);
}

/// @brief deserialize pid debug_info 
/// @param bs - the bytestream fromw hich to load the debug_info
/// @param state [out] - data from the bytestream is used to write the signal
/// @pre the bytestream must contain the data corresponding to debug_info
void pid_debug_info_extract(struct bytestream * bs, struct pid_debug_info * debug_info)
{
    if(!bs || !debug_info)
    {
        error(FILE_LINE, "NULL ptr");
    }
    pid_state_extract(bs, &debug_info->state);
    pid_signals_extract(bs, &debug_info->signals);
    debug_info->sequence = bytestream_extract_u8(bs);
    debug_info->missed = bytestream_extract_u8(bs);
}
