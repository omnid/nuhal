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

void omni_robot_split_vels(const struct matrix_4x1 * u, struct type_wheel_velocities * uFront, struct type_wheel_velocities * uRear)
{
    // Input velocity vector should be in the order: [0]FL, [1]FR, [2]RR, [3]RL
    // This follows the same setup as the one used in Chapter 13 of Modern Robotics
    uFront->right = u->data[1];
    uFront->left = u->data[0];
    uRear->right = u->data[2];
    uRear->left = u->data[3];
}

void omni_robot_join_vels(const struct type_wheel_velocities * uFront, const struct type_wheel_velocities * uRear, struct matrix_4x1 * u)
{
    // Output velocity vector should be in the order: [0]FL, [1]FR, [2]RR, [3]RL
    // This follows the same setup as the one used in Chapter 13 of Modern Robotics
    u->data[0] = uFront->left;
    u->data[1] = uFront->right;
    u->data[2] = uRear->right;
    u->data[3] = uRear->left;
}

/// @brief Updates omni robot odometry
/// @param v - an input twist
/// @param pose [out] - the robot's updated pose
void omni_robot_update_odometry(const struct type_twist * v, struct omni_robot * pose, const float time_step)
{
    // Get change in position from input twist in body frame
    float wz = 0.0f, vx = 0.0f, vy = 0.0f;
    if(fabs(v->wz) < 0.0001f)     // Precision up to 10^-5 for determining if a number should be rounded down to 0.0
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
    // printf("Delta Body:\nWz: %f, Vx: %f, Vy: %f\n", wz, vx, vy);

    // Transform coordinates to the fixed frame
    struct matrix_3x1 delta_fixed = {0};
    struct matrix_3x3 fixed_transform = {0};
    matrix_3x3_init(&fixed_transform,
                    1.0f, 0.0f, 0.0f,
                    0.0f, cos(pose->theta_pos), sin(pose->theta_pos)*(-1.0f),
                    0.0f, sin(pose->theta_pos), cos(pose->theta_pos));
    matrix_3x3_multiply_vector(&fixed_transform, &delta_body, &delta_fixed);
    // printf("Delta Fixed:\nWz: %f, Vx: %f, Vy: %f\n", delta_fixed.data[0], delta_fixed.data[1], delta_fixed.data[2]);

    // Update robot position and orientation
    pose->theta_pos += (delta_fixed.data[0] * time_step);
    pose->x_pos += (delta_fixed.data[1] * time_step);
    pose->y_pos += (delta_fixed.data[2] * time_step);
    // printf("New Pose:\nWz: %f, Vx: %f, Vy: %f\n---------------------------\n\n", pose->theta_pos, pose->x_pos, pose->y_pos);
}
