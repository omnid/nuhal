#include "common/matrix.h"
#include "common/parameters.h"
#include "common/omni_robot.h"
#include <math.h>
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
    {-0.0620117, 0.0254, -0.0254},
    {0.0620117, 0.0254, 0.0254},
    {0.0620117, 0.0254, -0.0254},
    {-0.0620117, 0.0254, 0.0254}
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
