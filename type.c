#include "common/type.h"
#include "common/bytestream.h"
#include "common/utilities.h"
#include "common/error.h"

/// @brief serialize a processor id
/// @param bs - the bytestream to place the id into
/// @param id - the processor id
/// @pre there must be enough room in bs for the id
void type_inject_processor_id(struct bytestream * bs,
                              enum type_processor_id id)
{
    bytestream_inject_u16(bs, id);
}

/// @brief deserialize a processor id
/// @param bs - the bytestream to place the id into
/// @return the processor id
/// @pre bs must contain a valid processor id
enum type_processor_id type_extract_processor_id(struct bytestream * bs)
{
    return (enum type_processor_id)bytestream_extract_u16(bs);
}

void type_inject_joint_state(struct bytestream * bs,
                                   const struct type_joint_state * js)
{
    if(!bs || !js)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, js->radians);
    bytestream_inject_f(bs, js->rad_sec);
    bytestream_inject_f(bs, js->newton_meters);
}

void type_extract_joint_state(struct bytestream * bs,
                                    struct type_joint_state * out)
{
    if(!bs || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    out->radians = bytestream_extract_f(bs);
    out->rad_sec = bytestream_extract_f(bs);
    out->newton_meters = bytestream_extract_f(bs);
}

void type_inject_arm_state(struct bytestream * bs,
                                 const struct type_arm_state * as)
{
    if(!bs || !as)
    {
        error(FILE_LINE, "NULL ptr");
    }
    type_inject_joint_state(bs, &as->before);
    type_inject_joint_state(bs, &as->after);
}

void type_extract_arm_state(struct bytestream * bs,
                                  struct type_arm_state * as)
{
    if(!bs || !as)
    {
        error(FILE_LINE, "NULL ptr");
    }
    type_extract_joint_state(bs, &as->before);
    type_extract_joint_state(bs, &as->after);
}


void type_inject_linear_position(struct bytestream * bs,
                                 const struct type_linear_position * lp)
{
    if(!bs || !lp)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, lp->x);
    bytestream_inject_f(bs, lp->y);
    bytestream_inject_f(bs, lp->z);
}
void type_extract_linear_position(struct bytestream * bs,
                                  struct type_linear_position * lp)
{
    if(!bs || !lp)
    {
        error(FILE_LINE, "NULL ptr");
    }
    lp->x = bytestream_extract_f(bs);
    lp->y = bytestream_extract_f(bs);
    lp->z = bytestream_extract_f(bs);
}

void type_inject_linear_velocity(struct bytestream * bs,
                                       const struct type_linear_velocity * lp)
{
    if(!bs || !lp)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, lp->xdot);
    bytestream_inject_f(bs, lp->ydot);
    bytestream_inject_f(bs, lp->zdot);
}
void type_extract_linear_velocity(struct bytestream * bs,
                                        struct type_linear_velocity * lp)
{
    if(!bs || !lp)
    {
        error(FILE_LINE, "NULL ptr");
    }
    lp->xdot = bytestream_extract_f(bs);
    lp->ydot = bytestream_extract_f(bs);
    lp->zdot = bytestream_extract_f(bs);
}

void type_inject_linear_force(struct bytestream * bs,
                                       const struct type_linear_force * lp)
{
    if(!bs || !lp)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_f(bs, lp->fx);
    bytestream_inject_f(bs, lp->fy);
    bytestream_inject_f(bs, lp->fz);
}
void type_extract_linear_force(struct bytestream * bs,
                                        struct type_linear_force * lp)
{
    if(!bs || !lp)
    {
        error(FILE_LINE, "NULL ptr");
    }
    lp->fx = bytestream_extract_f(bs);
    lp->fy = bytestream_extract_f(bs);
    lp->fz = bytestream_extract_f(bs);
}

void type_inject_joint_encoders(struct bytestream * bs,
                                const struct type_joint_encoders * enc)
{
    if(!bs || !enc)
    {
        error(FILE_LINE, "NULL ptr");
    }
    bytestream_inject_u32(bs, enc->before_raw);
    bytestream_inject_f(bs, enc->before_radians);
    bytestream_inject_u32(bs, enc->after_raw);
    bytestream_inject_f(bs, enc->after_radians);
}

/// @brief deserialize encoder data from the bytestream
/// @param bs - the bytestream
/// @param out [out] - the encoder data read from the stream
void type_extract_joint_encoders(struct bytestream * bs,
                                 struct type_joint_encoders * out)
{
    if(!bs || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    out->before_raw = bytestream_extract_u32(bs);
    out->before_radians = bytestream_extract_f(bs);
    out->after_raw = bytestream_extract_u32(bs);
    out->after_radians = bytestream_extract_f(bs);
}


void type_inject_delta_state(struct bytestream * bs,
                             const struct type_delta_state * ds)
{
    if(!bs || !ds)
    {
        error(FILE_LINE, "NULL ptr");
    }
    type_inject_arm_state(bs, &ds->arms[0]);
    type_inject_arm_state(bs, &ds->arms[1]);
    type_inject_arm_state(bs, &ds->arms[2]);
    type_inject_linear_position(bs, &ds->platform_pos);
    type_inject_linear_velocity(bs, &ds->platform_vel);
    type_inject_linear_force(bs, &ds->platform_force);
}

/// @brief deserialize delta state data from the bytestream
/// @param bs - the bytestream
/// @param out [out] - the encoder data read from the stream
void type_extract_delta_state(struct bytestream * bs,
                                 struct type_delta_state * out)
{
    if(!bs || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    type_extract_arm_state(bs, &out->arms[0]);
    type_extract_arm_state(bs, &out->arms[1]);
    type_extract_arm_state(bs, &out->arms[2]);
    type_extract_linear_position(bs, &out->platform_pos);
    type_extract_linear_velocity(bs, &out->platform_vel);
    type_extract_linear_force(bs, &out->platform_force);
}

void type_inject_led_color(struct bytestream * bs, enum type_led_color color)
{
    if(!bs)
    {
        error(FILE_LINE, "NULL ptr");
    }
    if(color > 0x7)
    {
        error(FILE_LINE, "invalid color");
    }
    bytestream_inject_u8(bs, color);
}

/// @param bs - the bytestream
/// @return the color that was stored in the bytestream
enum type_led_color type_extract_led_color(struct bytestream * bs)
{
    if(!bs)
    {
        error(FILE_LINE, "NULL ptr");
    }
    enum type_led_color color = (enum type_led_color)bytestream_extract_u8(bs);
    if(color > 0x7)
    {
        error(FILE_LINE, "invalid color");
    }
    return color;
}


enum type_control_effort type_control_effort_extract(struct bytestream * bs)
{
    return (enum type_control_effort)bytestream_extract_u8(bs);
}

void type_control_effort_inject(struct bytestream * bs, enum type_control_effort ef)
{
    bytestream_inject_u8(bs, ef);
}

void type_inject_twist(struct bytestream * bs, const struct type_twist * v)
{
  if(!bs || !v)
  {
    error(FILE_LINE, "NULL ptr");
  }

  bytestream_inject_f(bs, v->wz);
  bytestream_inject_f(bs, v->vx);
  bytestream_inject_f(bs, v->vy);
}

void type_extract_twist(struct bytestream * bs, struct type_twist * v)
{
  if(!bs || !v)
  {
    error(FILE_LINE, "NULL ptr");
  }

  v->wz = bytestream_extract_f(bs);
  v->vx = bytestream_extract_f(bs);
  v->vy = bytestream_extract_f(bs);
}

void type_inject_wheel_velocities(struct bytestream * bs, struct type_wheel_velocities * u)
{
  if(!bs || !u)
  {
    error(FILE_LINE, "NULL ptr");
  }

  bytestream_inject_f(bs, u->right);
  bytestream_inject_f(bs, u->left);
}

void type_extract_wheel_velocities(struct bytestream * bs, struct type_wheel_velocities * u)
{
  if(!bs || !u)
  {
    error(FILE_LINE, "NULL ptr");
  }

  u->right = bytestream_extract_f(bs);
  u->left = bytestream_extract_f(bs);
}

void type_inject_omni_velocities(struct bytestream * bs, struct type_omni_velocities * u)
{
  if(!bs || !u)
  {
    error(FILE_LINE, "NULL ptr");
  }

  bytestream_inject_f(bs, u->uFR);
  bytestream_inject_f(bs, u->uFL);
  bytestream_inject_f(bs, u->uRR);
  bytestream_inject_f(bs, u->uRL);
}

void type_extract_omni_velocities(struct bytestream * bs, struct type_omni_velocities * u)
{
  if(!bs || !u)
  {
    error(FILE_LINE, "NULL ptr");
  }

  u->uFR = bytestream_extract_f(bs);
  u->uFL = bytestream_extract_f(bs);
  u->uRR = bytestream_extract_f(bs);
  u->uRL = bytestream_extract_f(bs);
}
