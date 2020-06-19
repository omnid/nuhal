#include "nutilities/pid.h"
#include "nutilities/error.h"
#include "nutilities/bytestream.h"
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

void pid_inject_gains(struct bytestream * bs, const struct pid_gains * gains)
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

void pid_extract_gains(struct bytestream * bs, struct pid_gains * gains)
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

void pid_inject_state(struct bytestream * bs, const struct pid_state * state)
{
    if(!bs || !state)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, state->p_error);
    bytestream_inject_f(bs, state->i_error);
    bytestream_inject_f(bs, state->d_error);
}


void pid_extract_state(struct bytestream * bs, struct pid_state * state)
{
    state->p_error = bytestream_extract_f(bs);
    state->i_error = bytestream_extract_f(bs);
    state->d_error = bytestream_extract_f(bs);
}
