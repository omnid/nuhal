#include "nuhal/pid.h"
#include "nuhal/error.h"
#include "nuhal/bytestream.h"
#include<math.h>

float pid_compute(const struct pid_gains * gains,
                  struct pid_state * st,
                  float reference,
                  float measurement) {
    if(!st || !gains)
    {
        error(FILE_LINE, "NULL ptr");
    }

    const float error = reference - measurement;
    const float i_error = st->i_error + error;

    st->d_error = error - st->p_error;
    st->p_error = error;
    st->i_error = i_error;

    // Saturate integral accumuator (integral anti-windup)
    #warning TODO: Make the integral range a parameter
    if(st->i_error > gains->u_max * 0.2f)
    {
        st->i_error = gains->u_max * 0.2f;
    }
    else if (st->i_error < gains->u_min * 0.2f)
    {
        st->i_error = gains->u_min * 0.2f;
    }

    // Compute the control signal
    const float u_actual = gains->kp * st->p_error
        + gains->ki * st->i_error
        + gains->kd * st->d_error;

    // Bound the output of the controller
    if(u_actual > gains->u_max)
    {
        return gains->u_max;
    }
    else if(u_actual < gains->u_min)
    {
        return gains->u_min;
    }
    else
    {
        return u_actual;
    }
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
