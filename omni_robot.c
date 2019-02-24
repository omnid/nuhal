#include "common/matrix.h"
#include "common/parameters.h"
#include "common/omni_robot.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/// @brief H matrix for the omni robot used for getting wheel velocities
/// from a given twist. See Eq 13.10 in Modern Robotics.
static const struct matrix_4x3 OMNI_H_MATRIX =
{
    .data =
    {
        {-(PARAMETERS_OMNI_LENGTH+PARAMETERS_OMNI_WIDTH)/PARAMETERS_OMNI_WHEEL_RADIUS,	1/PARAMETERS_OMNI_WHEEL_RADIUS,	-1/PARAMETERS_OMNI_WHEEL_RADIUS},
        {(PARAMETERS_OMNI_LENGTH+PARAMETERS_OMNI_WIDTH)/PARAMETERS_OMNI_WHEEL_RADIUS,	1/PARAMETERS_OMNI_WHEEL_RADIUS,	1/PARAMETERS_OMNI_WHEEL_RADIUS},
        {(PARAMETERS_OMNI_LENGTH+PARAMETERS_OMNI_WIDTH)/PARAMETERS_OMNI_WHEEL_RADIUS,	1/PARAMETERS_OMNI_WHEEL_RADIUS,	-1/PARAMETERS_OMNI_WHEEL_RADIUS},
        {-(PARAMETERS_OMNI_LENGTH+PARAMETERS_OMNI_WIDTH)/PARAMETERS_OMNI_WHEEL_RADIUS,	1/PARAMETERS_OMNI_WHEEL_RADIUS,	1/PARAMETERS_OMNI_WHEEL_RADIUS}
    },
    .transpose = false
};

/// @brief Pseudoinverse of the H matrix for getting a twist from a given
/// set of wheel velocities. See Eq 13.10 in Modern Robotics.
static const struct matrix_4x3 OMNI_H_PINV =
{
    .data =
    {
        {-0.0601468, 0.0254, -0.0254},
        {0.0601468, 0.0254, 0.0254},
        {0.0601468, 0.0254, -0.0254},
        {-0.0601468, 0.0254, 0.0254}
    },
    .transpose = true
};

void omni_robot_twist_to_vels(const struct matrix_3x1 * v, struct matrix_4x1 * u)
{
    // Matrix multiplication to get wheel velocities from input twist
    matrix_4x3_3x1_multiply_vector(&OMNI_H_MATRIX, v, u);
}

void omni_robot_vels_to_twist(const struct matrix_4x1 * u, struct matrix_3x1 * v)
{
    // Matrix multiplication to get twist from a set of wheel velocities
    matrix_4x3T_4x1_multiply_vector(&OMNI_H_PINV, u, v);
}

void omni_robot_split_vels(const struct matrix_4x1 * u, struct type_wheel_velocities u_out[])
{
    // Input velocity vector should be in the order: [0]FL, [1]FR, [2]RR, [3]RL
    // This follows the same setup as the one used in Chapter 13 of Modern Robotics
    u_out[0].right = u->data[1];
    u_out[0].left = u->data[0];
    u_out[1].right = u->data[2];
    u_out[1].left = u->data[3];
}

void omni_robot_join_vels(const struct type_wheel_velocities u[], struct matrix_4x1 * u_out)
{
    // Output velocity vector should be in the order: [0]FL, [1]FR, [2]RR, [3]RL
    // This follows the same setup as the one used in Chapter 13 of Modern Robotics
    u_out->data[0] = u[0].left;
    u_out->data[1] = u[0].right;
    u_out->data[2] = u[1].right;
    u_out->data[3] = u[1].left;
}

/// @brief Updates omni robot odometry
/// @param v - an input twist
/// @param pose [out] - the robot's updated pose
void omni_robot_update_odometry(const struct type_twist * v, float * x, float * y, float * theta, const float time_step)
{
    // multiply twist components by timestep
    v->wz = v->wz * timestep;
    v->vx = v->vx * timestep;
    v->vy = v->vy * timestep;

    // Get change in position from input twist in body frame
    float wz = 0.0f, vx = 0.0f, vy = 0.0f;
    if(fabs(v->wz) < 0.000001f)     // Precision up to 10^-6 for determining if a number should be rounded down to 0.0
    {
        wz = 0.0f;
        vx = v->vx;
        vy = v->vy;
    } else
    {
        wz = v->wz;
        vx = (v->vx*sin(v->wz) + v->vy*(cos(v->wz) - 1.0f))/v->wz;
        vy = (v->vy*sin(v->wz) + v->vx*(1.0f - cos(v->wz)))/v->wz;
    }

    // Add values to a vector for computation
    struct matrix_3x1 delta_body;
    delta_body.data[0] = wz;
    delta_body.data[1] = vx;
    delta_body.data[2] = vy;

    // Transform coordinates to the fixed frame
    struct matrix_3x1 delta_fixed = {0};
    struct matrix_3x3 fixed_transform = {0};
    matrix_3x3_init(&fixed_transform,
                    1.0f, 0.0f, 0.0f,
                    0.0f, cos(*theta), sin(*theta)*(-1.0f),
                    0.0f, sin(*theta), cos(*theta));
    matrix_3x3_multiply_vector(&fixed_transform, &delta_body, &delta_fixed);

    // Update robot position and orientation
    *theta += delta_fixed.data[0];
    *x += delta_fixed.data[1];
    *y += delta_fixed.data[2];
}
