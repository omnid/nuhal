#include "nuhal/encoder.h"
#include "nuhal/error.h"
#include "nuhal/bytestream.h"

int32_t encoder_ticks(const struct encoder * enc, struct encoder_raw raw)
{
    if(!enc)
    {
        error(FILE_LINE, "NULL ptr");
    }

    const uint32_t ticks = raw.single;

    // the angle over the full interval, from 0 to ticks_per_rev
    uint32_t angle_full = 0;
    if(ticks > enc->zero_angle_ticks)
    {
        const uint32_t zeroed = ticks - enc->zero_angle_ticks;
        if(enc->orientation == ENCODER_ORIENTATION_UP_UP)
        {
            angle_full = zeroed;
        }
        else
        {
            angle_full = enc->ticks_per_rev - zeroed;
        }
    }
    else
    {
        const uint32_t zeroed = enc->zero_angle_ticks - ticks;
        if(enc->orientation == ENCODER_ORIENTATION_UP_UP)
        {
            angle_full = enc->ticks_per_rev - zeroed;
        }
        else
        {
            angle_full = zeroed;
        }
    }

    // range in which to place the angle
    const int32_t upper = enc->ticks_per_rev/2;

    // convert an angle in the range from 0 to ticks_per_rev
    // to one in the range of
    // -enc_ticks_per_rev/2 < angle < enc_ticks_per_rev/2
    return angle_full < (uint32_t)upper ?
        (int32_t)angle_full :
        -(int32_t)(enc->ticks_per_rev - angle_full);
}


float encoder_radians(const struct encoder * enc, int32_t ticks)
{
    if(!enc)
    {
        error(FILE_LINE, "NULL ptr");
    }
    return 2.0f*PI*(float)(ticks)/(float)enc->ticks_per_rev;
}


uint32_t encoder_zero_raw(const struct encoder * enc,
                          struct encoder_raw raw,
                          float radians)
{
    if(!enc)
    {
        error(FILE_LINE, "NULL ptr");
    }

    if(radians <= -PI || radians > PI)
    {
        error(FILE_LINE,"out of range");
    }

    // this is the number of ticks to achieve an angular displacement of radians
    const int32_t ticks = radians*(float)enc->ticks_per_rev/(2.0f*PI);

    const uint32_t uticks = ticks < 0 ?
        (uint32_t)ticks + enc->ticks_per_rev :
        (uint32_t)ticks;

    if(enc->orientation == ENCODER_ORIENTATION_UP_UP)
    {
        // perform raw.single - uticks, while accounting
        // for underflow and wrap around
        if(uticks < raw.single)
        {
            return raw.single - uticks;
        }
        else
        {
            return enc->ticks_per_rev + raw.single - uticks;
        }
    }
    else if(enc->orientation == ENCODER_ORIENTATION_UP_DOWN)
    {
        return (raw.single + uticks) % enc->ticks_per_rev;
    }
    else
    {
        error(FILE_LINE, "unknown orientation");
    }
}

void encoder_joints_inject(struct bytestream * bs,
                          const struct encoder_joints * enc)
{
    if(!bs || !enc)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_i32(bs, enc->before_ticks);
    bytestream_inject_f(bs, enc->before_radians);
    bytestream_inject_i32(bs, enc->after_ticks);
    bytestream_inject_f(bs, enc->after_radians);
}

void encoder_joints_extract(struct bytestream * bs,
                           struct encoder_joints * out)
{
    if(!bs || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    out->before_ticks = bytestream_extract_i32(bs);
    out->before_radians = bytestream_extract_f(bs);
    out->after_ticks = bytestream_extract_i32(bs);
    out->after_radians = bytestream_extract_f(bs);
}

void encoder_gimbal_inject(struct bytestream * bs,
                          const struct encoder_gimbal * enc)
{
    if(!bs || !enc)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_i32(bs, enc->x_ticks);
    bytestream_inject_i32(bs, enc->y_ticks);
    bytestream_inject_i32(bs, enc->z_ticks);

    bytestream_inject_f(bs, enc->x_radians);
    bytestream_inject_f(bs, enc->y_radians);
    bytestream_inject_f(bs, enc->z_radians);
}

void encoder_gimbal_extract(struct bytestream * bs,
                           struct encoder_gimbal * out)
{
    if(!bs || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    out->x_ticks = bytestream_extract_i32(bs);
    out->y_ticks = bytestream_extract_i32(bs);
    out->z_ticks = bytestream_extract_i32(bs);

    out->x_radians = bytestream_extract_f(bs);
    out->y_radians = bytestream_extract_f(bs);
    out->z_radians = bytestream_extract_f(bs);
}
