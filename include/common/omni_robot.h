#ifndef COMMON_ALL_OMNI_CALCULATIONS_H
#define COMMON_ALL_OMNI_CALCULATIONS_H
#include "common/type.h"

/// @brief calculations and important parameters for the omni robot
#ifdef __cplusplus
extern "C" {
#endif

/// @brief Position and orientation of the omni robot
struct omni_robot
{
  float x_pos;          // x position
  float y_pos;          // y position
  float theta_pos;      // orientation
};

/// @brief Converts an input twist to a set of wheel velocities.
/// See Eq 13.10 in Modern Robotics
/// @param v - input twist
/// @param u [out] - output wheel velocities
void omni_robot_twist_to_vels(const struct matrix_3x1 * v, struct matrix_4x1 * u);

/// @brief Converts an input set of wheel velocities to a twist.
/// See Eq 13.10 in Modern Robotics
/// @param u - input wheel velocities
/// @param v [out] - output twist
void omni_robot_vels_to_twist(const struct matrix_4x1 * u, struct matrix_3x1 * v);

/// @brief Takes a complete set of wheel velocities and splits them into two
/// correct front and rear subsets
/// @param u - input wheel velocity vector
/// @param u_out [out] - array of output wheel velocities
void omni_robot_split_vels(const struct matrix_4x1 * u, struct type_wheel_velocities u_out[]);

/// @brief Takes the front and rear sets of wheel velocities and joins them
/// correctly into a single 4x1 vector that can be used for matrix operations
/// @param u - input array of wheel velocities
/// @param u_out [out] - output wheel velocity vector
void omni_robot_join_vels(const struct type_wheel_velocities u[], struct matrix_4x1 * u_out);

/// @brief Updates omni robot odometry for a given time step. Trnasforms the robot's body
/// twist to the space frame and then integrates over time to update the pose.
/// @param v - an input twist
/// @param pose [out] - the robot's pose that is updated
void omni_robot_update_odometry(const struct type_twist * v, struct omni_robot * pose, float time_step);

#ifdef __cplusplus
}
#endif

#endif
