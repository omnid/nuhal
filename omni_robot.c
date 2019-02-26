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
    struct type_twist v_inc = {0.0f, 0.0f, 0.0f};
    v_inc.wz = v->wz * time_step;
    v_inc.vx = v->vx * time_step;
    v_inc.vy = v->vy * time_step;

    // Get change in position from input twist in body frame
    float wz = 0.0f, vx = 0.0f, vy = 0.0f;
    if(fabs(v_inc.wz) < 0.000001f)     // Precision up to 10^-6 for determining if a number should be rounded down to 0.0
    {
        wz = 0.0f;
        vx = v_inc.vx;
        vy = v_inc.vy;
    } else
    {
        wz = v_inc.wz;
        vx = (v_inc.vx*sin(v_inc.wz) + v_inc.vy*(cos(v_inc.wz) - 1.0f))/v_inc.wz;
        vy = (v_inc.vy*sin(v_inc.wz) + v_inc.vx*(1.0f - cos(v_inc.wz)))/v_inc.wz;
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

/// @brief Calculates Adjoint when in FORMATION control
/// @param pivot - Pose2D of pivot point
/// @param Tsb_pose2D - transform of robot relative to map frame
/// @param adjoint [out] - the Adjoint map Ad_Tbr from the robot to the pivot
void omni_robot_compute_adjoint(const struct type_pose2D pivot, const struct type_pose2D Tsb_pose2D, struct matrix_6x6 *adjoint)
{
    // First, convert the Tsb_pose2D to a 3x3 rotation matrix and a 3x1 point
    struct matrix_3x3 R_sb = {0};
    struct matrix_3x1 p_sb = {0};
    matrix_3x3_init(&R_sb,
                    cos(Tsb_pose2D.theta), sin(Tsb_pose2D.theta)*(-1.0f), 0.0f,
                    sin(Tsb_pose2D.theta), cos(Tsb_pose2D.theta),         0.0f,
                    0.0f,                  0.0f,                          1.0f);

    p_sb.data[0] = Tsb_pose2D.x;
    p_sb.data[1] = Tsb_pose2D.y;
    p_sb.data[2] = 0;

    // Transpose the rotation matrix to get R^T and calculate -R^T * p_sb
    struct matrix_3x3 R_bs = {0};
    matrix_3x3_return_transpose(&R_sb, &R_bs);
    struct matrix_3x1 p_bs = {0};
    matrix_3x3_multiply_vector(&R_bs, &p_sb, &p_bs);
    p_bs.data[0] = -p_bs.data[0];
    p_bs.data[1] = -p_bs.data[1];
    p_bs.data[2] = -p_bs.data[2];

    // Now let's create a rotation matrix and position vector for the pivot
    struct matrix_3x3 R_sr = {0};
    struct matrix_3x1 p_sr = {0};
    matrix_3x3_init(&R_sr,
        cos(pivot.theta), sin(pivot.theta)*(-1.0f), 0.0f,
        sin(pivot.theta), cos(pivot.theta),         0.0f,
        0.0f,             0.0f,                     1.0f);

    p_sr.data[0] = pivot.x;
    p_sr.data[1] = pivot.y;
    p_sr.data[2] = 0;

    // Now that we essentially have T_bs and T_sr, let's get T_br = T_bs * T_sr
    // First do R_bs * R_sr to get R_br
    struct matrix_3x3 R_br = {0};
    matrix_3x3_multiply_matrix(&R_bs, &R_sr, &R_br);

    // To get p_br, we need to do R_bs * p_sr + p_bs
    struct matrix_3x1 p_br = {0};
    matrix_3x3_multiply_vector(&R_bs, &p_sr, &p_br);
    p_br.data[0] += p_bs.data[0];
    p_br.data[1] += p_bs.data[1];
    p_br.data[2] += p_bs.data[2];

    // Now we need to calculate [p_br]R_br
    // First, compute the skew symmetic matrix of the point p_br (pose of pivot wrt the robot)
    struct matrix_3x3 p_skew = {0};
    matrix_3x3_init(&p_skew,
                    0.0f, -p_br.data[2], p_br.data[1],
                    p_br.data[2], 0.0f, -p_br.data[0],
                    -p_br.data[1], p_br.data[0], 0.0f);

    // Now, me multiply and put the result in sp_R
    struct matrix_3x3 sp_R = {0};
    matrix_3x3_multiply_matrix(&p_skew, &R_br, &sp_R);

    // Finally, we can build the adjoint map
    matrix_6x6_init(adjoint,
    R_br.data[0][0], R_br.data[0][1], R_br.data[0][2], 0.0f, 0.0f, 0.0f,
    R_br.data[1][0], R_br.data[1][1], R_br.data[1][2], 0.0f, 0.0f, 0.0f,
    R_br.data[2][0], R_br.data[2][1], R_br.data[2][2], 0.0f, 0.0f, 0.0f,
    sp_R.data[0][0], sp_R.data[0][1], sp_R.data[0][2], R_br.data[0][0], R_br.data[0][1], R_br.data[0][2],
    sp_R.data[1][0], sp_R.data[1][1], sp_R.data[1][2], R_br.data[1][0], R_br.data[1][1], R_br.data[1][2],
    sp_R.data[2][0], sp_R.data[2][1], sp_R.data[2][2], R_br.data[2][0], R_br.data[2][1], R_br.data[2][2]);
}

/// @brief Computes the body twist of the robot V_body from the pivot Twist V_piv using the adjoint - used in FORMATION control
/// @param adjoint - Adjoint map of the pivot frame wrt the body frame
/// @param V_piv - Twist wrt the pivot frame
/// @param V_body [out] - Twist wrt the body frame
void omni_robot_compute_Vb(const struct matrix_6x6 *adjoint, const struct type_twist * V_piv, struct type_twist * V_body)
{
    struct matrix_6x1 Vr = {0};
    struct matrix_6x1 Vb = {0};
    Vr.data[2] = V_piv->wz;
    Vr.data[3] = V_piv->vx;
    Vr.data[4] = V_piv->vy;
    matrix_6x6_multiply_vector(adjoint, &Vr, &Vb);
    V_body->wz = Vb.data[2];
    V_body->vx = Vb.data[3];
    V_body->vy = Vb.data[4];
}
