#ifndef COMMON_ALL_DELTA_KINEMATICS_H
#define COMMON_ALL_DELTA_KINEMATICS_H
#include"common/type.h"

/// @brief kinematic equations/calculations for the delta robot
#ifdef __cplusplus
extern "C" {
#endif

struct matrix_3x3;

/// @brief physical parameters for delta robot
/// @see delta_robot.pdf
struct delta_robot
{
    float base_radius; // radius of the delta robot base (|r|)
    float platform_radius; // radius of the end-effector platform (|q|)
    float lower_leg_length; // lower leg length (|L|)
    float upper_leg_length; // upper leg length (|M|)
};



/// @brief given the position of the delta robot end effector,
/// get the corresponding joint angles. these angles are before the spring
/// @param params - physical parameters describing the delta robot
/// @param pos - position of the end-effector
/// @param out [out] - the joint angles corresponding to the platform position
void delta_robot_inverse_kinematics(const struct delta_robot * params,
                                   const struct type_linear_position * pos,
                                   struct type_angular_position * out);

/// @brief given the joint angles of the delta robot end effector (after spring)
/// get the corresponding platform position
/// @param params - physical parameters describing the delta robot
/// @param angles - delta robot joint angles in radians
/// @param out [out] - platform position  corresponding to the joint angles
void delta_robot_forward_kinematics(const struct delta_robot * params,
                           const struct type_angular_position * angles,
                           struct type_linear_position * out);

/// @brief compute the Jacobian, which maps joint velocities to end-effector
/// velocities
/// @param params - the delta robot parameters
/// @param pos - the position of the end effector
/// @param angle - the joint angles
/// @param out [out] - the jacobian
void delta_robot_jacobian(const struct delta_robot * params,
                                  const struct type_linear_position * pos,
                                  const struct type_angular_position * angle,
                                  struct matrix_3x3 * out);

/// @brief compute the inverse jacobian, which maps end-effector velocities
///  to joint velocities
/// @param params - the delta robot parameters
/// @param pos - the position of the end effector
/// @param angle - the joint angles
/// @param out [out] - the inverse jacobian
void delta_robot_inverse_jacobian(const struct delta_robot * params,
                                  const struct type_linear_position * pos,
                                  const struct type_angular_position * angle,
                                  struct matrix_3x3 * out);


/// @brief use the inverse jacobian to compute joint velocities
/// given end-effector velocities
/// @param params - the physical parameters describing the delta robot
/// @param pos - the position of the end effector
/// @param ang - the joint angles, in radians
/// @param vel - the desired end-effector velocity
/// @param out - the joint velocities corresponding to the end-effector velocity
///              in rad/sec
void delta_robot_inverse_velocity(const struct delta_robot * params,
                                  const struct type_linear_position * pos,
                                  const struct type_angular_position * angle,
                                  const struct type_linear_velocity * vel,
                                  struct type_angular_velocity * out);

/// @brief use the  Jacobian transpose to compute joint torques 
/// given end-effector forces, assuming that the robot is stationary
/// @param params - the physical parameters describing the delta robot
/// @param pos - the position of the end effector
/// @param ang - the joint angles, in radians
/// @param force - the desired end-effector force
/// @param out - the joint torques corresponding to the end-effector forces, N/m
void delta_robot_inverse_force(const struct delta_robot * params,
                               const struct type_linear_position * pos,
                               const struct type_angular_position * angle,
                               const struct type_linear_force * force,
                               struct type_torques * out);


/// @brief use the forward jacobian to compute end-effector velocities
/// given joint velocities
/// @param params - the physical parameters describing the delta robot
/// @param pos - the position of the end effector
/// @param ang - the joint angles in radians
/// @param vel - the desired joint velocities
/// @param out - the end-effector velocities corresponding to joint velocities
void delta_robot_forward_velocity(const struct delta_robot * params,
                         const struct type_linear_position * pos,
                         const struct type_angular_position * angle,
                         const struct type_angular_velocity * vel,
                         struct type_linear_velocity * out);

/// @brief use the inverse Jacobian transpose to compute end-effector torques
/// from joint torques, assuming the robot is stationary
/// @param params - the physical parameters describing the delta robot
/// @param pos - the position of the end effector
/// @param ang - the joint angles, in radians
/// @param torque - the joint torques corresponding to the end-effector forces
/// @param out - the desired end-effector force
void delta_robot_forward_force(const struct delta_robot * params,
                               const struct type_linear_position * pos,
                               const struct type_angular_position * angle,
                               const struct type_torques * torque,
                               struct type_linear_force * out);
#ifdef __cplusplus
}
#endif

#endif

