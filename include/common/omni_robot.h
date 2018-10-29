#ifndef COMMON_ALL_OMNI_CALCULATIONS_H
#define COMMON_ALL_OMNI_CALCULATIONS_H
#include "common/type.h"

/// @brief calculations and important parameters for the omni robot
#ifdef __cplusplus
extern "C" {
#endif

/// @brief physical parameters for the omni robot
struct omni_robot
{
  float wheel_radius;   // Radius of the mecanum wheel
  float body_length;    // Length of the robot
  float body_width;     // Width of the robot
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
/// @param uFront [out] - output velocites of the front wheels
/// @param uRear [out] - output velocitied of the rear wheels
void omni_robot_split_vels(const struct matrix_4x1 * u, struct type_wheel_velocities * uFront, struct type_wheel_velocities * uRear);

/// @brief Takes the front and rear sets of wheel velocities and joins them
/// correctly into a single 4x1 vector that can be used for matrix operations
/// @param uFront - input velocites of the front wheels
/// @param uRear - input velocitied of the rear wheels
/// @param u [out] - output wheel velocity vector
void omni_robot_join_vels(const struct type_wheel_velocities * uFront, const struct type_wheel_velocities * uRear, struct matrix_4x1 * u);


#ifdef __cplusplus
}
#endif

#endif
