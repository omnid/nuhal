#ifndef COMMON_PARAMETERS_INCLUDE_GUARD_H
#define COMMON_PARAMETERS_INCLUDE_GUARD_H
/// @brief store general parameters that are useful to the whole system
#include "common/utilities.h"
#include "common/matrix.h"

/// @brief joint limits for the arm angle
#define PARAMETERS_ARM_MIN_RAD DEGREES_TO_RADIANS(-5.0f)
#define PARAMETERS_ARM_MAX_RAD DEGREES_TO_RADIANS(80.0f)

/// @brief maximum velocity of the arm
#define PARAMETERS_ARM_MAX_RAD_SEC 10.0f

// amps of current,
// actual max is 6
#define PARAMETERS_ARM_MAX_CURRENT 6.0f
/// @brief workspace limits, in m.  origin is at the center of the base
/// so limits are x (-PARAMETERS_WORK_X_MM,LIMIT_WORK_X_M)
/// so limits are y (-PARAMETERS_WORK_Y_MM,LIMIT_WORK_Y_M)
/// so limits are y (-PARAMETERS_WORK_Z_MIN_MM,LIMIT_WORK_Z_MAX_M)
#define PARAMETERS_WORK_X_M 0.070f
#define PARAMETERS_WORK_Y_M 0.070f
#define PARAMETERS_WORK_Z_MIN_M 0.20f
#define PARAMETERS_WORK_Z_MAX_M 0.45f

/// physical parameters of the delta robot (in meters)
#define PARAMETERS_DELTA_BASE_RADIUS  0.18f
#define PARAMETERS_DELTA_PLATFORM_RADIUS  0.062f
#define PARAMETERS_DELTA_LOWER_LEG_LENGTH 0.200f
#define PARAMETERS_DELTA_UPPER_LEG_LENGTH 0.368f

/// height of the calibration fixture (m), from the drive axes to the axes
/// of the joints connected to the platform
#define PARAMETERS_CALIBRATION_FIXTURE_HEIGHT 0.2467737f

/// mass of the robot above the spring, in kg, excluding the gimbal
#define PARAMETERS_CALIBRATION_SPRUNG_MASS 0.9f

/// max wheel speed is 285 rpm - corresponds to sending a data byte of '127'
#define PARAMETERS_WHEEL_MAX_SPEED 127u


/// Frequencies for some control loop

/// @brief joint control loop in hz. The speed of this loop is limited
/// by the speed of the motor firmware.
/// it can take up to 2 ms to get a response for
/// a motor command.  It could be possible to circumvent this limitation by
/// not waiting for the response; however, the commands would then fill the
/// motor's queue and be buffered, which does not help much
#define PARAMETERS_JOINT_CONTROL_HZ 800u

/// @brief frequency of the control loop on the main delta controller
#define PARAMETERS_DELTA_CONTROL_LOOP_HZ 100u

/// @brief wheel control loop in hz.
#define PARAMETERS_WHEEL_CONTROL_HZ 400u

/// @brief omni control loop in hz.
#define PARAMETERS_OMNI_CONTROL_LOOP_HZ 100u

/// @brief Mobile base configuration (units are in m)
#define PARAMETERS_OMNI_WHEEL_RADIUS 0.1016f
#define PARAMETERS_OMNI_LENGTH 0.2064f
#define PARAMETERS_OMNI_WIDTH 0.2159f

#define PARAMETERS_SPRING_CONSTANT 17.0f

#endif
