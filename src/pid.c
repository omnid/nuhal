#include "nutilities/pid.h"
#include "nutilities/error.h"
#include "nutilities/bytestream.h"
#include<math.h>

float pid_compute(struct pid_state * st, float reference, float measurement)
{
    if(!st)
    {
        error(FILE_LINE, "NULL ptr");
    }

    const float error = reference - measurement;
    const float i_error = st->i_error + error;

    st->d_error = error - st->p_error;
    st->p_error = error;

    // control effort
    const float u = st->gains.kp * st->p_error
        + st->gains.ki * i_error
        + st->gains.kd * st->d_error;

    // integral anti-windup:
    // only increase the integral error under certain conditions
    // if input is not saturated.
    // if input is saturated HIGH but integral error is getting smaller
    // if input is saturated LOW but integral error is getting bigger
    // see https://jagger.berkeley.edu/~pack/me132/Section15.pdf
    if((st->u_min < u && u < st->u_max)
       || (error < 0 && u > st->u_max)
       || (error > 0 && u < st->u_min))
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
}

void pid_inject_data(struct bytestream * bs, struct pid_data * data)
{
    if(!bs || !data)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, data->p_error);
    bytestream_inject_f(bs, data->i_error);
    bytestream_inject_f(bs, data->d_error);
    bytestream_inject_f(bs, data->u_effort);
    bytestream_inject_f(bs, data->r_reference);
    bytestream_inject_f(bs, data->z_measurement);
    bytestream_inject_u8(bs, data->sequence);
    bytestream_inject_u8(bs, data->missed);
}


void pid_extract_data(struct bytestream * bs, struct pid_data * data)
{
    data->p_error = bytestream_extract_f(bs);
    data->i_error = bytestream_extract_f(bs);
    data->d_error = bytestream_extract_f(bs);
    data->u_effort = bytestream_extract_f(bs);
    data->r_reference = bytestream_extract_f(bs);
    data->z_measurement = bytestream_extract_f(bs);
    data->sequence = bytestream_extract_u8(bs);
    data->missed = bytestream_extract_u8(bs);
}
